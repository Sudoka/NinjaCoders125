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
	int harpoon;

protected:
	virtual void actionCharge(bool buttondown);
	virtual void actionAttack();
	int delay, delaycounter;
	bool delaytrigger;

private:
	void fireHarpoon();
};

