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
	bulletdamage = CM::get()->find_config_as_int("BULLET_DAMAGE");
	maxbulletcount = CM::get()->find_config_as_int("SHOOTER_MAX_BULLET_COUNT");
}

void ShooterSObj::actionAttack() {
	// Does nothing at the moment.
}

void ShooterSObj::actionCharge(bool buttondown) {
	if(buttondown && BulletSObj::TotalShooterBullets < maxbulletcount) {
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
			Vec3f gravity = dirVec(PE::get()->getGravDir())*-1;
			Vec3f position = mechpos + gravity*15 + offset;

			BulletSObj * bso = new BulletSObj(SOM::get()->genId(), (Model)-1, position, offset, bulletdamage, (int)diameter, this);
			sTrig = SOUND_SHOOTER_FIRE;
			SOM::get()->add(bso);

			charging = false;
			charge = 0;
		}
	}
}

void ShooterSObj::clearAccessory() {
	// Nothing Happens... There's no accessory.
}