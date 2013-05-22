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
	charge = 0.0f;
	chargeUpdate = 13.0f/50.0f;
}

void CyborgSObj::actionAttack() {
	// Does nothing atm
}

void CyborgSObj::actionCharge(bool buttondown) {
	if(buttondown && !this->getFlag(IS_FALLING)) {
		charging = true;
		charge += chargeUpdate;
		if(charge > 13.0f) charge = 13.0f;
	} else {
		// reset charge
		if(charging) {
			float anglepi = (fabs(camPitch*1.5f) > (M_PI/4.f)) ? camPitch : camPitch*1.5f;
			float upforce = -sin(anglepi);
			float forwardforce = cos(anglepi);
			Vec3f force = rotate(Vec3f(0, upforce * chargeForce * charge, forwardforce * chargeForce * charge), pm->ref->getRot());
			pm->applyForce(force);
		}
		charge = 0.0f;
		charging = false;
	}
}


