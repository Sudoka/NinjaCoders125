#include "TentacleSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
#include "defs.h"
#include "PlayerSObj.h"
#include "BulletSObj.h"
#include "HarpoonSObj.h"
#include "ConfigurationManager.h"
#include "CollisionModel.h"
#include "PhysicsEngine.h"
#include <time.h>
#include <random>

TentacleSObj::TentacleSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new TentacleObj %d\n", id);
	overlord = master;
	overlord->addTentacle(this);
	//Box bxVol = CM::get()->find_config_as_box("BOX_MONSTER");
	this->modelNum = modelNum;
	this->health = CM::get()->find_config_as_int("INIT_HEALTH");

	pm = new PhysicsModel(pos, rot, 50*CM::get()->find_config_as_float("PLAYER_MASS"));

	/////////////// Collision Boxes /////////////////
	idleBoxes[0] = CM::get()->find_config_as_box("BOX_TENT_BASE"); 
	idleBoxes[1] = CM::get()->find_config_as_box("BOX_TENT_MID");
	idleBoxes[2] = CM::get()->find_config_as_box("BOX_TENT_TIP");

	CollisionModel *cm = getCollisionModel();

	for (int i=0; i<3; i++) {
		assert((cm->add(new AabbElement(idleBoxes[i])) == i) && "Your physics model is out of sync with the rest of the world...");
	}

	this->setFlag(IS_STATIC, 1);
	modelAnimationState = T_IDLE; // the boxes will be rotated appropriately within the idle part of update()
	
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1,50);
	attackCounter = distribution(generator);
	
	idleCounter = 0;

	// Configuration options
	attackBuffer = CM::get()->find_config_as_int("TENTACLE_ATTACK_BUF");
	attackFrames = CM::get()->find_config_as_int("TENTACLE_ATTACK_FRAMES");
	pushForce = CM::get()->find_config_as_int("TENTACLE_PUSH_FORCE");
}


TentacleSObj::~TentacleSObj(void)
{
	delete pm;
}

bool TentacleSObj::update() {
	attackCounter++;

	// start attacking!
	if (attackCounter > attackBuffer && !( (attackCounter - attackBuffer) % CYCLE)){
		if (this->getFlag(IS_HARMFUL))
		{
			this->setFlag(IS_HARMFUL, 0);
			attackBuffer = rand() % 40;
			attackFrames = - rand() % 15;
			modelAnimationState = T_IDLE;
			this->getPhysicsModel()->ref->setRot(lastRotation);
		} else {
			float angle = this->angleToNearestPlayer();
			if (angle != -1.f)
			{
				Vec3f axis = Vec3f(0,0,1);
				Vec4f qAngle = Vec4f(axis,angle);
				lastRotation = this->getPhysicsModel()->ref->getRot();
				this->getPhysicsModel()->ref->rotate(qAngle);
				this->setFlag(IS_HARMFUL, 1);
				modelAnimationState = T_SLAM;
			}
		}
	}
	
	/* Cycle logic:
	 * CYCLE*1/2 = The tentacle is extended
	 * CYCLE = when the tentacle is back at the default position
	 */
	CollisionModel *cm = getCollisionModel();
	Box base =	 ((AabbElement*)cm->get(0))->bx; //this->getPhysicsModel()->colBoxes.at(0);
	Box middle = ((AabbElement*)cm->get(1))->bx; //this->getPhysicsModel()->colBoxes.at(1);
	Box tip =	 ((AabbElement*)cm->get(2))->bx; //this->getPhysicsModel()->colBoxes.at(2);
	Vec3f changePosT = Vec3f(), changeProportionT = Vec3f();
	Vec3f changePosM = Vec3f(), changeProportionM = Vec3f();

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	if (modelAnimationState == T_IDLE)
	{
		// reset your state
		if (idleCounter == 0) {
			Box origBase = idleBoxes[0];
			Box origMiddle = idleBoxes[1];
			Box origTip = idleBoxes[2];

			base.setPos(axis.rotateToThisAxis(origBase.getPos()));
			base.setSize(axis.rotateToThisAxis(origBase.getSize()));

			middle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
			middle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));

			tip.setPos(axis.rotateToThisAxis(origTip.getPos()));
			tip.setSize(axis.rotateToThisAxis(origTip.getSize()));
		}
		else if(idleCounter < 15) {
			changeProportionM.y+=7;
			changePosM.y--;
			changePosT.y++;
		}
		else {
			changeProportionM.y-=7;
			changePosM.y++;
			changePosT.y--;
		}

		idleCounter = (idleCounter + 1) % 31;

	} else { // SLAM
		Vec3f pos;
		if ((attackCounter - attackBuffer)%CYCLE < CYCLE/2) {
			//changing the tip
			if ((attackCounter - attackBuffer)%CYCLE < CYCLE/4)
			{
				//decrease y
				changePosT.y -= 7;
				//increase height (h)
				changeProportionT.y += 3;
			} else {
				//increase y
				changePosT.y += 7;
				//decrease height (h)
				changeProportionT.y -= 3;
			}
			//decrease depth (l)
			changeProportionT.z  -= 1;
			//increase z
			changePosT.z += 6;

			changeProportionM.y += 2;
			changePosM.y -= 2;
			//tip.z = tip.z + 6;
			//middle.l = middle.l - 2;
		} else if ((attackCounter - attackBuffer)%CYCLE < CYCLE) {
			//changing the tip
			if ((attackCounter - attackBuffer)%CYCLE < 3*CYCLE/4)
			{
				//decrease y
				changePosT.y -= 7;
				//increase height (h)
				changeProportionT.y += 3;
			} else {
				//increase y
				changePosT.y += 7;
				//decrease height (h)
				changeProportionT.y -= 3;
			}
			//increase z
			changePosT.z -= 6;
			//increase depth (l, associated with z)
			changeProportionT.x += 1;
			
			changeProportionM.y -= 2;
			changePosM.y += 2;
			//tip.z = tip.z - 6;
			//middle.l = middle.l + 2;
		} 
	}
	
	// Rotate the relative change according to where we're facing
	changePosT = axis.rotateToThisAxis(changePosT);
	changeProportionT = axis.rotateToThisAxis(changeProportionT);
	changePosM = axis.rotateToThisAxis(changePosM);
	changeProportionM = axis.rotateToThisAxis(changeProportionM);
	
	tip.setRelPos(changePosT);
	tip.setRelSize(changeProportionT);

	middle.setRelPos(changePosM);
	middle.setRelSize(changeProportionM);
	
	// Set new collision boxes
	((AabbElement*)cm->get(0))->bx = *(base.fix());		//pm->colBoxes[0] = *(base.fix());
	((AabbElement*)cm->get(1))->bx = *(middle.fix());	//pm->colBoxes[1] = *(middle.fix());
	((AabbElement*)cm->get(2))->bx = *(tip.fix());		//pm->colBoxes[2] = *(tip.fix());

	if (health <= 0) {
		health = 0;
		overlord->removeTentacle(this);
		return true; // I died!
		//health = 0;
		// fancy animation 
		// just dont attack
		//attackBuffer = 0;
		//attackFrames = 0;
	}


	return false;
}

