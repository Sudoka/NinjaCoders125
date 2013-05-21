#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
#include "GameServer.h"
#include "WallSObj.h"
#include "defs.h"
#include "PhysicsEngine.h"

PlayerSObj::PlayerSObj(uint id, uint clientId) : ServerObject(id) {
	// Save parameters here
	this->clientId = clientId;
	DC::get()->print("Player %d with obj id %d created\n", clientId, id);

	// Set all your pointers to NULL here, so initialize()
	// knows if it should create them or not
	pm = NULL;

	// Other re-initializations (things that don't depend on parameters, like config)
	this->initialize();
}


void PlayerSObj::initialize() {
	// Configuration options
	jumpDist = CM::get()->find_config_as_float("JUMP_DIST");
	movDamp = CM::get()->find_config_as_int("MOV_DAMP");
	chargeForce = CM::get()->find_config_as_float("CHARGE_FORCE");
	swordDamage = CM::get()->find_config_as_int("SWORD_DAMAGE");
	chargeDamage = CM::get()->find_config_as_int("CHARGE_DAMAGE");
	chargeUpdate = CM::get()->find_config_as_float("CHARGE_UPDATE");
	this->health = CM::get()->find_config_as_int("INIT_HEALTH");


	if(SOM::get()->debugFlag) DC::get()->print("Initialized new PlayerSObj %d\n", this->getId());

	Point_t pos = Point_t(0, 5, 10);
	Box bxVol = CM::get()->find_config_as_box("BOX_CUBE");//Box(-10, 0, -10, 20, 20, 20);

	if(pm != NULL)
		delete pm;

	pm = new PhysicsModel(pos, Quat_t(), CM::get()->find_config_as_float("PLAYER_MASS"));
	pm->addBox(bxVol);
	lastCollision = pos;

	// Initialize input status
	istat.attack = false;
	istat.jump = false;
	istat.quit = false;
	istat.start = false;
	istat.specialPower = false;
	istat.rotAngle = 0.0;
	istat.rotHoriz = 0.0;
	istat.rotVert = 0.0;
	istat.rightDist = 0.0;
	istat.forwardDist = 0.0;
	istat.camLock = false;
	camLocked = true;

	// Avoids button holding to keep applying ability
	newJump = true; // any jump at this point is a new jump
	newAttack = true; // same here
	newCharge = true; // yes yes, we get the idea

	appliedJumpForce = false;
	firedeath = false;
	attacking = false;
	gravityTimer = 0;
	charging = false;
	charge = 0.0;
	damage = 0;
	modelAnimationState = IDLE;
	ready = false;

	lastGravDir = DOWN;
	t = 1;
	tRate = CM::get()->find_config_as_float("GRAVITY_SWITCH_CAMERA_SPEED");
	yaw = 0;
	initUpRot = Quat_t();
	finalUpRot = Quat_t();
	camYaw = 0;
	camKpSlow = CM::get()->find_config_as_float("CAM_KP_SLOW");
	camKpFast = CM::get()->find_config_as_float("CAM_KP_FAST");
	camKp = camKpSlow;
}

PlayerSObj::~PlayerSObj(void) {
	delete pm;
}


