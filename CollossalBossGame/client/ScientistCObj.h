#pragma once

#include "PlayerCObj.h"
#include "HarpoonCObj.h"

class ScientistCObj : public PlayerCObj
{
public:
	ScientistCObj(uint id, char *data);
	~ScientistCObj(void);
};

