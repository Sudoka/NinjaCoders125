#pragma once
#include "ServerObject.h"
#include "defs.h"

class WorldSObj : public ServerObject {
public:
	WorldSObj(uint id);
	virtual ~WorldSObj(void);

	virtual bool update();
	virtual int serialize(char * buf);
	virtual PhysicsModel *getPhysicsModel() { return NULL; }
	virtual ObjectType getType()			{ return OBJ_WORLD; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal) {}

	void setGravitySwitch(bool en) { gravSwitchEnabled = en; }

private:
	int gravityTimer;
	int gravityInterval, nullInterval;
	bool gravSwitchEnabled;
	//DIRECTION gravDir;
};

