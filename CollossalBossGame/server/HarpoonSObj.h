#pragma once

#include "ServerObject.h"
#include "PlayerSObj.h"

enum HarpoonState {
	HS_FLYING = 0,
	HS_GRAPPLE = 1,
	HS_HARPOON = 2,
	HS_DEAD = 3
};

class HarpoonSObj : public ServerObject
{
public:
	HarpoonSObj(uint id, Model modelNum, Point_t pos, Vec3f initialForce, int diameter, PlayerSObj * pso);
	virtual ~HarpoonSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_HARPOON; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);
	void gracefullyfail(ServerObject * creator, ServerObject * target);
	char serialbuffer[100];

	HarpoonState state;
	int diameter;
	int creatorid;
	int targetid;
private:
	PhysicsModel *pm;
	Model modelNum;
	Vec3f dist2target;
};