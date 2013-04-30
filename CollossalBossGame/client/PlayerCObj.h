#pragma once
#include "RenderModel.h"
#include "ClientObject.h"

class PlayerCObj : public ClientObject
{
public:
	PlayerCObj(uint id, char *data);
	virtual ~PlayerCObj(void);

	void showStatus();

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);
	
	int ready;

private:
	int health;
	RenderModel *rm;
	float cameraPitch;
};

