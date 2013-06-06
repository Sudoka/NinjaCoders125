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
	uint bulletdamage;
	uint maxbulletcount;
	bool shootAttack;
	virtual void actionCharge(bool buttondown);
	virtual void actionAttack();
};

