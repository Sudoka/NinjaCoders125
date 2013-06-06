#include "MechanicSObj.h"
#include "BulletSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"
#include "WorldSObj.h"
#include <math.h>

MechanicSObj::MechanicSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_MECHANIC)
{
	this->initialize();
}


MechanicSObj::~MechanicSObj(void)
{
}

void MechanicSObj::initialize() {
	if(harpoon != -1) {
		this->clearAccessory();
	}
	this->harpoon = -1;
	this->chargeUpdate = 13.0f/50.0f;
	this->charge = 0.0f;
	delay = 50;
	delaycounter = 0;
	delaytrigger = true;
	hookshotPlaying = false;
}

void MechanicSObj::fireHarpoon() {
	Vec3f gravity = dirVec(PE::get()->getGravDir())*-1;
	Vec3f mechanic_position = this->pm->ref->getPos();
	Vec3f force = rotate(Vec3f(0, -sin(camPitch) * 30, cos(camPitch) * 30), pm->ref->getRot());
	Vec3f initial_bullet_position = mechanic_position + gravity*15 + force; // Vec3f(mechanic_position.x, mechanic_position.y + 15, mechanic_position.z) + force;
	this->harpoon = SOM::get()->genId();
	HarpoonSObj * hso = new HarpoonSObj(this->harpoon, (Model)-1, initial_bullet_position, force, 10, this);
	SOM::get()->add(hso);
	sState = SOUND_MECHANIC_HARPOON_ON;
	hookshotPlaying = true;
}

void MechanicSObj::actionCharge(bool buttondown) {
	if(!delaytrigger) delaycounter++;
	if(!(delaycounter % delay)) delaytrigger = true;
	HarpoonSObj * hso = reinterpret_cast<HarpoonSObj *>(SOM::get()->find(this->harpoon));
	if(buttondown) {
		if(delaytrigger && !charging && (this->harpoon == -1 || hso == NULL)) {
			fireHarpoon();
		} else {
			if(hso == NULL) {
				charge = 0.0f;
			} else if(hso->state == HS_GRAPPLE || hso->state == HS_STUNGUN) {
				if(!charging) {
					charge = chargeCap;
					charging = true;
					this->delaytrigger = false;
				}
				charge -= chargeUpdate;
				if(charge < 0) {
					this->clearAccessory();
					charge = 0.0f;
					
				}
			} else if(hso->state == HS_HARPOON) {
				if(!charging) {
					charge = chargeCap;
					charging = true;
					this->delaytrigger = false;
				}
			}
		}
	} else {
		if(this->harpoon != -1 || hso != NULL) {
			this->clearAccessory();
			charge = 0.0f;
		}
		charging = false;
	}
}

void MechanicSObj::actionAttack() {


}

void MechanicSObj::clearAccessory() {
	ServerObject * so = SOM::get()->find(harpoon);
	if(so != NULL && so->getType() == OBJ_HARPOON) {
		HarpoonSObj * hso = reinterpret_cast<HarpoonSObj *>(so);
		if(hso->state == HS_HARPOON) {
			if((so = SOM::get()->find(hso->targetid)) != NULL) {
				so->setFlag(IS_FLOATING, 0);
				so->setFlag(IS_FALLING, 1);
			}
		}
		hso->state = HS_DEAD;
		this->harpoon = -1;
	}
	//stop the hookshot
	if(hookshotPlaying) {
		sState = SOUND_MECHANIC_HARPOON_OFF;
		hookshotPlaying = false;
	}

	this->setFlag(IS_FLOATING, 0);
	this->setFlag(IS_FALLING, 1);
	this->jumping = true;
}