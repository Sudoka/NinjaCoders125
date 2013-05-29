#pragma once
#include "ClientObject.h"
#include "PushAwayEffect.h"
#include "RenderEngine.h"

class RageCObj :
	public ClientObject
{
public:
	RageCObj(uint id, char *serializedData);
	virtual ~RageCObj(void);
	
	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);
	
	virtual RenderModel * getBox() { return NULL; } // todo ask what this is o_o

private:
	RenderModel *rm;
	PushAwayEffect* push;
};

