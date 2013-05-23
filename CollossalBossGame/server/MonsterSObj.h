#pragma once
#include "ServerObject.h"
//#include "TentacleSObj.h"
#include <random>
//#include <list>
//#include <map>
#include <vector>

/* MonsterSObj.h
 * This defines our tentacle & it's strategy
 *
 * Author: Bryan
 */
// fwd decl
//class TentacleSObj;
class MonsterPartSObj;

class MonsterSObj : public ServerObject
{
public:
	MonsterSObj(uint id, uint numParts);
	virtual ~MonsterSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return NULL; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_MONSTER; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);

	void addPart(MonsterPartSObj* t) { parts.insert(t); }
	void removePart(MonsterPartSObj* t);

	Frame updatePosition(Frame oldPos);

	char serialbuffer[100];

private:
	PhysicsModel *pm;
	int health;
	int phase; // what phase of the monster you're in

	set<MonsterPartSObj*> parts;
	vector<Frame> availablePlacements;
	//vector<Point_t> placements;
	uint numParts;
	//TentacleState state;

	//The logic is that we keep track of how long until we switch to another state.
	//int stateCounter;

	//Determining how many cycles we wait until we activate a sweep. 
	//assumes CYCLE cycles per second
	//std::default_random_engine generator;
	//std::uniform_int_distribution<int> distribution;
};

