#pragma once

#include "PlayerSObj.h"
#include "HarpoonSObj.h"

class ScientistSObj : public PlayerSObj
{
public:
	ScientistSObj(uint id, uint clientId);
	virtual ~ScientistSObj(void);

	virtual void initialize();
	virtual void clearAccessory();
	HarpoonSObj * hso;

protected:
	virtual void actionCharge(bool buttondown);
	virtual void actionAttack();
};

