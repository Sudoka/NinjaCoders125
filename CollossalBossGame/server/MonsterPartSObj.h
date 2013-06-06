#pragma once
#include "ServerObject.h"
#include "MonsterSObj.h"

// Different things tentacles can do
enum MonsterAction {
	IDLE_ACTION,
	ATTACK_ACTION, // SLAM or SHOOT
	SPIKE_ACTION,
	PROBE_ACTION,
	COMBO_ACTION,
	RAGE_ACTION,
	MOVE_ACTION,
	DEATH_ACTION,
	NUM_MONSTER_ACTIONS
};

/*
 * Ways our monster can be attacked
 * Smart Heads will counteract these =P
 */
enum PlayerAttack {
	CHARGE,
	SHOOT,
	STUN,
	NUM_PLAYER_ATTACKS
};

class MonsterPartSObj :
	public ServerObject
{
public:
	MonsterPartSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master);
	virtual ~MonsterPartSObj(void);

	// Things all monster parts do the same
	int getHealth() { return health; }
	void setHealth(int h) { health = h; } // this should only be called by the monster to kill them off xD
	void setFogging(bool fog) { isFogging = fog; }
	virtual PhysicsModel *getPhysicsModel() { return pm; }

	void setAnimationState(MonsterAnimationState state) { modelAnimationState = state; }
	void setAction(MonsterAction action) { actionState = action; }

	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);
	virtual int serialize(char * buf);
	virtual bool update();


	char serialbuffer[100];

	// Player targetting
	void findPlayer();

	//////////////////// ACTIONS /////////////////////

	// Should be implemented in the children
	virtual void idle() = 0;
	virtual void probe() = 0;
	virtual void attack() = 0;
	virtual void combo() = 0;
	virtual void spike() = 0;
	virtual void rage() = 0;
	virtual void move() = 0;
	virtual void death() = 0;


	//trigger the roar sound TODO_MICHAEL: remove me?
	virtual void roar() = 0;

	virtual ObjectType getType() = 0;

	virtual int getModelNumber() = 0;
	virtual void reset();
	virtual void reinitialize() = 0;
	bool takes_double_damage;
	bool frozen;

protected:
	int health;
	bool isFogging;
	Model modelNum;
	MonsterAnimationState modelAnimationState;
	MonsterAction actionState;
	bool attacked;
	PhysicsModel *pm;
	MonsterSObj* overlord;
	int stateCounter; // keeps track of our frames within each state
	int slamCounter;  // keeps track of our frames in an individual slam
	bool currStateDone; // whether or not our current state has gone through it's full cycle

	DIRECTION oldGravDir; // used to know when gravity switched so everyone moves

	// smart heads
	PlayerAttack lastAttack;
	int attackerId; // this should be set to -1 when the attacker has been dealt with

	// player targetting
	int targettingDist;
	bool playerFound;
	float playerAngle;
	Vec3f playerPos;

	// Collision Boxes in common
	Box idleBoxes[3]; // stores initial idle collision boxes

	MonsterSoundTrigger sTrig;
	MonsterSoundState sState;
	int roarProb;
};

