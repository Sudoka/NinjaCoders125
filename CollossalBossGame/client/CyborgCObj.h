#pragma once

#include "PlayerCObj.h"

class CyborgCObj : public PlayerCObj
{
public:
	CyborgCObj(uint id, char *data);
	~CyborgCObj(void);
	virtual int getTypeInt();
	virtual bool update();
	ChargeEffect* chargingEffect;
};

