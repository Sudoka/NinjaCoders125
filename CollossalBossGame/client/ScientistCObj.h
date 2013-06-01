#pragma once

#include "PlayerCObj.h"
#include "HarpoonCObj.h"
#include "BeamEffect.h"

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
	CharacterClass transformedclass;
	BeamEffect* be;
};
