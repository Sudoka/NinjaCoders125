#pragma once

#include "defs.h"
#include "RenderModel.h"

class ClientObject
{
public:
	ClientObject(uint id);
	ClientObject(uint id, ObjectType type);
	virtual ~ClientObject(void);

	//To create a client object, you must implement these methods
	virtual bool update() = 0;
	virtual RenderModel* getRenderModel() = 0;
	//virtual AudioModel* getAudioModel() = 0;
	virtual void deserialize(char* newState) = 0;

	uint getId() { return id; }
	ObjectType getObjectType() { return type; }

private:
	uint id;
	ObjectType type;
};

