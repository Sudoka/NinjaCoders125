#pragma once
#include "ClientObject.h"
#include "SmokeEffect.h"
#include "PushAwayEffect.h"

class TentacleCObj : public ClientObject
{
public:
	TentacleCObj(uint id, char *serializedData);
	virtual ~TentacleCObj(void);

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);
	virtual RenderModel * getBox();

private:
	RenderModel * box;
	RenderModel *rm;
	SmokeEffect* smoking;
	bool fogging;
	bool startedFogging;
};

