#pragma once

#include "ServerObject.h"
#include "PlayerSObj.h"

enum StunGunState {
	SGS_FLYING = 0,
	SGS_STUNNING = 1,
	SGS_DEAD = 2
};

class StunGunSObj : public ServerObject
{
public:
	StunGunSObj(uint id, Model modelNum, Point_t pos, Vec3f initialForce, int diameter, PlayerSObj * pso);
	virtual ~StunGunSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_STUNGUN; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);
	char serialbuffer[100];

	StunGunState state;
	int diameter;
	int creatorid;
	int targetid;
private:
	PhysicsModel *pm;
	Model modelNum;
	Vec3f dist2target;
};