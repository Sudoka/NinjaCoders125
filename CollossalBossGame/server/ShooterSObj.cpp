#include "ShooterSObj.h"
#include "BulletSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"

ShooterSObj::ShooterSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_SHOOTER)
{
	// Other re-initializations (things that don't depend on parameters, like config)
	this->initialize();
}


ShooterSObj::~ShooterSObj(void)
{
}

void ShooterSObj::initialize() {
	// Configuration options
}

void ShooterSObj::actionAttack() {
	// Does nothing at the moment.
}

void ShooterSObj::actionCharge(bool buttondown) {
	if(buttondown && BulletSObj::TotalBullets < 5) {
		charging = true;
		charge += chargeUpdate;
	} else {
		if(charging) {
			Vec3f mechpos = this->pm->ref->getPos();
			float anglepi = camPitch;
			float upforce = -sin(anglepi);
			float forwardforce = cos(anglepi);
			float diameter = 10*(charge/3);
			Vec3f offset = rotate(Vec3f(0, upforce * diameter * sqrt(2.0f), forwardforce * diameter * sqrt(2.0f)), pm->ref->getRot());
			Vec3f position = Vec3f(mechpos.x, mechpos.y + 15, mechpos.z) + offset;

			BulletSObj * bso = new BulletSObj(SOM::get()->genId(), (Model)-1, position, offset, damage, (int)diameter);
			SOM::get()->add(bso);

			charging = false;
			charge = 0;
		}
	}
}

void ShooterSObj::clearAccessory() {
	// Nothing Happens... There's no accessory.
}