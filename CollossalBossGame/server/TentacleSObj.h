#pragma once
#include "ServerObject.h"
#include "MonsterSObj.h"

// fwd decl
class MonsterSObj;

#define CYCLE 48 //always even because of integer division

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

	int getHealth() { return health; }

	float angleToNearestPlayer();

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	Model modelNum;
	MonsterSObj* overlord;
	Box updatableBox;
	int health;
	int idleCounter; // keeps track of what frame we are in the idle animation
	int attackCounter; // number of frames in between when the monster is harmful (emulates an 'attack')
	int attackBuffer; // how many frames pass before we're harmful again
	int attackFrames; // how many continuous frames we are harmful
	bool sweepingZPositive; // are we sweeping in the direction which makes z positive 
	int dir;
	TentacleActionState modelAnimationState;
	int pushForce; // force of tentacle when it pushes player away after attacking it
	Quat_t lastRotation;
	Box idleBoxes[3]; // stores initial idle collision boxes
};

