#include "ScientistSObj.h"
#include "BulletSObj.h"
#include "HarpoonSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"

//TODO_HARO: Scientist clone sounds

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
	delay = 50;
	cyborgcanCharge = false;
	cyborgdelayCounter = 0;

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

	bulletdamage = CM::get()->find_config_as_int("BULLET_DAMAGE");
	maxbulletcount = CM::get()->find_config_as_int("SCIENTIST_MAX_BULLET_COUNT");

	this->transformclass = CHAR_CLASS_SCIENTIST;
}

void ScientistSObj::actionAttack() {

}

void ScientistSObj::actionCharge(bool buttondown) {
	if(transformclass != CHAR_CLASS_SCIENTIST) { // You're not the scientist right now
		transformduration--;
		if(transformduration < 0) {
			//TODO_MICHAEL: scientist sound stop
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

void ScientistSObj::fireHarpoon() {
	Vec3f gravity = dirVec(PE::get()->getGravDir())*-1;
	Vec3f mechanic_position = this->pm->ref->getPos();
	Vec3f force = rotate(Vec3f(0, -sin(camPitch) * 30, cos(camPitch) * 30), pm->ref->getRot());
	Vec3f initial_bullet_position = mechanic_position + gravity*15 + force; // Vec3f(mechanic_position.x, mechanic_position.y + 15, mechanic_position.z) + force;
	this->harpoon = SOM::get()->genId();
	HarpoonSObj * hso = new HarpoonSObj(this->harpoon, (Model)-1, initial_bullet_position, force, 10, this);
	SOM::get()->add(hso);
}

void ScientistSObj::MechanicActionCharge(bool buttondown) {
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

void ScientistSObj::CyborgActionCharge(bool buttondown) {
	if(!cyborgcanCharge && charge == 0.f) cyborgdelayCounter++;
	if((charge == 0.f) && ((cyborgdelayCounter % cyborgdelay) == 0)) cyborgcanCharge = true;

	// only charge if you already started or you're out of charge
	if(cyborgcanCharge && buttondown/*!this->getFlag(IS_FALLING)*/) {
		charging = true;
		charge += chargeUpdate;
		if(charge > 13.0f) charge = 13.0f;
	} else {
		// reset charge
		if(/*charging && */charge > 0.0f) {
			/*float anglepi = (fabs(camPitch*1.5f) > (M_PI/4.f)) ? camPitch : camPitch*1.5f;
			float upforce = -sin(anglepi);
			float forwardforce = cos(anglepi);
			Vec3f force = rotate(Vec3f(0, upforce * chargeForce * charge, forwardforce * chargeForce * charge), pm->ref->getRot());
			pm->applyForce(force);*/
			cyborgcanCharge = false;
			damage = this->chargeDamage * this->charge;
			this->setFlag(IS_INVINCIBLE, 1);
		}
		else
		{
			damage = 0;
			this->setFlag(IS_INVINCIBLE, 0);
		}

		//charge = 0.0f;
		charge -= chargeUpdate;
		if(charge < 0.f) charge = 0.f;
		charging = false;
	}
}

void ScientistSObj::ShooterActionCharge(bool buttondown) {
	if(buttondown && BulletSObj::TotalScientistBullets < maxbulletcount) {
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
		//TODO_MICHAEL: start scientist loop
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
	ObjectType collidedWith = obj->getType();
	if (collidedWith == OBJ_TENTACLE || collidedWith == OBJ_HEAD) charge = 0;

	PlayerSObj::onCollision(obj, collNorm);
}