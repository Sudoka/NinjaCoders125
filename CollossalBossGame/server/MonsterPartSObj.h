#pragma once
#include "ServerObject.h"
#include "MonsterSObj.h"

class MonsterPartSObj :
	public ServerObject
{
public:
	MonsterPartSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master);
	virtual ~MonsterPartSObj(void);

	// Things all monster parts can do
	int getHealth() { return health; }
	void setFogging(bool fog) { isFogging = fog; }
	virtual PhysicsModel *getPhysicsModel() { return pm; }

	virtual int MonsterPartSObj::serialize(char * buf);

protected:
	int health;
	bool isFogging;
	Model modelNum;
	int modelAnimationState;
	bool attacked;
	PhysicsModel *pm;
	MonsterSObj* overlord;
};

