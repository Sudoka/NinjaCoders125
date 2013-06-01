#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
#include "GameServer.h"
#include "WallSObj.h"
#include "defs.h"
#include "PhysicsEngine.h"


#define DEFAULT_PITCH_10 0.174532925f	//10 degrees or stg like that

PlayerSObj::PlayerSObj(uint id, uint clientId, CharacterClass cc) : ServerObject(id) {
	// Save parameters here
	this->clientId = clientId;
	DC::get()->print("Player %d with obj id %d created\n", clientId, id);

	// Set all your pointers to NULL here, so initialize()
	// knows if it should create them or not
	pm = NULL;
	charclass = cc;

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
	bxStaticVol = CM::get()->find_config_as_box("BOX_PLAYER");//Box(-10, 0, -10, 20, 20, 20);

	if(pm != NULL) {
		delete pm;
		getCollisionModel()->clean();
	}

	pm = new PhysicsModel(pos, Quat_t(), CM::get()->find_config_as_float("PLAYER_MASS"));
	getCollisionModel()->add(new AabbElement(bxStaticVol));

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
	chargeCap = 13.0f;
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
	camPitch = DEFAULT_PITCH_10;
	camKpSlow = CM::get()->find_config_as_float("CAM_KP_SLOW");
	camKpFast = CM::get()->find_config_as_float("CAM_KP_FAST");
	camKp = camKpSlow;
	camDistMin = CM::get()->find_config_as_float("CAM_FIRST_PERSON_DIST");
	camDistMax = camDist = CM::get()->find_config_as_float("CAM_DIST");

	scientistBuffCounter = 0;
	scientistBuffDecreasing = false;
	zoomed = false;
	setFlag(IS_DIRTY, true);
}

PlayerSObj::~PlayerSObj(void) {
	delete pm;
}

