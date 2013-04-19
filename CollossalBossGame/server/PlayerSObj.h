#pragma once
#include "ServerObject.h"
#include "Action.h"

class PlayerSObj : public ServerObject
{
public:
	PlayerSObj(uint id);
	virtual ~PlayerSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	void deserialize(char* newInput);
	virtual ObjectType getType() { return OBJ_PLAYER; }

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	inputstatus istat;

	// Configuration options
	float jumpDist;
	int movDamp;
};

