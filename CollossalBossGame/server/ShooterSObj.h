#pragma once

#include "PlayerSObj.h"

class ShooterSObj : public PlayerSObj
{
public:
	ShooterSObj(uint id, uint clientId);
	virtual ~ShooterSObj(void);

	virtual void initialize();

protected:
	virtual void releaseCharge();
	virtual void actionAttack();
};

