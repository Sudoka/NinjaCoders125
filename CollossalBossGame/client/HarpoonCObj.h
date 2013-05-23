#pragma once

#include "ClientObject.h"
#include "Action.h"
#include "HarpoonEffect.h"

class HarpoonCObj : public ClientObject
{
public:
	HarpoonCObj(uint id, char *serializedData);
	virtual ~HarpoonCObj(void);

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);

	int creatorid;

private:
	RenderModel *rm;
	HarpoonEffect* pewPew;
	int diameter;
};

