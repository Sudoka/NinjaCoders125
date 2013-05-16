#pragma once

#include "PlayerSObj.h"
#include "HarpoonSObj.h"

class MechanicSObj : public PlayerSObj
{
public:
	MechanicSObj(uint id, uint clientId);
	virtual ~MechanicSObj(void);

	virtual void initialize();
	virtual void clearAccessory();
	HarpoonSObj * hso;

protected:
	virtual void releaseCharge();
	virtual void actionAttack();
};

