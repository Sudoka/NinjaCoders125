#pragma once

#include "PlayerSObj.h"

class ScientistSObj : public PlayerSObj
{
public:
	ScientistSObj(uint id, uint clientId);
	virtual ~ScientistSObj(void);

	virtual void initialize();
	virtual void clearAccessory();
	virtual void onCollision(ServerObject *obj, const Vec3f &collNorm);
	virtual int serialize(char * buf);
	int harpoon;

protected:
	virtual void actionCharge(bool buttondown);
	virtual void CyborgActionCharge(bool buttondown);
	virtual void ShooterActionCharge(bool buttondown);
	virtual void MechanicActionCharge(bool buttondown);
	virtual void ScientistActionCharge(bool buttondown);

	virtual void clearScientistAccessory();
	virtual void clearMechanicAccessory();
	virtual void clearCyborgAccessory() { }
	virtual void clearShooterAccessory() { }

	virtual void ScientistOnCollision(ServerObject *obj, const Vec3f &collNorm) { }
	virtual void CyborgOnCollision(ServerObject *obj, const Vec3f &collNorm);
	virtual void MechanicOnCollision(ServerObject *obj, const Vec3f &collNorm) { }
	virtual void ShooterOnCollision(ServerObject *obj, const Vec3f &collNorm) { }

	virtual void actionAttack();

	// Mechanic Variables
	int delay, delaycounter;
	bool delaytrigger;

	CharacterClass transformclass;
	int currenttarget;
	int transformdelay;
	int transformduration;
};

