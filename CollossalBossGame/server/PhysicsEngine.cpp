#include "PhysicsEngine.h"
#include "ConfigurationManager.h"
#include "CollisionModel.h"
#define TIMESTEP 5

//Movement Defines
#define MAX_VEL 5.0f

//Collision Defines
#define SMALLRADIUS 5.0f
#define LARGERADIUS 30.0f

//Static members
PhysicsEngine *PhysicsEngine::pe;

PhysicsEngine::PhysicsEngine(void)
{
	// Configuration options
	gravMag = CM::get()->find_config_as_float("GRAVITY_FORCE");

	xPos = yPos = zPos = 500;
	xNeg = yNeg = zNeg = 0;
	gravVec = Vec3f(0, -1, 0);
	gravDir = DOWN;
	this->setGravDir(DOWN);
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
	if(obj->getFlag(IS_FALLING)) {
		//gravity
		mdl->applyAccel(gravVec*gravMag);
	} else {
		//friction
	}


	//Update position
	mdl->lastPos = mdl->ref->getPos();
	//if(mdl->ref->getPos().y <= 0) mdl->lastPosOnGround = mdl->ref->getPos();
	float dx = 0.5f * mdl->accel.x * dt * dt + mdl->vel.x * dt,
		  dy = 0.5f * mdl->accel.y * dt * dt + mdl->vel.y * dt,
		  dz = 0.5f * mdl->accel.z * dt * dt + mdl->vel.z * dt;
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
		mdl->frictCoeff = AIR_FRICTION;
	}

	return true;	//We'll add a detection for has-moved later
}

