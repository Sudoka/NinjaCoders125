#include "StunGunSObj.h"
#include "ShooterSObj.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"
#include "defs.h"
#include <math.h>

StunGunSObj::StunGunSObj(uint id, Model modelNum, Point_t pos, Vec3f initialForce, int diameter, PlayerSObj * pso) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new Harpoon %d ", id);
	
	Quat_t rot = Quat_t();
	Box bxVol = Box((float)-(diameter/2), (float)-(diameter/2), (float)-(diameter/2), (float)diameter, (float)diameter, (float)diameter);

	pm = new PhysicsModel(pos, rot, 50);
	getCollisionModel()->add(new AabbElement(bxVol));
	pm->applyForce(initialForce);

	this->modelNum = modelNum;
	state = SGS_FLYING;
	this->diameter = diameter;
	this->creatorid = pso->getId();
	this->targetid = -1;

	this->setFlag(IS_FLOATING, 1); // YAY IT'S A LASER PEWPEW
	// this->setFlag(IS_PASSABLE, 1); // YAY IT'S A GHOST PEWPEW 
}


StunGunSObj::~StunGunSObj(void)
{
	delete pm;
}

void StunGunSObj::gracefullyfail(ServerObject * creator, ServerObject * target) {
	// TODO: Implement
	// assert(false && "WTF");
	return;
}

bool StunGunSObj::update() {
	if(this->state == SGS_FLYING) {
		return false;
	}  else if(this->state == SGS_DEAD) {
		return true;
	} else if(this->state == SGS_STUNNING) {
		// Ensure that the target being stunned still exists
		ServerObject * creator = SOM::get()->find(creatorid);
		ServerObject * target = SOM::get()->find(targetid);
		if(creator == NULL || target == NULL) {
			gracefullyfail(creator, target);
			return true;
		}
		// 

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
	} else {
		assert(false && "Dunno how you got here =[");
		return false;
	}
}

int StunGunSObj::serialize(char * buf) {
	// All this ObjectState stuff is extra. TODO: Remove extra. 
	*(int *)buf = diameter;
	buf = buf + 4;
	*(int *)buf = creatorid;
	buf = buf + 4;

	ObjectState *state = (ObjectState*)buf;
	state->modelNum = modelNum;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(ObjectState));

		vector<CollisionElement*>::iterator cur = getCollisionModel()->getStart(),
			end = getCollisionModel()->getEnd();

		collState->totalBoxes = min(end - cur, maxBoxes);

		for(int i=0; i < collState->totalBoxes; i++, cur++) {
			//The collision box is relative to the object's frame-of-ref.  Get the non-relative collision box
			collState->boxes[i] = ((AabbElement*)(*cur))->bx + pm->ref->getPos();
		}

		return pm->ref->serialize(buf + sizeof(ObjectState) + sizeof(CollisionState)) + sizeof(ObjectState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
	}
}

void StunGunSObj::onCollision(ServerObject *obj, const Vec3f &collNorm) {
	/*
	if(obj->getId() == creatorid && this->state == HS_HARPOON) {
		this->state = SGS_DEAD;
		((PlayerSObj *)SOM::get()->find(this->creatorid))->clearAccessory();
		ServerObject * target = SOM::get()->find(targetid);
		if(target == NULL) {
			gracefullyfail(NULL, target);
		}
		target->setFlag(IS_FLOATING, 0);
		target->setFlag(IS_FALLING, 1);
	} else if(this->state == HS_FLYING && obj->getId() != creatorid) {
		if(obj->getFlag(IS_STATIC)) {
			this->setFlag(IS_STATIC, 1);
			this->pm->accel = Vec3f();
			this->pm->vel = Vec3f();
			this->state = HS_GRAPPLE;
			this->targetid = obj->getId();
		} else {
			// this->setFlag(IS_STATIC, 1);
			this->state = HS_HARPOON;
			this->targetid = obj->getId();
			this->dist2target = obj->getPhysicsModel()->ref->getPos() - this->getPhysicsModel()->ref->getPos();
		}
	}
	*/
}