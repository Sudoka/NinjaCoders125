#include "CyborgSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"

CyborgSObj::CyborgSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_CYBORG)
{
	// Other re-initializations (things that don't depend on parameters, like config)
	this->initialize();
}


CyborgSObj::~CyborgSObj(void)
{
}

void CyborgSObj::initialize() {
	// Configuration options
	PlayerSObj::initialize();
	

}

void CyborgSObj::releaseCharge() {
	float anglepi = (fabs(camPitch*1.5) > (M_PI/4.f)) ? camPitch : camPitch*1.5;
	float upforce = -sin(anglepi);
	float forwardforce = cos(anglepi);
	Vec3f force = rotate(Vec3f(0, upforce * chargeForce * charge, forwardforce * chargeForce * charge), pm->ref->getRot());
	pm->applyForce(force);
	charging = true;
}


void CyborgSObj::actionAttack() {
	float anglepi = (fabs(camPitch*1.5) > (M_PI/4.f)) ? camPitch : camPitch*1.5;
	float upforce = -sin(anglepi);
	float forwardforce = cos(anglepi);
	Vec3f force = rotate(Vec3f(0, upforce * chargeForce * charge, forwardforce * chargeForce * charge), pm->ref->getRot());
	pm->applyForce(force);
	charging = true;
}