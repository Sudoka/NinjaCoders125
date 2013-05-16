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
	PlayerSObj::initialize();
	hso = NULL;
}

void ShooterSObj::actionAttack() {
	Vec3f position;
	position.x = this->pm->ref->getPos().x;
	position.y = 10 + this->pm->ref->getPos().y;
	position.z = this->pm->ref->getPos().z;
	float anglepi = camPitch;
	float upforce = -sin(anglepi);
	float forwardforce = cos(anglepi);
	Vec3f force = rotate(Vec3f(0, upforce * 50, forwardforce * 50), pm->ref->getRot());
	int dmg = (charge < 1) ? 1 : charge;
	if(0) {
		BulletSObj * bso = new BulletSObj(SOM::get()->genId(), (Model)-1/*MDL_TEST_BOX*/, position, force, dmg, 10*(charge/3));
		SOM::get()->add(bso);
	} else { // Harpoon | Hook Shot
		 if(this->hso == NULL) {
			hso = new HarpoonSObj(SOM::get()->genId(), (Model)-1/*MDL_TEST_BOX*/, position, force, dmg, 10, this);
			// pso->getPhysicsModel()->accel = (PE::get()->getGravVec() * -1);
			SOM::get()->add(hso);
		} else {
			// Kill the Harpoon
			this->clearAccessory();
			// Possible bug - if harpoon has a lower id than player, odd things may happen
		}
	}
}

void ShooterSObj::releaseCharge() {
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
	hso->state = HS_DEAD;
	this->setFlag(IS_FLOATING, 0);
	// this->pm->accel = this->pm->vel*-1;
	// this->pm->vel = Vec3f();
	this->setFlag(IS_FALLING, 1);
	this->jumping = true;
	this->hso = NULL;
	
}