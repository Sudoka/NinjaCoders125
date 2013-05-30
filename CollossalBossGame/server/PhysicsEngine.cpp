#include "PhysicsEngine.h"
#include "ConfigurationManager.h"
#include "CollisionModel.h"
#include "PhysicsModel.h"
#include "ServerObjectManager.h"
#include "ServerObject.h"
#define TIMESTEP 5

//Movement Defines
#define MAX_VEL 5.0f

//Static members
PhysicsEngine *PhysicsEngine::pe;

PhysicsEngine::PhysicsEngine(void)
{
	// Configuration options
	gravMag = constGravMag = CM::get()->find_config_as_float("GRAVITY_FORCE");

	xPos = yPos = zPos = 500;
	xNeg = yNeg = zNeg = 0;
	gravVec = Vec3f(0, -1, 0);
	gravDir = DOWN;
	this->setGravDir(gravDir);

	frictGround = CM::get()->find_config_as_float("FRICT_GROUND");
	frictAir    = CM::get()->find_config_as_float("FRICT_AIR");\
}


PhysicsEngine::~PhysicsEngine(void)
{
}


/*
 * applyPhysics(PhysicsModel *mdl)
 * Updates the physical position of the model and returns true if this object
 * should have collision physics applied.
 */
bool PhysicsEngine::applyPhysics(ServerObject *obj) {
	float dt = TIMESTEP;

	if(obj->getFlag(IS_STATIC)) return true;

	PhysicsModel *mdl = obj->getPhysicsModel();

	//Apply gravity if not falling; otherwise, apply friction
	if(obj->getFlag(IS_FALLING) && !obj->getFlag(IS_FLOATING)) {
		//gravity
		mdl->applyAccel(gravVec*gravMag);

	} else {

		//friction
	}
	
	Vec3f surfaceShift = Vec3f();
	//if(!obj->getFlag(IS_FALLING) && !obj->getFlag(IS_FLOATING)) {
	//	ServerObject *obj = SOM::get()->find(mdl->surfaceId);
	//	if(obj != NULL) {
	//		PhysicsModel *mdlSurf = obj->getPhysicsModel();
	//		surfaceShift = mdlSurf->ref->getPos() - mdlSurf->lastPos;
	//	}
	//}

	//Update position
	//if(mdl->ref->getPos().y <= 0) mdl->lastPosOnGround = mdl->ref->getPos();
	float dx = 0.5f * mdl->accel.x * dt * dt + mdl->vel.x * dt + surfaceShift.x,
		  dy = 0.5f * mdl->accel.y * dt * dt + mdl->vel.y * dt + surfaceShift.y,
		  dz = 0.5f * mdl->accel.z * dt * dt + mdl->vel.z * dt + surfaceShift.z;
	mdl->lastPos = mdl->ref->getPos();
	mdl->ref->translate(Point_t(dx, dy, dz));



	//Update velocity
	mdl->vel.x = mdl->accel.x * dt + mdl->vel.x / mdl->frictCoeff;
	mdl->vel.y = mdl->accel.y * dt + mdl->vel.y / mdl->frictCoeff;
	mdl->vel.z = mdl->accel.z * dt + mdl->vel.z / mdl->frictCoeff;

	//Cap velocity
	float magSq = mdl->vel.x * mdl->vel.x + mdl->vel.y * mdl->vel.y + mdl->vel.z * mdl->vel.z;
	if(magSq > MAX_VEL * MAX_VEL) {
		mdl->vel *= MAX_VEL / sqrt(magSq);
	}

	//Update acceleration
	mdl->accel = Vec3f();


	//Object falls if it has moved (it may not fall after collision checks have been applied)
	if(fabs(dx) > 0 || fabs(dy) > 0 || fabs(dz) > 0) {
		obj->setFlag(IS_FALLING, true);
		mdl->frictCoeff = frictAir;
	}

	return true;	//We'll add a detection for has-moved later
}

void PhysicsEngine::applyPhysics(ServerObject *obj1, ServerObject *obj2) {
	CollisionModel *cmdl1 = obj1->getCollisionModel();
	if( obj1->getPhysicsModel() == NULL ||
		obj2->getPhysicsModel() == NULL) {
			return;
	}

	vector<CollisionElement*>::iterator cur;
	for(cur = cmdl1->getStart(); cur < cmdl1->getEnd(); ++cur) {
		switch((*cur)->getType()) {
		case CMDL_AABB:
			handleCollision(obj1, obj2, (AabbElement*)(*cur));
			break;
		case CMDL_HMAP:
			handleCollision(obj1, obj2, (HMapElement*)(*cur));
			break;
		default:
			//Unrecognized collision type
			break;
		}
	}
}


void PhysicsEngine::handleCollision(ServerObject *obj1, ServerObject *obj2, AabbElement *el) {
	//el is always from obj1
	DIRECTION dir;

	Vec3f shift;
	CollisionModel *cmdl2 = obj2->getCollisionModel();
	vector<CollisionElement*>::iterator cur, end = cmdl2->getEnd();

	
	Box bx1 = el->bx + obj1->getPhysicsModel()->ref->getPos(),
		bx2;
	Point_t pos;

	//AABBs can collide with anything
	for(cur = cmdl2->getStart(); cur < cmdl2->getEnd(); ++cur) {
		switch((*cur)->getType()) {
		case CMDL_AABB:
			bx2 = ((AabbElement*)(*cur))->bx + obj2->getPhysicsModel()->ref->getPos();

			if(areColliding(bx1, bx2)) {
				getCollisionInfo(&shift, &dir, bx1, bx2);
				handleCollision(obj1, obj2, shift, dir);
			}
			break;
		case CMDL_HMAP:
			bx2 = ((HMapElement*)(*cur))->bxTotalVolume;
			pos = obj2->getPhysicsModel()->ref->getPos();
			if(areColliding(&shift, &dir, bx1, pos, *(HMapElement*)(*cur))) {
				//TODO: Replace with appropriate collision code!
				//getCollisionInfo(&shift, &dir, bx1, bx2 + pos);
				handleCollision(obj1, obj2, shift, dir);
			}
			break;
		default:
			//Unrecognized collision type
			break;
		}
	}
}

