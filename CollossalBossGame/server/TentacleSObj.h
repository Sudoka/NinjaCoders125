#pragma once
#include "ServerObject.h"
#include "MonsterSObj.h"

// fwd decl
class MonsterSObj;


#define CYCLE 30

class TentacleSObj : public ServerObject
{
public:
	TentacleSObj(uint id, Model modelNum, Point_t pos, Rot_t rot, MonsterSObj* master);
	virtual ~TentacleSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_TENTACLE; }
	virtual void initialize();						//Initial position/rotation/etc of the object
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);
	void setAnimationState(TentacleActionState state) { modelAnimationState = state; }

	int getHealth() { return health; }

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	Model modelNum;
	MonsterSObj* overlord;
	int updatableBoxIndex;
	Box updatableBox;
	int health;
	int attackCounter; // number of frames in between when the monster is harmful (emulates an 'attack')
	int attackBuffer; // how many frames pass before we're harmful again
	int attackFrames; // how many continuous frames we are harmful

	TentacleActionState modelAnimationState;
};