bool PlayerSObj::update() {

	if (istat.start && !ready) {
		ready = true;
	}

	if (istat.quit) {
		return true; // delete me!
	}
	
	
	Quat_t upRot;
	calcUpVector(&upRot);
	controlCamera(upRot);
	sState = SOUND_SILENT;
	sTrig = SOUND_NO_NEW_TRIG;

	if(this->health > 0)
	{
		firedeath = false;

		attacking = istat.attack && newAttack;
		newAttack = !istat.attack;

		//if (attacking) attackCounter++;
		//else attackCounter = 0;

		// Jumping can happen in two cases
		// 1. Collisions
		// 2. In the air
		// This handles in the air, collisions
		// are handled in OnCollision()

		// This part discretizes jumps (so no button mashing)
		jumping = istat.jump && newJump; // isFalling?
		newJump = !istat.jump;

		// This part gives us a buffer, so the user can bounce off the wall even 
		// when they pressed 'jump' before they got there
		if (jumping) jumpCounter++;
		else jumpCounter = 0; 

		//this is HACKY! HELP ME!!!!!!
		//if(jumpCounter == 1)

		appliedJumpForce = false; // we apply it on collision

		/*if (istat.specialPower && newCharge && !getFlag(IS_FALLING)) {
			// CHARGE!!!
			// todo for now up, should be forward (or up + forward?)
			Vec3f up = (PE::get()->getGravVec() * -1);
			pm->applyForce(up * chargeForce);
			charging = true;
		}
		newCharge = !istat.specialPower;*/

		damage = charging ? chargeDamage : attacking ? swordDamage : 0;
		

		//Update the yaw rotation of the player (about the default up vector)
		if(fabs(istat.forwardDist) > 0.0f || fabs(istat.rightDist) > 0.0f) {
			yaw = camYaw + istat.rotAngle;
		}

		Quat_t qRot = upRot * Quat_t(Vec3f(0,1,0), yaw);
		pm->ref->setRot(qRot);

		//Move the player: apply a force in the appropriate direction
		float rawRight = istat.rightDist / movDamp;
		float rawForward = istat.forwardDist / movDamp;
		float fwdMag = sqrt(rawRight *rawRight + rawForward * rawForward);
		//Vec3f total = rotate(Vec3f(rawRight, 0, rawForward), qRot);
		Vec3f total = rotate(Vec3f(0, 0, fwdMag), qRot);
		
		pm->applyForce(total);

		// Apply special power
		if (istat.specialPower && !getFlag(IS_FALLING)) // holding down increases the charge
		{
			charge+=chargeUpdate;
			if(charge > 13) charge = 13.f;
		}
		else
		{
			// If we accumulated some charge, fire!
			if (charge > 0.f)
			{
				// Vec3f up = (PE::get()->getGravVec() * -1);
				//pm->applyForce(up * (chargeForce * charge));
				pm->applyForce(rotate(Vec3f(0, chargeForce * charge, chargeForce * charge), qRot));
				charging = true;
			}

			charge = 0.f;
		}

		
		// change animation according to state
		if(pm->vel.x <= 0.25 && pm->vel.x >= -0.25 && pm->vel.z <= 0.25 && pm->vel.z >= -0.25) {
			this->setAnimationState(IDLE);
		} else {
			this->setAnimationState(WALK);
		}
	} else {
		damage = 0; // you can't kill things if you're dead xD

		// TODO Franklin: THE PLAYER IS DEAD. WHAT DO?
		// NOTE: Player should probably be also getting their client id.
		if(!firedeath) {
			firedeath = true;
			GameServer::get()->event_player_death(this->getId());
		}
	}

	return false;
}

void PlayerSObj::calcUpVector(Quat_t *upRot) {
	//Update up direction
	PE *pe = PE::get();
	if(lastGravDir != pe->getGravDir()) {
		lastGravDir = pe->getGravDir();
		//pm->ref->rotate(pe->getCurGravRot());
		slerp(&initUpRot, initUpRot, finalUpRot, t);	//We may not have finished rotating
		finalUpRot = pe->getCurGravRot();
		t = 0;
	}

	//Rotate by amount specified by player (does not affect up direction)
	slerp(upRot, initUpRot, finalUpRot, t);
	if(t < 1.0f) {
		t += tRate;
	}
}

void PlayerSObj::controlCamera(const Quat_t &upRot) {
		//If the camera lock button is pressed, then we interpolate much faster
		if(istat.camLock) {
			camKp = camKpFast;
		} else {
			camKp = camKpSlow;
		}

		//Update the camera-lock state: Locked to or unlocked from the player
		if(istat.camLock) {
			camLocked = true;
		} else if(camLocked && fabs(istat.rotHoriz) > 0) {
			camLocked = false;
		}

		//If the camera is locked, calculate the yaw as a function of kp,
		// player yaw, and camera yaw.  Otherwise, control with right joystick
		if(camLocked) {
			camYaw = controlAngles(yaw, camYaw);
		} else {
			camYaw += istat.rotHoriz;
		}

		//Correct the camera angle so it is between +/-pi
		if(camYaw < -M_PI) camYaw += M_TAU;
		else if(camYaw > M_PI) camYaw -= M_TAU;
		camRot = upRot * Quat_t(Vec3f(0,1,0), camYaw);
}

