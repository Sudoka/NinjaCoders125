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
	hso = NULL;
}

void ShooterSObj::actionAttack() {
	// Does nothing at the moment.
}

void ShooterSObj::actionCharge(bool buttondown) {
	Vec3f position;
	position.x = this->pm->ref->getPos().x;
	position.y = 10 + this->pm->ref->getPos().y;
	position.z = this->pm->ref->getPos().z;
	float anglepi = camPitch;
	float upforce = -sin(anglepi);
	float forwardforce = cos(anglepi);
	float bulletforce = 50; //chargeForce * charge;
	// TODO: force should be fetched from config file
	Vec3f force = rotate(Vec3f(0, upforce * bulletforce, forwardforce * bulletforce), pm->ref->getRot());
	BulletSObj * bso = new BulletSObj(SOM::get()->genId(), (Model)-1/*MDL_TEST_BOX*/, position, force, 1, 10*(charge/3));
	SOM::get()->add(bso);
}

void ShooterSObj::clearAccessory() {

}