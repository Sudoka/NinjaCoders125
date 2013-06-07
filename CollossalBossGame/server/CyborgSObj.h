#pragma once

#include "PlayerSObj.h"

class CyborgSObj : public PlayerSObj
{
public:
	CyborgSObj(uint id, uint clientId);
	virtual ~CyborgSObj(void);

	virtual void initialize();
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);

protected:
	virtual void actionCharge(bool buttondown);
	virtual void actionAttack();
	int delay, delayCounter;
	bool canCharge;
	bool chargeAttack;
};