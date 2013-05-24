#pragma once

#include "PlayerCObj.h"
#include "HarpoonCObj.h"

class ShooterCObj :	public PlayerCObj
{
public:
	ShooterCObj(uint id, char *data);
	~ShooterCObj(void);
};

