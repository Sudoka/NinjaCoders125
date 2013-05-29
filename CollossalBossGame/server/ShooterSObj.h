#pragma once

#include "PlayerSObj.h"
#include "HarpoonSObj.h"

class ShooterSObj : public PlayerSObj
{
public:
	ShooterSObj(uint id, uint clientId);
	virtual ~ShooterSObj(void);

	virtual void initialize();
	virtual void clearAccessory();

protected:
	virtual void actionCharge(bool buttondown);
	virtual void actionAttack();
};