int TentacleSObj::serialize(char * buf) {
	TentacleState *state = (TentacleState*)buf;
	state->modelNum = this->modelNum;
	state->animationState = this->modelAnimationState;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(TentacleState));

		vector<CollisionElement*>::iterator cur = getCollisionModel()->getStart(),
			end = getCollisionModel()->getEnd();

		collState->totalBoxes = min(end - cur, maxBoxes);

		for(int i=0; i < collState->totalBoxes; i++, cur++) {
			//The collision box is relative to the object's frame-of-ref.  Get the non-relative collision box
			collState->boxes[i] = ((AabbElement*)(*cur))->bx + pm->ref->getPos();
		}
		return pm->ref->serialize(buf + sizeof(TentacleState) + sizeof(CollisionState)) + sizeof(TentacleState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(TentacleState)) + sizeof(TentacleState);
	}
}

/**
 * Checks if there's a player we can smash, if so
 * returns the angle we need to roll before we attack.
 * If no player is within range, we return -1.
 * Author: Haro
 */
float TentacleSObj::angleToNearestPlayer()
{
	float angle = -1.f;

	// Find player with minimum distance to me
	vector<ServerObject *> players;
	SOM::get()->findObjects(OBJ_PLAYER, &players);

	#define TENTACLE_LENGTH 300

	float minDist = TENTACLE_LENGTH;
	float currDist;
	Vec3f difference;

	for(vector<ServerObject *>::iterator it = players.begin(); it != players.end(); ++it) {
		difference = (*it)->getPhysicsModel()->ref->getPos() - this->getPhysicsModel()->ref->getPos();
		currDist = magnitude(difference);
		if (currDist < minDist) {
			minDist = currDist;
		}
	}

	if (minDist < TENTACLE_LENGTH)
	{
		// ignoring z... this is with respect to the y axis (since the tentacle smashes DOWN)
		angle = atan2(difference.x, -1*difference.y);
	}
	return angle;
}

void TentacleSObj::onCollision(ServerObject *obj, const Vec3f &collisionNormal) {
	// if the monster is attacking, it pushes everything off it on the last attack frame
	if (attackCounter == (attackBuffer + attackFrames))
	{
		Vec3f up = (PE::get()->getGravVec() * -1);
		obj->getPhysicsModel()->applyForce((up + collisionNormal)*(float)pushForce);
	}

	// if I collided against the player, AND they're attacking me, loose health
	if(obj->getType() == OBJ_PLAYER)
	{	
		PlayerSObj* player = reinterpret_cast<PlayerSObj*>(obj);
		health-= player->damage;
		if(this->health < 0) health = 0;
		if(this->health > 100) health = 100;
	}

	if(obj->getType() == OBJ_BULLET) {
		BulletSObj* bullet = reinterpret_cast<BulletSObj*>(obj);
		health -= bullet->damage;
		if(this->health < 0) health = 0;
		if(this->health > 100) health = 100;
	}

	if(obj->getType() == OBJ_HARPOON) {
		// HarpoonSObj* bullet = reinterpret_cast<HarpoonSObj*>(obj);
		// health -= bullet->damage;
		// if(this->health < 0) health = 0;
		// if(this->health > 100) health = 100;
	}
}

