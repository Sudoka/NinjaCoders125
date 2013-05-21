#pragma once

#include "PlayerCObj.h"

class ShooterCObj :	public PlayerCObj
{
public:
	ShooterCObj(uint id, char *data);
	~ShooterCObj(void);
};

