#pragma once

#include "PlayerCObj.h"
#include "HarpoonCObj.h"
#include "BeamEffect.h"
#include "RenderEngine.h"

class ScientistCObj : public PlayerCObj
{
public:
	ScientistCObj(uint id, char *data);
	~ScientistCObj(void);
	virtual int getTypeInt();
	virtual void deserialize(char * newState);
	virtual bool update();

private:
	int transformdelay;
	int transformduration;
	int transformtargetid;
	CharacterClass transformedclassprevious;
	CharacterClass transformedclass;
	BeamEffect* be;

	RenderModel * rm1;
	RenderModel * rm2;
	RenderModel * rm3;
	RenderModel * rm4;
};
