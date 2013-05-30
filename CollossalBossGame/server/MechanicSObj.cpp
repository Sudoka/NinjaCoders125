#include "MechanicSObj.h"
#include "BulletSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"
#include <math.h>

MechanicSObj::MechanicSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_MECHANIC)
{
	this->initialize();
}


MechanicSObj::~MechanicSObj(void)
{
}

void MechanicSObj::initialize() {
	this->harpoon = -1;
	this->chargeUpdate = 13.0f/50.0f;
	this->charge = 0.0f;
	delay = 50;
	delaycounter = 0;
	delaytrigger = true;
}

void MechanicSObj::actionCharge(bool buttondown) {
	if(!delaytrigger) delaycounter++;
	if(!(delaycounter % delay)) delaytrigger = true;
	ServerObject * so = SOM::get()->find(this->harpoon);
	if(buttondown) {
		if(delaytrigger && !charging && (this->harpoon == -1 || so == NULL)) {
			Vec3f mechpos = this->pm->ref->getPos();
			Vec3f force;// = rotate(Vec3f(0, -sin(camPitch) * 30, cos(camPitch) * 30), pm->ref->getRot());
			if(this->targetlockon != -1) {
				ServerObject * nil = SOM::get()->find(this->targetlockon);
				if(nil != NULL) {
					Vec3f targetlocation = nil->getPhysicsModel()->ref->getPos();
					force = targetlocation - mechpos;
					force.normalize();
					force *= 30;
				}	
			} else {
				force = rotate(Vec3f(0, -sin(camPitch) * 30, cos(camPitch) * 30), pm->ref->getRot());
			}
			Vec3f position = Vec3f(mechpos.x, mechpos.y + 15, mechpos.z) + force;
			this->harpoon = SOM::get()->genId();
			HarpoonSObj * hso = new HarpoonSObj(this->harpoon, (Model)-1, position, force, 10, this);
			SOM::get()->add(hso);
		} else {
			if(so == NULL) {
				charge = 0.0f;
			} else if(((HarpoonSObj *)so)->state == HS_GRAPPLE) {
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
			} else if(((HarpoonSObj *)so)->state == HS_HARPOON) {
				if(!charging) {
					charge = chargeCap;
					charging = true;
					this->delaytrigger = false;
				}
			}
		}
	} else {
		if(this->harpoon != -1 || so != NULL) {
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
	this->setFlag(IS_FLOATING, 0);
	this->setFlag(IS_FALLING, 1);
	this->jumping = true;
}