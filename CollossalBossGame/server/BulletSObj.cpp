#include "BulletSObj.h"
#include "ServerObjectManager.h"
#include "defs.h"
#include <math.h>

BulletSObj::BulletSObj(uint id, Model modelNum, Point_t pos, Vec3f initialForce, int dmg) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new Bullet %d ", id);
	Box bxVol;
	Quat_t rot = Quat_t();
	bxVol = Box(-5, -5, -5, 10, 10, 10);
	rot = Quat_t();

	pm = new PhysicsModel(pos, rot, 50);
	getCollisionModel()->add(new AabbElement(bxVol));
	pm->applyForce(initialForce);

	this->modelNum = modelNum;
	t = 0;
	health = 5;
	this->damage = dmg;
}


BulletSObj::~BulletSObj(void)
{
	delete pm;
}

bool BulletSObj::update() {
	// Apply Force of Gravity on every time step - or not, since we wanted an arc-ing shot
	// return true when it collides with something?
	// That'll wait for onCollision, I suppose.
	this->setFlag(IS_FALLING, 0); // YAY IT'S A LASER PEWPEW
	Vec3f magvec = this->getPhysicsModel()->vel;
	Vec3f magacc = this->getPhysicsModel()->accel;
	if(fabs(magvec.x) < 0.1 && fabs(magvec.y) < 0.1 && fabs(magvec.z) < 0.1 && fabs(magacc.x) < 0.1 && fabs(magacc.y) < 0.1 && fabs(magacc.z) < 0.1) {
		return true;
	}
	if(this->health > 0) {
		return false;
	} else {
		return true;
		//return false;
	}
}

int BulletSObj::serialize(char * buf) {
	// All this ObjectState stuff is extra. TODO: Remove extra. 

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

void BulletSObj::onCollision(ServerObject *obj, const Vec3f &collNorm) {
	if(obj->getType() == OBJ_GENERAL) {
		this->health = 0;
	} else {
 		if(obj->getType() == OBJ_TENTACLE) {
			this->health = 0;
		}
	}
}