#pragma once
#include "ServerObject.h"

class MonsterSObj : public ServerObject
{
public:
	MonsterSObj(uint id, Model modelNum, Point_t pos, Rot_t rot);
	virtual ~MonsterSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_MONSTER; }
	virtual void onCollision(ServerObject *obj);

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	Model modelNum;
	int health;
};

