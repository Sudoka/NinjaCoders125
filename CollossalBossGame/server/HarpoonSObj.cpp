#include "HarpoonSObj.h"
#include "ShooterSObj.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"
#include "defs.h"
#include <math.h>

HarpoonSObj::HarpoonSObj(uint id, Model modelNum, Point_t pos, Vec3f initialForce, int dmg, int diameter, PlayerSObj * pso) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new Harpoon %d ", id);
	
	Quat_t rot = Quat_t();
	Box bxVol = Box(-(diameter/2), -(diameter/2), -(diameter/2), diameter, diameter, diameter);

	pm = new PhysicsModel(pos, rot, 50);
	pm->addBox(bxVol);
	pm->applyForce(initialForce);

	this->modelNum = modelNum;
	state = HS_FLYING;
	this->damage = dmg;
	this->diameter = diameter;
	this->creatorid = pso->getId();

	this->setFlag(IS_FLOATING, 1); // YAY IT'S A LASER PEWPEW
}


HarpoonSObj::~HarpoonSObj(void)
{
	delete pm;
}

void gracefullyfail(ServerObject * creator, ServerObject * target) {
	// TODO: Implement
	assert(false && "WTF");
}

bool HarpoonSObj::update() {
	if(this->state == HS_FLYING) {
		return false;
	}  else if(this->state == HS_DEAD) {
		return true;
	} else if(this->state == HS_HARPOON) {
		ServerObject * creator = SOM::get()->find(creatorid);
		ServerObject * target = SOM::get()->find(targetid);
		if(creator == NULL || target == NULL) {
			gracefullyfail(creator, target);
			return true;
		}
		Vec3f v2creator = creator->getPhysicsModel()->ref->getPos() - this->getPhysicsModel()->ref->getPos();
		v2creator.normalize();
		Vec3f diffvector = this->pm->ref->getPos() - creator->getPhysicsModel()->ref->getPos();
		if(magnitude(diffvector) < magnitude(v2creator)) {
			this->getPhysicsModel()->ref->setPos(creator->getPhysicsModel()->ref->getPos());
		} else {
			this->getPhysicsModel()->vel = Vec3f();
			this->getPhysicsModel()->accel = (v2creator*2);
		}

		Vec3f followpos = this->getPhysicsModel()->ref->getPos() + this->dist2target;
		target->setFlag(IS_FLOATING, 1);
		target->getPhysicsModel()->ref->setPos(followpos);

		return false;
	} else if(this->state == HS_GRAPPLE) {
		ServerObject * creator = SOM::get()->find(creatorid);
		if(creator == NULL) {
			gracefullyfail(creator, NULL);
			return true;
		}
		Vec3f diffvector = this->pm->ref->getPos() - creator->getPhysicsModel()->ref->getPos();
		if(fabs(diffvector.x) < 15 && fabs(diffvector.y) < 15 && fabs(diffvector.z) < 15) {
			creator->getPhysicsModel()->accel = Vec3f();
			creator->getPhysicsModel()->vel = Vec3f();
		} else {
			diffvector.normalize();
			creator->getPhysicsModel()->accel = (diffvector/2);
			creator->setFlag(IS_FLOATING, 1);
		}
		return false;
	} else {
		assert(false && "Dunno how you got here =[");
		return false;
	}
}

int HarpoonSObj::serialize(char * buf) {
	// All this ObjectState stuff is extra. TODO: Remove extra. 
	*(int *)buf = diameter;
	buf = buf + 4;

	ObjectState *state = (ObjectState*)buf;
	state->modelNum = modelNum;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(ObjectState));

		vector<Box> objBoxes = pm->colBoxes;

		collState->totalBoxes = min(objBoxes.size(), maxBoxes);

		for (int i=0; i<collState->totalBoxes; i++)
		{
			collState->boxes[i] = objBoxes[i] + pm->ref->getPos(); // copying applyPhysics
		}

		return pm->ref->serialize(buf + sizeof(ObjectState) + sizeof(CollisionState)) + sizeof(ObjectState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
	}
}

void HarpoonSObj::onCollision(ServerObject *obj, const Vec3f &collNorm) {
	if(obj->getId() == creatorid && this->state == HS_HARPOON) {
		this->state = HS_DEAD;
		((PlayerSObj *)SOM::get()->find(this->creatorid))->clearAccessory();
		ServerObject * target = SOM::get()->find(targetid);
		if(target == NULL) {
			gracefullyfail(NULL, target);
		}
		target->setFlag(IS_FLOATING, 0);
	} else if(this->state == HS_FLYING && obj->getId() != creatorid) {
		if(obj->getFlag(IS_STATIC)) {
			this->setFlag(IS_STATIC, 1);
			this->state = HS_GRAPPLE;
		} else {
			// this->setFlag(IS_STATIC, 1);
			this->state = HS_HARPOON;
			this->targetid = obj->getId();
			this->dist2target = obj->getPhysicsModel()->ref->getPos() - this->getPhysicsModel()->ref->getPos();
		}
	}
}