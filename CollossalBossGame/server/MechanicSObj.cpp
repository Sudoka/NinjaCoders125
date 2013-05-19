#include "MechanicSObj.h"
#include "BulletSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"

MechanicSObj::MechanicSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_SHOOTER)
{
	// Other re-initializations (things that don't depend on parameters, like config)
	this->initialize();
}


MechanicSObj::~MechanicSObj(void)
{
}

void MechanicSObj::initialize() {
	// Configuration options
	PlayerSObj::initialize();
	this->harpoon = -1;
}

void MechanicSObj::actionAttack() {
	ServerObject * so = NULL;
	if(this->harpoon == -1 || (so = SOM::get()->find(this->harpoon)) == NULL) {
		Vec3f mechpos = this->pm->ref->getPos();
		Vec3f position = Vec3f(mechpos.x, mechpos.y + 10, mechpos.z);
		Vec3f force = rotate(Vec3f(0, -sin(camPitch) * 50, cos(camPitch) * 50), pm->ref->getRot());
		int dmg = (charge < 1) ? 1 : charge;
		this->harpoon = SOM::get()->genId();
		HarpoonSObj * hso = new HarpoonSObj(this->harpoon, (Model)-1, position, force, dmg, 10, this);
		SOM::get()->add(hso);
	} else {
		// Kill the Harpoon
		this->clearAccessory();
		// Possible bug - if harpoon has a lower id than player, odd things may happen
	}
}

void MechanicSObj::releaseCharge() {
	Vec3f position;
	Vec3f offset = rotateFwd(pm->ref->getRot())*15;
	position.x = this->pm->ref->getPos().x + offset.x;
	position.y = 10 + this->pm->ref->getPos().y + offset.y;
	position.z = this->pm->ref->getPos().z + offset.z;
	float anglepi = camPitch;
	float upforce = -sin(anglepi);
	float forwardforce = cos(anglepi);
	float bulletforce = 10; //chargeForce * charge;
	// TODO: force should be fetched from config file
	Vec3f force = rotate(Vec3f(0, upforce * bulletforce, forwardforce * bulletforce), pm->ref->getRot());
	BulletSObj * bso = new BulletSObj(SOM::get()->genId(), (Model)-1/*MDL_TEST_BOX*/, position, force, 1, 10*(charge/3));
	SOM::get()->add(bso);
}

void MechanicSObj::clearAccessory() {
	ServerObject * so = SOM::get()->find(harpoon);
	if(so != NULL && so->getType() == OBJ_HARPOON) {
		HarpoonSObj * hso = reinterpret_cast<HarpoonSObj *>(so);
		if(hso->state == HS_HARPOON) {
			if((so = SOM::get()->find(hso->targetid)) != NULL) {
				so->setFlag(IS_FLOATING, 0);
				Vec3f pos = so->getPhysicsModel()->ref->getPos();
				pos.y += 10;
				so->getPhysicsModel()->ref->setPos(pos);
				so->setFlag(IS_FALLING, 1);
			}
		}
		hso->state = HS_DEAD;
		this->harpoon = -1;
	}
	this->setFlag(IS_FLOATING, 0);
	this->setFlag(IS_FALLING, 1);
	this->jumping = true;
}