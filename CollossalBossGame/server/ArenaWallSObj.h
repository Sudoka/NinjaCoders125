#pragma once
#include "ServerObject.h"

/*
 * This class is used for heightmap-based arena objects
 */
class ArenaWallSObj : public ServerObject {
public:
	ArenaWallSObj(uint id, const char* filename, Model modelNum, Point_t pos, DIRECTION dir);
	virtual ~ArenaWallSObj(void);

	virtual bool update();							//Perform logic update
	virtual int serialize(char * buf);				//Write all pertinent information to the specified buffer

	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual ObjectType getType() { return OBJ_GENERAL; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal) {}
private:
	PhysicsModel *pm;
	Model modelNum;
};

