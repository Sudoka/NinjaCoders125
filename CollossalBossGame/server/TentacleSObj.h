#pragma once
#include "ServerObject.h"
#include "MonsterSObj.h"

// fwd decl
class MonsterSObj;

// note this is only for slam now
//#define CYCLE 50 //always even because of integer division
#define CYCLE 18 //always even because of integer division

// Different things tentacles can do
enum TentacleAction {
	IDLE_ACTION,
	SLAM_ACTION,
	SPIKE_ACTION,
	PROBE_ACTION,
	SHOOT_ACTION,
	COMBO_ACTION,
	RAGE_ACTION,
	NUM_TENTACLE_ACTIONS
};

class TentacleSObj : public ServerObject
{
public:
	TentacleSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master);
	virtual ~TentacleSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_TENTACLE; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);
	void setAnimationState(TentacleActionState state) { modelAnimationState = state; }
	void setAction(TentacleAction action) { actionState = action; }
	void setFogging(bool fog) { isFogging = fog; }

	// Actions
	void idle();
	void slam();
	void slamCombo();
	void spike();
	void rage();

	int getHealth() { return health; }

	float angleToNearestPlayer();

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	Model modelNum;
	MonsterSObj* overlord;
	Box updatableBox;
	int health;
	bool attacked;
	int stateCounter; // keeps track of our frames within each state
	int attackBuffer; // how many frames pass before we're harmful again
	int attackFrames; // how many continuous frames we are harmful
	bool sweepingZPositive; // are we sweeping in the direction which makes z positive 
	int dir;
	bool currStateDone; // whether or not our current state has gone through it's full cycle
	TentacleActionState modelAnimationState;
	TentacleAction actionState;
	int pushForce; // force of tentacle when it pushes player away after attacking it
	Quat_t lastRotation;
	float playerAngle;
	bool isFogging;

	// Collision boxes
	Box idleBoxes[3]; // stores initial idle collision boxes
	Box slamBoxes[3]; // stores first position for the slam boxes
	Box spikeBox; // stores spike box =D

	// Helper actions
	void slamMotion(); // doesn't do the begin/end rotation
};

