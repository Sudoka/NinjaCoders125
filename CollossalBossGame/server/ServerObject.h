#pragma once
#include "defs.h"
#include "PhysicsModel.h"
#include <assert.h>
#include "NetworkData.h"
#include "CollisionModel.h"

class ServerObject
{
public:
	ServerObject(uint id);
	virtual ~ServerObject(void);

	//To create a server object, you must implement these methods
	virtual bool update() = 0;							//Perform logic update
	virtual PhysicsModel *getPhysicsModel() = 0;		//Get this object's physics model
	virtual int serialize(char * buf) = 0;				//Write all pertinent information to the specified buffer
	virtual ObjectType getType() = 0;					//Get object type for client-side creation
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal) = 0;	//Perform any logic ops on collision

	// Can be overriden, but should be only used by objects that receive
	// input from the client (a.k.a. playerobjects)
	virtual void deserialize(char* newState) {
		// TODO: add to error console
		// DC::get()->print("ERROR! Trying to deserialize input for a server object that doesn't take input from the client\n");
		assert(false && "ERROR! Trying to deserialize input for a server object that doesn't take input from the client\n");
	}

	// Should be overriden for objects that need to save some special state
	// and you can't just delete and re-create on restart (ex. PlayerObjects)
	virtual void initialize() { }

	inline uint getId() { return id; }
	inline uint getFlag(uint flag) { return GET_FLAG(flags, flag); }
	inline void setFlag(uint flag, uint val) { flags = SET_FLAG(flags, flag, val); }
	inline CollisionModel *getCollisionModel() { return cm; }	//Get this object's collision model

private:
	uint id;
	uint flags;
	CollisionModel *cm;
};