void PhysicsEngine::handleCollision(ServerObject *obj1, ServerObject *obj2, HMapElement *el) {
	//el is always from obj1
	DIRECTION dir;
	Vec3f shift;
	CollisionModel *cmdl2 = obj2->getCollisionModel();
	vector<CollisionElement*>::iterator cur, end = cmdl2->getEnd();
	
	Box bx;
	Point_t pos = obj1->getPhysicsModel()->ref->getPos();

	//AABBs can collide with anything
	for(cur = cmdl2->getStart(); cur < cmdl2->getEnd(); ++cur) {
		switch((*cur)->getType()) {
		case CMDL_AABB:
			bx = ((AabbElement*)(*cur))->bx + obj2->getPhysicsModel()->ref->getPos();
			if(areColliding(&shift, &dir, bx, pos, *el)) {
				//TODO: Replace with appropriate collision code!
				//getCollisionInfo(&shift, &dir, el->bxTotalVolume + pos, bx);
				handleCollision(obj1, obj2, shift, dir);
			}
			break;
		case CMDL_HMAP:
			DC::get()->print("WARNING: HMap on HMap collision- skipping\n");
			break;
		default:
			//Unrecognized collision type
			break;
		}
	}
}


/*
 * Move the objects according to the specified shift and their properties
 */
void PhysicsEngine::handleCollision(ServerObject *obj1, ServerObject *obj2, const Vec3f &shift, DIRECTION dir) {
	PhysicsModel *mdl1 = obj1->getPhysicsModel(),
				 *mdl2 = obj2->getPhysicsModel();
	Vec3f shift1, shift2, axis;

	//Passable or static collision objects should not be moved because of a collision
	if((obj1->getFlag(IS_PASSABLE) || obj2->getFlag(IS_PASSABLE)) ||
			(obj1->getFlag(IS_STATIC) && obj2->getFlag(IS_STATIC))) {
					obj1->onCollision(obj2, Vec3f());
					obj2->onCollision(obj1, Vec3f());
		return;
	}

	//Handle not-falling status
	if(flip(dir) == gravDir) {
		obj1->setFlag(IS_FALLING, false);
		obj1->getPhysicsModel()->frictCoeff = frictGround;
		obj1->getPhysicsModel()->surfaceId = obj2->getId();
	} else if((dir) == gravDir) {
		obj2->setFlag(IS_FALLING, false);
		obj2->getPhysicsModel()->frictCoeff = frictGround;
		obj2->getPhysicsModel()->surfaceId = obj1->getId();
	}

	//Get the actual object shifts
	if(obj1->getFlag(IS_STATIC)) {
		shift1 = Vec3f();
		shift2 = shift * -1;
	} else if(obj2->getFlag(IS_STATIC)) {
		shift1 = shift;
		shift2 = Vec3f();
	} else {
		shift1 = shift * 0.5;
		shift2 = shift * -0.5;
	}

	//Move the objects by the specified amount
	mdl1->ref->translate(shift1);
	mdl2->ref->translate(shift2);

	//Inform the logic module of the collision event
	obj1->onCollision(obj2, dirVec(dir));
	obj2->onCollision(obj1, dirVec(flip(dir)));

	//Clear the velocity vectors in the specified direction
	axis = dirAxis(dir);
	mdl1->vel -= (mdl1->vel) * axis;	//Removes exactly one velocity component
	mdl2->vel -= (mdl2->vel) * axis;
}

void PhysicsEngine::setGravDir(DIRECTION dir) {
	Vec3f newVec, crossVec;
	switch(dir) {
	case NORTH:
		newVec = Vec3f(0,0,1);
		curGravRot = Quat_t(Vec3f(1,0,0), (float)(3 * M_PI / 2));
		break;
	case SOUTH:
		newVec = Vec3f(0,0,-1);
		curGravRot = Quat_t(Vec3f(1,0,0), (float)(-3 * M_PI / 2));
		break;
	case EAST:
		newVec = Vec3f(1,0,0);
		curGravRot = Quat_t(Vec3f(0,0,1), (float)(-3 * M_PI / 2));
		break;
	case WEST:
		newVec = Vec3f(-1,0,0);
		curGravRot = Quat_t(Vec3f(0,0,1), (float)(3 * M_PI / 2));
		break;
	case UP:
		newVec = Vec3f(0,1,0);
		curGravRot = Quat_t(Vec3f(0,0,1), (float)(M_PI));
		break;
	case DOWN:
		newVec = Vec3f(0,-1,0);
		curGravRot = Quat_t();
		break;
	default:
		gravMag = 0.0f;
		return;	//We don't want to set gravVec or gravDir, just gravMag
	}

	gravVec = newVec;
	gravDir = dir;
	gravMag = constGravMag;	//Make sure this is nonzero
}
