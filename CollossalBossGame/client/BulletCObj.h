#pragma once

#include "ClientObject.h"
#include "Action.h"
#include "ShootingEffect.h"

class BulletCObj : public ClientObject
{
public:
	BulletCObj(uint id, char *serializedData);
	virtual ~BulletCObj(void);

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);

private:
	RenderModel *rm;
	ShootingEffect* pewPew;
};