bool PlayerSObj::update() {

	if (istat.start && !ready) {
		ready = true;
	}

	if (istat.start && istat.quit) {
		return true; // delete me!
	}
	Point_t myPos = pm->ref->getPos();
	CollisionModel *cm = getCollisionModel();
	DC::get()->print(LOGFILE | TIMESTAMP, "Player pos: (%f,%f,%f), collSize = %d\n", myPos.x, myPos.y, myPos.z, cm->getEnd() - cm->getStart());

	
	Quat_t upRot;
	calcUpVector(&upRot);
	controlCamera(upRot);
	sState = SOUND_PLAYER_SLIENT;
	sTrig = SOUND_PLAYER_NO_NEW_TRIG;

	if(this->health > 0 && !GameServer::get()->state.gameover)
	{
		firedeath = false;

		attacking = istat.specialPower && newAttack;
		newAttack = !istat.specialPower;

		if (attacking) {
			actionAttack();
		}

		if(istat.specialPower) {
			if(this->targetlockon == -1) this->acquireTarget();
		} else {
			this->targetlockon = -1;
		}


		zoomed = istat.zoom;	//this logic may or may not change
		if(zoomed) {
			camDist = camDistMin;
		} else {
			camDist = camDistMax;
		}

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

		// damage = charging ? chargeDamage : 0;

		//Update the yaw rotation of the player (about the default up vector)
		if(fabs(istat.forwardDist) > 0.0f || fabs(istat.rightDist) > 0.0f) {
			yaw = camYaw + istat.rotAngle;
		} else if(zoomed) {
			yaw = camYaw;
		}

		if(istat.camLock) {
			camPitch = DEFAULT_PITCH_10;
		} else {
			camPitch += istat.rotVert;
		}
		if (camPitch > M_PI / 2.f) {
			camPitch = (float)M_PI / 2.f;
		} else if(camPitch < -M_PI / 4) {
			camPitch = (float)-M_PI / 4.f;
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
		actionCharge(istat.attack);

		// change animation according to state
		if(pm->vel.x <= 0.25 && pm->vel.x >= -0.25 && pm->vel.z <= 0.25 && pm->vel.z >= -0.25) {
			this->setAnimationState(IDLE);
		} else {
			this->setAnimationState(WALK);
		}
	} else if (!GameServer::get()->state.gameover) {
		Quat_t qRot = upRot * Quat_t(Vec3f(0,1,0), yaw);
		pm->ref->setRot(qRot);

		damage = 0; // you can't kill things if you're dead xD

		if(!firedeath) {
			firedeath = true;
			GameServer::get()->event_player_death(this->clientId);
			deathtimer = 0;
		}
		
		deathtimer++;
		if(deathtimer == 50) {
			firedeath = false;
			this->PlayerSObj::initialize();
			this->initialize();
			this->ready = true;
		}
	}

	if (this->scientistBuffDecreasing)
	{
		this->attacking = true;
		this->scientistBuffCounter -= 1;
		if (this->scientistBuffCounter == 0) {
			this->scientistBuffDecreasing = false;
			this->attacking = false;
		}
	}
	
	setFlag(IS_DIRTY, true);	//This is probably a safe assumption

	return false;
}

void PlayerSObj::calcUpVector(Quat_t *upRot) {
	//Update up direction
	PE *pe = PE::get();
	if(lastGravDir != pe->getGravDir()) {
		lastGravDir = pe->getGravDir();

		//Calculate the new initial and final up vectors
		slerp(&initUpRot, initUpRot, finalUpRot, t);	//We may not have finished rotating
		finalUpRot = pe->getCurGravRot();
		t = 0;
		
		//Update the collision box
		AabbElement *e = dynamic_cast<AabbElement*>(getCollisionModel()->get(0));
		if(e != NULL) {
			e->bx = bxStaticVol;
			e->bx.rotate(finalUpRot);
		} else {
			DC::get()->print(__FILE__" %d: ERROR- Player collision AABB 0 not found!\n", __LINE__);
		}
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
		if(istat.camLock && !zoomed) {
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
		if(camYaw < -M_PI) camYaw += (float)M_TAU;
		else if(camYaw > M_PI) camYaw -= (float)M_TAU;
		camRot = upRot * Quat_t(Vec3f(0,1,0), camYaw);
}

float PlayerSObj::controlAngles(float des, float cur) {
	//Determine the camera angle cost
	float err1 = des - cur, err2, errDiff;
	if(des < 0) err2 = (des + (float)M_TAU) - cur;
	else err2 = (des - (float)M_TAU) - cur;

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
	switch(this->charclass) {
		case CHAR_CLASS_CYBORG:
			state->modelNum = (Model)(MDL_PLAYER_1);
			break;
		case CHAR_CLASS_SHOOTER:
			state->modelNum = (Model)(MDL_PLAYER_2);
			break;
		case CHAR_CLASS_SCIENTIST:
			state->modelNum = (Model)(MDL_PLAYER_3);
			break;
		case CHAR_CLASS_MECHANIC:
			state->modelNum = (Model)(MDL_PLAYER_4);
			break;
	}
	state->health = health;
	state->ready = ready;
	state->charge = charge;
	if (SOM::get()->debugFlag) DC::get()->print("CURRENT MODEL STATE %d\n",this->modelAnimationState);
	state->animationstate = this->modelAnimationState;
	state->sState = this->sState;
	state->sTrig = this->sTrig;
	state->camRot = this->camRot;
	state->camPitch = this->camPitch;
	state->camDist = this->camDist;

	//'or together any boolean states
	state->bStates = PLAYER_NONE;
	if(zoomed) {
		state->bStates |= PLAYER_ZOOM;
	}

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(PlayerState));

		vector<CollisionElement*>::iterator cur = getCollisionModel()->getStart(),
			end = getCollisionModel()->getEnd();

		collState->totalBoxes = min(end - cur, maxBoxes);

		for(int i=0; i < collState->totalBoxes; i++, cur++) {
			//The collision box is relative to the object's frame-of-ref.  Get the non-relative collision box
			collState->boxes[i] = ((AabbElement*)(*cur))->bx + pm->ref->getPos();
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

	if(istat.start) {
		this->health = CM::get()->find_config_as_int("INIT_HEALTH");
		this->pm->ref->setPos(Point_t());
	}
}

void PlayerSObj::onCollision(ServerObject *obj, const Vec3f &collNorm) {
	if(obj->getType() == OBJ_BULLET || obj->getType() == OBJ_FIREBALL) {
		this->health-=3;
		if(this->health < 0) health = 0;
		if(this->health > 100) health = 100;
	}
	if(obj->getType() == OBJ_HARPOON) {
		return;
	}
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
		//Clear player's velocity along the gravity axis
		pm->vel -= pm->vel * dirAxis(PE::get()->getGravDir());

		//Apply jump force
		Vec3f jumpVec = collNorm - PE::get()->getGravVec();
		jumpVec.normalize();
		pm->applyForce(jumpVec * jumpDist);

		//play jump sound
		sTrig = SOUND_PLAYER_JUMP;
		appliedJumpForce = true;
	}

	// Set last collision pos for bouncing off different surfaces
	lastCollision = pm->ref->getPos();
	jumping = false;
	// charging = false; // Charging handled by subclasses.
}

void PlayerSObj::acquireTarget() {
	Vec3f currpos = this->pm->ref->getPos();
	Vec3f currdir = rotate(Vec3f(0, -sin(camPitch), cos(camPitch)), pm->ref->getRot());
	currdir.normalize();
	vector<ServerObject *> allobjs;
	float closestdirection = 0.0f;
	int closestobjid = -1;
	for(int i = 0; i < NUM_OBJS; i++) {
		if((ObjectType)i == OBJ_WORLD || (ObjectType)i == OBJ_MONSTER) continue;
		SOM::get()->findObjects((ObjectType)i, &allobjs);
	}
	for(unsigned int i = 0; i < allobjs.size(); i++) {
		Vec3f temppos = allobjs[i]->getPhysicsModel()->ref->getPos();
		Vec3f tempdir = temppos - currpos;
		tempdir.normalize();
		float dotprod = tempdir ^ currdir;
		if(dotprod > 0 && dotprod > closestdirection) {
			closestdirection = dotprod;
			closestobjid = allobjs[i]->getId();
		}
	}
	if(closestobjid != -1) {
		targetlockon = closestobjid;
	} else {
		assert(false && "I'm pretty sure it doesn't hit here, but just in case.");
	}
}
