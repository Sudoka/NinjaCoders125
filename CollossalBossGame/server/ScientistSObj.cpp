#include "ScientistSObj.h"
#include "BulletSObj.h"
#include "HarpoonSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"

ScientistSObj::ScientistSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_SCIENTIST)
{
	// Other re-initializations (things that don't depend on parameters, like config)
	this->initialize();
}


ScientistSObj::~ScientistSObj(void)
{
}

void ScientistSObj::initialize() {
	charge = 0.0f;
	chargeUpdate = 13.0f/50.0f;
	if(harpoon != -1) {
		this->clearAccessory();
	}
	this->harpoon = -1;
	this->currenttarget = -1;
	this->chargeUpdate = 13.0f/50.0f;
	this->charge = 0.0f;
	delay = 50;
	delaycounter = 0;
	delaytrigger = true;
	this->transformclass = CHAR_CLASS_SCIENTIST;
}

void ScientistSObj::actionAttack() {

}

void ScientistSObj::actionCharge(bool buttondown) {
	if(transformclass != CHAR_CLASS_SCIENTIST) { // You're not the scientist right now
		transformduration--;
		if(transformduration < 0) {
			transformclass = CHAR_CLASS_SCIENTIST;
			this->currenttarget = -1;
			clearAccessory();
			damage = 0;
		}
	}
	switch(transformclass) {
		case CHAR_CLASS_SCIENTIST: this->ScientistActionCharge(buttondown); break;
		case CHAR_CLASS_CYBORG: this->CyborgActionCharge(buttondown); break;
		case CHAR_CLASS_SHOOTER: this->ShooterActionCharge(buttondown); break;
		case CHAR_CLASS_MECHANIC: this->MechanicActionCharge(buttondown); break;
		default: assert(false && "New class not handled here - please implement?");
	}
}
void ScientistSObj::clearAccessory() {
	clearMechanicAccessory();
}

void ScientistSObj::MechanicActionCharge(bool buttondown) {
	if(!delaytrigger) delaycounter++;
	if(!(delaycounter % delay)) delaytrigger = true;
	ServerObject * so = SOM::get()->find(this->harpoon);
	if(buttondown) {
		if(delaytrigger && !charging && (this->harpoon == -1 || so == NULL)) {
			Vec3f mechpos = this->pm->ref->getPos();
			Vec3f force = rotate(Vec3f(0, -sin(camPitch) * 30, cos(camPitch) * 30), pm->ref->getRot());
			if(this->targetlockon != -1) {
				ServerObject * nil = SOM::get()->find(this->targetlockon);
				if(nil != NULL) {
					Vec3f targetlocation = nil->getPhysicsModel()->ref->getPos();
					force = targetlocation - mechpos;
					force.normalize();
					force *= 30;
				}	
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

void ScientistSObj::CyborgActionCharge(bool buttondown) {
	if(buttondown && !this->getFlag(IS_FALLING)) {
		charging = true;
		charge += chargeUpdate;
		if(charge > 13.0f) charge = 13.0f;
	} else {
		// reset charge
		if(charging && charge > 0.0f) {
			float anglepi = (fabs(camPitch*1.5f) > (M_PI/4.f)) ? camPitch : camPitch*1.5f;
			float upforce = -sin(anglepi);
			float forwardforce = cos(anglepi);
			Vec3f force = rotate(Vec3f(0, upforce * chargeForce * charge, forwardforce * chargeForce * charge), pm->ref->getRot());
			pm->applyForce(force);
			damage = this->chargeDamage;
		}
		charge = 0.0f;
		charging = false;
	}
}

void ScientistSObj::ShooterActionCharge(bool buttondown) {
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

			// todo clean up or config or something
			const int bulletDamage = 3;
			BulletSObj * bso = new BulletSObj(SOM::get()->genId(), (Model)-1, position, offset, bulletDamage, (int)diameter, this);
			SOM::get()->add(bso);

			charging = false;
			charge = 0;
		}
	}
}

int selectplayertarget(PlayerSObj * caller, Vec3f currentposition, Vec3f currentdirection) {
	currentdirection.normalize();

	vector<ServerObject *> playerobjects;
	SOM::get()->findObjects(OBJ_PLAYER, &playerobjects);

	float closestdirection = 0.8f;
	int currenttarget = -1;

	for(unsigned int i = 0; i < playerobjects.size(); i++) {
		if(playerobjects[i]->getId() == caller->getId()) { continue; }

		Vec3f temppos = playerobjects[i]->getPhysicsModel()->ref->getPos();
		Vec3f tempdir = temppos - currentposition;
		tempdir.normalize();
		float dotprod = tempdir ^ currentdirection;
		if(dotprod > 0 && dotprod > closestdirection) {
			closestdirection = dotprod;
			currenttarget = playerobjects[i]->getId();
		}
	}

	return currenttarget;
}

void ScientistSObj::ScientistActionCharge(bool buttondown) {
	if(buttondown) {
		currenttarget = selectplayertarget(this, this->pm->ref->getPos(), rotate(Vec3f(0, -sin(camPitch), cos(camPitch)), pm->ref->getRot()));
	} else if(currenttarget != -1) {
		transformduration = 330;
		ServerObject * s = SOM::get()->find(currenttarget);
		if(s != NULL && s->getType() == OBJ_PLAYER) {
			PlayerSObj * p = (PlayerSObj *)s;
			transformclass = p->getCharacterClass();
		} else {
			assert(false && "What. - ScientistClass Transform");
		}
	} else {
		currenttarget = -1;
		transformduration = 0;
		charge = 0;
	}
}

void ScientistSObj::clearMechanicAccessory() {
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

void ScientistSObj::clearScientistAccessory() { }

int ScientistSObj::serialize(char * buf) {
	*(int *)buf = this->transformdelay; buf += 4;
	*(int *)buf = this->transformduration; buf += 4;
	*(int *)buf = this->currenttarget; buf += 4;
	*(int *)buf = (int)this->transformclass; buf += 4;
	return PlayerSObj::serialize(buf) + 16;
}

void ScientistSObj::onCollision(ServerObject *obj, const Vec3f &collNorm) {
	if(this->transformclass == CHAR_CLASS_CYBORG) {
		this->CyborgOnCollision(obj, collNorm);
	} else {
		PlayerSObj::onCollision(obj, collNorm);
	}
}

void ScientistSObj::CyborgOnCollision(ServerObject *obj, const Vec3f &collNorm) {
	// Reset Damage
	damage = 0;

	PlayerSObj::onCollision(obj, collNorm);
}