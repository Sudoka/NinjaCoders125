#pragma once
#include "MonsterPartSObj.h"
#include "MonsterSObj.h"

// fwd decl
class MonsterSObj;

// note this is only for slam now
//#define CYCLE 50 //always even because of integer division
#define CYCLE 18 //always even because of integer division

class TentacleSObj : public MonsterPartSObj
{
public:
	TentacleSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master);
	virtual ~TentacleSObj(void);

	// Actions
	virtual void idle();
	virtual void probe();
	virtual void attack();
	virtual void combo();
	virtual void spike();
	virtual void rage();

	virtual void fastForward();

private:
	//Box updatableBox;
	//int attackBuffer; // how many frames pass before we're harmful again
	//int attackFrames; // how many continuous frames we are harmful
	//bool sweepingZPositive; // are we sweeping in the direction which makes z positive 
	//int dir;
	//int pushForce; // force of tentacle when it pushes player away after attacking it
	Quat_t lastRotation;

	// Collision boxes
	Box slamBoxes[3]; // stores first position for the slam boxes
	Box spikeBox; // stores spike box =D

	// Helper actions
	void slamMotion(); // doesn't do the begin/end rotation
};