void PhysicsEngine::applyPhysics(ServerObject *obj1, ServerObject *obj2)
{
	PhysicsModel *mdl1 = obj1->getPhysicsModel(),
				 *mdl2 = obj2->getPhysicsModel();
	if(mdl1 == NULL || mdl2 == NULL) {
		return;
	}

	// go through all the boxes of obj1 
	vector<Box> obj1Boxes = mdl1->colBoxes;
	vector<Box> obj2Boxes = mdl2->colBoxes;
	Box bx1, bx2;
	DIRECTION dir;
	Vec3f shift, shift1, shift2;
	for(std::vector<Box>::iterator box1 = obj1Boxes.begin(); box1 != obj1Boxes.end(); ++box1) {
		// go through all the boxes of obj
		for(std::vector<Box>::iterator box2 = obj2Boxes.begin(); box2 != obj2Boxes.end(); ++box2)
		{
			//applyPhysics(obj1, obj2, *box1, *box2);
			bx1 = *box1 + mdl1->ref->getPos();
			bx2 = *box2 + mdl2->ref->getPos();

			//Check for collision
			if(!areColliding(bx1,bx2)) {
				return;
			}

			//Passable objects or static pairs cannot be collided with
			if((obj1->getFlag(IS_PASSABLE) || obj2->getFlag(IS_PASSABLE)) ||
					(obj1->getFlag(IS_STATIC) && obj2->getFlag(IS_STATIC))) {
				obj1->onCollision(obj2, Vec3f());
				obj2->onCollision(obj1, Vec3f());
				return;
			}

			getCollisionInfo(&shift, &dir, bx1, bx2);

			//Handle not-falling status
			if(dir == gravDir) {
				obj1->setFlag(IS_FALLING, false);
				obj1->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
			} else if(flip(dir) == gravDir) {
				obj2->setFlag(IS_FALLING, false);
				obj2->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
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
		}
	}
}

void PhysicsEngine::applyPhysics(ServerObject *obj1, ServerObject *obj2, Box b1, Box b2) {
	PhysicsModel *mdl1 = obj1->getPhysicsModel(),
				 *mdl2 = obj2->getPhysicsModel();


	Box oldBx1 = b1 + mdl1->ref->getPos(),
		oldBx2 = b2 + mdl2->ref->getPos();

	Vec3f collNorm1 = Vec3f(),
		  collNorm2 = Vec3f();

	//Check for collision
	// This part works with negative height, width, length
	Vec3f minCorner1 = Vec3f(	min(oldBx1.x + oldBx1.w, oldBx1.x), 
								min(oldBx1.y + oldBx1.h, oldBx1.y), 
								min(oldBx1.z + oldBx1.l, oldBx1.z));

	Vec3f maxCorner1 = Vec3f(	max(oldBx1.x + oldBx1.w, oldBx1.x), 
								max(oldBx1.y + oldBx1.h, oldBx1.y), 
								max(oldBx1.z + oldBx1.l, oldBx1.z));

	Vec3f minCorner2 = Vec3f(	min(oldBx2.x + oldBx2.w, oldBx2.x), 
								min(oldBx2.y + oldBx2.h, oldBx2.y), 
								min(oldBx2.z + oldBx2.l, oldBx2.z));

	Vec3f maxCorner2 = Vec3f(	max(oldBx2.x + oldBx2.w, oldBx2.x), 
								max(oldBx2.y + oldBx2.h, oldBx2.y), 
								max(oldBx2.z + oldBx2.l, oldBx2.z));

	Box bx1 = Box(minCorner1.x, minCorner1.y, minCorner1.z,
					maxCorner1.x - minCorner1.x,
					maxCorner1.y - minCorner1.y,
					maxCorner1.z - minCorner1.z);	

	Box bx2 = Box(minCorner2.x, minCorner2.y, minCorner2.z,
				maxCorner2.x - minCorner2.x,
				maxCorner2.y - minCorner2.y,
				maxCorner2.z - minCorner2.z);		

	if(!aabbCollision(bx1,bx2)) {
		return;
	}

	//Passable objects or static pairs cannot be collided with
	if((obj1->getFlag(IS_PASSABLE) || obj2->getFlag(IS_PASSABLE)) ||
			(obj1->getFlag(IS_STATIC) && obj2->getFlag(IS_STATIC))) {
		obj1->onCollision(obj2, collNorm1);
		obj2->onCollision(obj1, collNorm2);
		return;
	}
	bool applyForce = ((gravVec.x == 0) && (gravVec.y == 0) && (gravVec.z == 0));

	bool canShiftNegX = (mdl1->cdirs & EAST)  && (mdl2->cdirs & WEST),
		 canShiftPosX = (mdl1->cdirs & WEST)  && (mdl2->cdirs & EAST),
		 canShiftNegY = (mdl1->cdirs & UP)    && (mdl2->cdirs & DOWN),
		 canShiftPosY = (mdl1->cdirs & DOWN)  && (mdl2->cdirs & UP),
		 canShiftNegZ = (mdl1->cdirs & NORTH) && (mdl2->cdirs & SOUTH),
		 canShiftPosZ = (mdl1->cdirs & SOUTH) && (mdl2->cdirs & NORTH);
	bool canShiftX, canShiftY, canShiftZ;

	//Move out bounding boxes if collision occurs
	float fXShift1 = bx2.x - (bx1.x + bx1.w),
          fXShift2 = (bx2.x + bx2.w) - bx1.x,
          fXShift;//  = fabs(fXShift1) < fabs(fXShift2) ? fXShift1 : fXShift2;
    float fYShift1 = bx2.y - (bx1.y + bx1.h),
          fYShift2 = (bx2.y + bx2.h) - bx1.y,
          fYShift;//  = fabs(fYShift1) < fabs(fYShift2) ? fYShift1 : fYShift2;
    float fZShift1 = bx2.z - (bx1.z + bx1.l),
          fZShift2 = (bx2.z + bx2.l) - bx1.z,
          fZShift;//  = fabs(fZShift1) < fabs(fZShift2) ? fZShift1 : fZShift2;

	if(canShiftNegX && fabs(fXShift1) < fabs(fXShift2)) {
		fXShift = fXShift1;
		canShiftX = canShiftNegX;
	} else {
		fXShift = fXShift2;
		canShiftX = canShiftPosX;
	}

	if(fabs(fYShift1) < fabs(fYShift2)) {
		fYShift = fYShift1;
		canShiftY = canShiftNegY;
	} else {
		fYShift = fYShift2;
		canShiftY = canShiftPosY;
	}

	if(fabs(fZShift1) < fabs(fZShift2)) {
		fZShift = fZShift1;
		canShiftZ = canShiftNegZ;
	} else {
		fZShift = fZShift2;
		canShiftZ = canShiftPosZ;
	}

	//DC::get()->print(CONSOLE | LOGFILE, "Shift capabilities b/w obj%d, obj%d: (%d,%d,%d) ", obj1->getId(), obj2->getId(), canShiftX, canShiftY, canShiftZ);

	Vec3f ptObj1Shift, ptObj2Shift;
	float sign = 1.0;
    if(canShiftX && fabs(fXShift) < fabs(fYShift) && fabs(fXShift) < fabs(fZShift)) {
        //Shift by X
        if(obj1->getFlag(IS_STATIC)) {
            ptObj1Shift = Vec3f();
            ptObj2Shift = Vec3f(-fXShift, 0, 0);
        } else if(obj2->getFlag(IS_STATIC)) {
            ptObj1Shift = Vec3f(fXShift, 0, 0);
            ptObj2Shift = Vec3f();

        } else {    //Split evenly
            ptObj1Shift = Vec3f(fXShift / 2, 0, 0);
            ptObj2Shift = Vec3f(-fXShift / 2, 0, 0);
        }
		sign = fXShift < 0 ? -1.f : 1.f;
		collNorm1 = Vec3f(sign,0,0);
		collNorm2 = Vec3f(-sign,0,0);
		mdl1->vel.x = 0;
		mdl2->vel.x = 0;

		//Stop the lower object from falling
        if( ((gravDir == WEST) && (bx2.x + ptObj2Shift.x > bx1.x + ptObj1Shift.x)) ||
			((gravDir == EAST) && (bx2.x + ptObj2Shift.x < bx1.x + ptObj1Shift.x)) ) {
            obj2->setFlag(IS_FALLING, false);
			obj2->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
        } else if( ((gravDir == WEST) && (bx1.x + ptObj1Shift.x > bx2.x + ptObj2Shift.x)) ||
				   ((gravDir == EAST) && (bx1.x + ptObj1Shift.x < bx2.x + ptObj2Shift.x)) ) {
			obj1->setFlag(IS_FALLING, false);
			obj1->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
		}
    } else if(canShiftY && fabs(fYShift) < fabs(fXShift) && fabs(fYShift) < fabs(fZShift)) {
        //Shift by Y (vertical)
        if(obj1->getFlag(IS_STATIC)) {
            ptObj1Shift = Vec3f();
            ptObj2Shift = Vec3f(0, -fYShift, 0);
        } else if(obj2->getFlag(IS_STATIC)) {
            ptObj1Shift = Vec3f(0, fYShift, 0);
            ptObj2Shift = Vec3f();
        } else {    //Split evenly
            ptObj1Shift = Vec3f(0, fYShift / 2, 0);
            ptObj2Shift = Vec3f(0, -fYShift / 2, 0);
        }
		sign = fYShift < 0 ? -1.f : 1.f;
		collNorm1 = Vec3f(0,sign,0);
		collNorm2 = Vec3f(0,-sign,0);
		mdl1->vel.y = 0;
		mdl2->vel.y = 0;

		//Stop the lower object from falling
        if( ((gravDir == DOWN) && (bx2.y + ptObj2Shift.y > bx1.y + ptObj1Shift.y)) ||
			((gravDir == UP)   && (bx2.y + ptObj2Shift.y < bx1.y + ptObj1Shift.y)) ) {
            obj2->setFlag(IS_FALLING, false);
			obj2->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
        } else if( ((gravDir == DOWN) && (bx1.y + ptObj1Shift.y > bx2.y + ptObj2Shift.y)) ||
				   ((gravDir == UP)   && (bx1.y + ptObj1Shift.y < bx2.y + ptObj2Shift.y)) ) {
			obj1->setFlag(IS_FALLING, false);
			obj1->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
		}
    } else {
        //Shift by Z
        if(obj1->getFlag(IS_STATIC)) {
            ptObj1Shift = Vec3f();
            ptObj2Shift = Vec3f(0, 0, -fZShift);
        } else if(obj2->getFlag(IS_STATIC)) {
            ptObj1Shift = Vec3f(0, 0, fZShift);
            ptObj2Shift = Vec3f();
        } else {    //Split evenly
            ptObj1Shift = Vec3f(0, 0, fZShift / 2);
            ptObj2Shift = Vec3f(0, 0, -fZShift / 2);
        }
		sign = fZShift < 0 ? -1.f : 1.f;
		collNorm1 = Vec3f(0,0,sign);
		collNorm2 = Vec3f(0,0,-sign);
		mdl1->vel.z = 0;
		mdl2->vel.z = 0;

		//Stop the lower object from falling
        if( ((gravDir == SOUTH) && (bx2.z + ptObj2Shift.z > bx1.z + ptObj1Shift.z)) ||
			((gravDir == NORTH) && (bx2.z + ptObj2Shift.z < bx1.z + ptObj1Shift.z)) ) {
            obj2->setFlag(IS_FALLING, false);
			obj2->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
        } else if( ((gravDir == SOUTH) && (bx1.z + ptObj1Shift.z > bx2.z + ptObj2Shift.z)) ||
				   ((gravDir == NORTH) && (bx1.z + ptObj1Shift.z < bx2.z + ptObj2Shift.z)) ) {
			obj1->setFlag(IS_FALLING, false);
			obj1->getPhysicsModel()->frictCoeff = GROUND_FRICTION;
		}
	}

#if 0
	//Testing prints
	char *s;
	switch(gravDir) {
	case NORTH:
		s = "NORTH";
		break;
	case SOUTH:
		s = "SOUTH";
		break;
	case EAST:
		s = "EAST";
		break;
	case WEST:
		s = "WEST";
		break;
	case UP:
		s = "UP";
		break;
	case DOWN:
		s = "DOWN";
		break;
	default:
		s = "?";
	}
	if(obj1->getId() == 8 || obj2->getId() == 8) {
	DC::get()->print(CONSOLE | LOGFILE, "Direction %s, isFalling %d/%d = %s/%s\n",
		s, obj1->getId(), obj2->getId(), obj1->getFlag(IS_FALLING) ? "T" : "F", obj2->getFlag(IS_FALLING) ? "T" : "F");
	}
#endif

	//Move the objects by the specified amount
	mdl1->ref->translate(ptObj1Shift);
	mdl2->ref->translate(ptObj2Shift);
	if(applyForce) {
		mdl1->applyForce(collNorm1);
		mdl2->applyForce(collNorm2);
	}

	//Inform the logic module of the collision event
	obj1->onCollision(obj2, collNorm1);
	obj2->onCollision(obj1, collNorm2);
}

// NOTE: This expects positive width, height, length
bool PhysicsEngine::aabbCollision(const Box &bx1, const Box &bx2) {
	return !(bx1.x + bx1.w < bx2.x ||
			 bx1.y + bx1.h < bx2.y ||
			 bx1.z + bx1.l < bx2.z ||
			 bx1.x > bx2.x + bx2.w ||
			 bx1.y > bx2.y + bx2.h ||
			 bx1.z > bx2.z + bx2.l);
}

void PhysicsEngine::setGravDir(DIRECTION dir) {
	gravDir = dir;

	Vec3f newVec, crossVec;
	switch(dir) {
	case NORTH:
		newVec = Vec3f(0,0,1);
		curGravRot = Quat_t(Vec3f(1,0,0), 3 * M_PI / 2);
		break;
	case SOUTH:
		newVec = Vec3f(0,0,-1);
		curGravRot = Quat_t(Vec3f(1,0,0), -3 * M_PI / 2);
		break;
	case EAST:
		newVec = Vec3f(1,0,0);
		curGravRot = Quat_t(Vec3f(0,0,1), -3 * M_PI / 2);
		break;
	case WEST:
		newVec = Vec3f(-1,0,0);
		curGravRot = Quat_t(Vec3f(0,0,1), 3 * M_PI / 2);
		break;
	case UP:
		newVec = Vec3f(0,1,0);
		curGravRot = Quat_t(Vec3f(0,0,1), M_PI);
		break;
	case DOWN:
		newVec = Vec3f(0,-1,0);
		curGravRot = Quat_t();
		break;
	}
	/*
	DC::get()->print(LOGFILE, "Old/New vecs: (%f,%f,%f)/(%f,%f,%f) ", gravVec.x, gravVec.y, gravVec.z, newVec.x, newVec.y, newVec.z);
	cross(&crossVec, newVec, gravVec);
	crossVec.normalize();
	float ang = M_PI * 2 - angle(gravVec, newVec);
	static Quat_t q = Quat_t();
	curGravRot = Quat_t(crossVec, ang);
	q *= curGravRot;
	DC::get()->print(LOGFILE, "Axis: (%f,%f,%f) Angle: %f ", crossVec.x, crossVec.y, crossVec.z, ang * 180 / M_PI);
	Vec3f up, fwd, rt;
	getCorrectedAxes(q, &fwd, &up, &rt);
	DC::get()->print(LOGFILE, "Up/Forward/Right: (%f,%f,%f)/(%f,%f,%f)/(%f,%f,%f)\n",
		up.x, up.y, up.z, fwd.x, fwd.y, fwd.z,rt.x,rt.y, rt.z);
	*/
	gravVec = newVec;
}
