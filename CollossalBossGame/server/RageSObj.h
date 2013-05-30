#pragma once
#include "ServerObject.h"
class RageSObj :
	public ServerObject
{
public:
	RageSObj(uint id, Point_t pos);
	virtual ~RageSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_RAGE; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal) { } // todo for now

	char serialbuffer[100];

	static int lifetime;

private:
	PhysicsModel *pm;
	Model modelNum;
	int age;
};

