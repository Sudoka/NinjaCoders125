#pragma once

#include "PlayerSObj.h"

class CyborgSObj : public PlayerSObj
{
public:
	CyborgSObj(uint id, uint clientId);
	virtual ~CyborgSObj(void);

	virtual void initialize();
protected:
	virtual void actionCharge(bool buttondown);
	virtual void actionAttack();
};