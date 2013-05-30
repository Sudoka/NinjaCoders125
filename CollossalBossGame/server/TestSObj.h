#pragma once
#include "ServerObject.h"
#include "Action.h"
#include "NetworkData.h"

#define TEST_NORTH 0
#define TEST_SOUTH 1
#define TEST_EAST  2
#define TEST_WEST  3
#define TEST_STILL 4

class TestSObj : public ServerObject {
public:
	TestSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, int dir = TEST_STILL);
	virtual ~TestSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_GENERAL; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal) {}

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	Model modelNum;
	int dir;
	int t;
	Box bxVol;
	uint testBoxIndex;
};

