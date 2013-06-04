#pragma once
#include "MonsterPartSObj.h"
#include "MonsterSObj.h"

class HeadSObj : public MonsterPartSObj
{
public:
	HeadSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master);
	virtual ~HeadSObj(void);

	// Actions
	virtual void idle();
	virtual void probe();
	virtual void attack();
	virtual void combo();
	virtual void spike();
	virtual void rage();
	virtual void move();
	virtual void death();

	virtual ObjectType getType() { return OBJ_HEAD; }

private:
	int fireballForce;
	int fireballDamage;
	int fireballDiameter;
	int headBoxSize;

	// Helper Actions
	void shootFireball();

	// Collision boxes
	Box shootBoxes[3]; // stores first position for the shoot boxes
};