float PlayerSObj::controlAngles(float des, float cur) {
	//Determine
	float err1 = des - cur, err2, errDiff;
	if(des < 0) err2 = (des + M_TAU) - cur;
	else err2 = (des - M_TAU) - cur;

	errDiff = fabs(fabs(err1) - fabs(err2));
	//DC::get()->print("Error differences: %f-%f = %f\n", err1, err2, );

	if(fabs(err1) < fabs(err2)) {
		return cur + err1 * errDiff * camKp;
	} else {
		return cur + err2 * errDiff * camKp;
	}
}

int PlayerSObj::serialize(char * buf) {
	PlayerState *state = (PlayerState*)buf;
	// This helps us distinguish between what model goes to what player
	state->modelNum = (Model)(MDL_PLAYER_1 + this->clientId);
	state->health = health;
	state->ready = ready;
	state->charge = charge;
	if (SOM::get()->debugFlag) DC::get()->print("CURRENT MODEL STATE %d\n",this->modelAnimationState);
	state->animationstate = this->modelAnimationState;
	state->sState = this->sState;
	state->sTrig = this->sTrig;
	state->camRot = this->camRot;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(PlayerState));

		vector<Box> objBoxes = pm->colBoxes;

		collState->totalBoxes = min(objBoxes.size(), maxBoxes);

		for (int i=0; i<collState->totalBoxes; i++)
		{
			collState->boxes[i] = objBoxes[i] + pm->ref->getPos(); // copying applyPhysics
		}

		return pm->ref->serialize(buf + sizeof(PlayerState) + sizeof(CollisionState)) + sizeof(PlayerState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(PlayerState)) + sizeof(PlayerState);
	}
}

void PlayerSObj::deserialize(char* newInput)
{
	inputstatus* newStatus = reinterpret_cast<inputstatus*>(newInput);
	istat = *newStatus;
	if (istat.start) {
		GameServer::get()->event_reset(this->getId());
	}
}

void PlayerSObj::onCollision(ServerObject *obj, const Vec3f &collNorm) {
	if(obj->getFlag(IS_HARMFUL) && !(attacking))
		this->health-=3;
	if(obj->getFlag(IS_HEALTHY))
		this->health++;
	if(this->health < 0) health = 0;
	if(this->health > 100) health = 100;

	
	// If I started jumping a little bit ago, that's a jump
	// appliedJumpForce is because OnCollision gets called twice
	// on every collision, so this makes sure you only apply the
	// jump force once every iteration
	if(!appliedJumpForce && (jumpCounter > 0 && jumpCounter < 10))
	{
		// surface bouncing
		// Get the collNorm from the surface
		float bounceDamp = 0.05f;

		Vec3f incident = pm->ref->getPos() - lastCollision;

		//play jump sound
		sTrig = SOUND_PLAYER_JUMP;

		// incident is zero, so we just jump upwards
		// this happens when you jump of the same surface
		// you were at before (so the floor, or when you
		// slide off the wall and then jump)
		if ((incident.x < .01 && incident.x > -.01)
			|| (incident.y < .01 && incident.y > -.01)
			|| (incident.z < .01 && incident.z > -.01))
		{
			Vec3f force = (PE::get()->getGravVec() * -1) + collNorm;
			pm->vel = Vec3f();
			pm->applyForce(force*jumpDist);
		}
		// we have incident! so we bounce
		else
		{
			// http://www.3dkingdoms.com/weekly/weekly.php?a=2
			// optimize: *= ^= better!
			pm->vel = (collNorm * (((incident ^ collNorm) * -2.f )) + incident) * bounceDamp;
		}

		appliedJumpForce = true;
	}

	// Set last collision pos for bouncing off different surfaces
	lastCollision = pm->ref->getPos();
	jumping = false;
	charging = false;
}
