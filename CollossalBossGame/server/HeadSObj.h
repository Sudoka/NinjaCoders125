#pragma once
#include "ServerObject.h"
#include "MonsterSObj.h"

// Different things heads can do
enum HeadAction {
	IDLE_ACTION_H,
	SPIKE_ACTION_H,
	PROBE_ACTION_H,
	SHOOT_ACTION_H,
	RAGE_ACTION_H,
	MOVE_ACTION_H,
	DEATH_ACTION_H,
	NUM_HEAD_ACTIONS
};

class HeadSObj : ServerObject
{
public:
	HeadSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master);
	virtual ~HeadSObj(void);

	//virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	//virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_TENTACLE; }
	//virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);
	void setAnimationState(HeadAnimationState state) { modelAnimationState = state; }
	void setAction(HeadAction action) { actionState = action; }
	void setFogging(bool fog) { isFogging = fog; }

	// Actions
	void idle();
	void slam();
	void slamCombo();
	void spike();
	void rage();
	void move();
	void death();

	int getHealth() { return health; }

	float angleToNearestPlayer();

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	Model modelNum;
	MonsterSObj* overlord;
	int health;
	HeadAnimationState modelAnimationState;
	HeadAction actionState;
	bool isFogging;
};

