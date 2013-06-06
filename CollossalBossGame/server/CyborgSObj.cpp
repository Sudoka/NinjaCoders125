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
	chargeAttack = false;
	delay = 50;
	canCharge = false;
	delayCounter = 0;
}


void CyborgSObj::actionAttack() {
	// Does nothing atm
}

void CyborgSObj::actionCharge(bool buttondown) {
	if(!canCharge && charge == 0.f) delayCounter++;
	if((charge == 0.f) && ((delayCounter % delay) == 0)) canCharge = true;

	// only charge if you already started or you're out of charge
	if(canCharge && buttondown/*!this->getFlag(IS_FALLING)*/) {
		//start the charge sound on 

		// TODO_HARO: Verify that Franklin's changes fit all of the other changes you've made
		if(charge == chargeUpdate*2/*0.0f*/) { // Minor change to make sure that sounds don't play obnoxiously if you're charging ontop of a tentacle.
			sTrig = SOUND_CYBORG_CHARGE;
		}
		if(charge > chargeUpdate) { 
			this->subclassstate = PAS_CHARGE;
		}
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
			canCharge = false;
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

		// switch to idle once we're done attacking
		if (this->chargeAttack && this->attackCounter < 30) {
			this->attackCounter++;			
			this->subclassstate = PAS_ATTACK;
		}
		else // otherwise, decide according to movement
		{
			this->subclassstate = PAS_IDLE;
			this->chargeAttack = false;
		}
	}

	this->stopMovement = charging;
}

void CyborgSObj::onCollision(ServerObject *obj, const Vec3f &collisionNormal) {
	// Reset Damage
	ObjectType collidedWith = obj->getType();

	if (collidedWith == OBJ_TENTACLE || collidedWith == OBJ_HEAD) {
		if(charge > chargeUpdate) {
			//TODO_HARO: I'm not sure where to put this now. It plays every time on collision
			//I think it needs to be moved to where the sword animation is triggered after I 
			//merge.
			// Note from Franklin - If you set charge > chargeUpdate then this code only runs once.
			// unless we're changing the cyborg to maintain invincibility post-attack.
			sTrig = SOUND_CYBORG_SWORD; 
			this->chargeAttack = true;
			this->attackCounter = 0;
			this->subclassstate = PAS_ATTACK;
		}
		charge = 0;
	}

	PlayerSObj::onCollision(obj, collisionNormal);
}

