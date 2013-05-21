#include "HeadSObj.h"
#include "ServerObjectManager.h"
#include "PhysicsEngine.h"
#include "PlayerSObj.h"
#include "BulletSObj.h"

HeadSObj::HeadSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master) :
											MonsterPartSObj(id, modelNum, pos, rot, master)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new HeadSObj %d\n", id);

}


HeadSObj::~HeadSObj(void)
{
}

bool HeadSObj::update() {
	return false;
}

void HeadSObj::onCollision(ServerObject *obj, const Vec3f &collisionNormal) {
//	// if the monster is attacking, it pushes everything off it on the last attack frame
////	if (attackCounter == (attackBuffer + attackFrames))
//	if (actionState == RAGE_ACTION_H)
//	{
//		Vec3f up = (PE::get()->getGravVec() * -1);
//		obj->getPhysicsModel()->applyForce((up + collisionNormal)*(float)pushForce);
//	}
//
	// if I collided against the player, AND they're attacking me, loose health
	if(obj->getType() == OBJ_PLAYER)
	{	
		PlayerSObj* player = reinterpret_cast<PlayerSObj*>(obj);
		health-= player->damage;
		
		if(this->health < 0) health = 0;
		if(this->health > 100) health = 100; // would this ever be true? o_O

		// I have been attacked! You'll regret it in the next udpate loop player! >_>
		attacked = player->damage > 0;
	}

	if(obj->getType() == OBJ_BULLET) {
		BulletSObj* bullet = reinterpret_cast<BulletSObj*>(obj);
		health -= bullet->damage;
		if(this->health < 0) health = 0;
		if(this->health > 100) health = 100;
	}
}