#include "BulletSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "defs.h"
#include <math.h>

int BulletSObj::TotalBullets = 0;

BulletSObj::BulletSObj(uint id, Model modelNum, Point_t pos, Vec3f initialForce, int dmg, int diameter, ServerObject* shooter) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new Bullet %d ", id);

	this->creator = shooter;

	Box bxVol;
	Quat_t rot = Quat_t();
	float negathing = -((float)diameter/2);

	bxVol = Box(negathing, negathing, negathing, (float)diameter, (float)diameter, (float)diameter);
	DC::get()->print(LOGFILE | TIMESTAMP, "Bullet diameter = %d\n", diameter);
	rot = Quat_t();

	pm = new PhysicsModel(pos, rot, 5);
	getCollisionModel()->add(new AabbElement(bxVol));
	pm->applyForce(initialForce);
	
	this->lastdirection = initialForce;
	this->lastdirection.normalize();
	this->basevelocity = (float)CM::get()->find_config_as_int("BULLET_VELOCITY");
	this->lifetime = CM::get()->find_config_as_int("BULLET_LIFETIME");
	health = CM::get()->find_config_as_int("BULLET_LIFETIME");
	this->modelNum = modelNum;
	this->damage = dmg;
	this->diameter = diameter;

	TotalBullets++;
}


BulletSObj::~BulletSObj(void)
{
	delete pm;
	TotalBullets--;
}

bool BulletSObj::update() {
	// Apply Force of Gravity on every time step - or not, since we wanted an arc-ing shot
	// return true when it collides with something?
	// That'll wait for onCollision, I suppose.

	this->health--;

	this->setFlag(IS_FALLING, 0); // YAY IT'S A LASER PEWPEW
	Vec3f velocity = this->getPhysicsModel()->vel;
	float velocitymagnitude = magnitude(velocity);
	Vec3f magvec = this->getPhysicsModel()->vel;
	Vec3f magacc = this->getPhysicsModel()->accel;
	if(fabs(magvec.x) < 0.1 && fabs(magvec.y) < 0.1 && fabs(magvec.z) < 0.1 && fabs(magacc.x) < 0.1 && fabs(magacc.y) < 0.1 && fabs(magacc.z) < 0.1) {
		//return true;
	}

	this->getPhysicsModel()->vel = this->lastdirection * this->basevelocity;

	if(this->health > 0) {
		return false;
	} else {
		return true;
		//return false;
	}
}

int BulletSObj::serialize(char * buf) {
	// All this ObjectState stuff is extra. TODO: Remove extra. 
	*(int *)buf = diameter;
	buf = buf + 4;
	*(int *)buf = GREEN;
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

void BulletSObj::onCollision(ServerObject *obj, const Vec3f &collNorm) {
	if(obj->getType() == OBJ_FIREBALL || obj->getType() == OBJ_BULLET || obj->getType() == OBJ_HARPOON) {
		return;
	}

	Vec3f olddir = this->lastdirection;
	float oldvel = this->basevelocity;

	Vec3f coll = Vec3f(collNorm.x, collNorm.y, collNorm.z);
	coll.normalize();
	this->lastdirection = this->lastdirection + coll + coll;
	this->basevelocity = this->basevelocity;
}