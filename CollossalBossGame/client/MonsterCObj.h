#pragma once
#include "ClientObject.h"

class MonsterCObj : public ClientObject
{
public:
	MonsterCObj(uint id, char *serializedData);
	virtual ~MonsterCObj(void);

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);

private:
	int health;
	RenderModel *rm;
};

