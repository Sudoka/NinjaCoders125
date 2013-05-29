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
#include "RageSObj.h"
#include <time.h>
#include <random>

TentacleSObj::TentacleSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master) : 
													MonsterPartSObj(id, modelNum, pos, rot, master)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new TentacleSObj %d\n", id);

	// Tentacle config items
	this->targettingDist = CM::get()->find_config_as_int("SLAM_MIN_DIST");

	//Box bxVol = CM::get()->find_config_as_box("BOX_MONSTER");

	/////////////// Collision Boxes /////////////////
	idleBoxes[0] = CM::get()->find_config_as_box("BOX_TENT_BASE"); 
	idleBoxes[1] = CM::get()->find_config_as_box("BOX_TENT_MID");
	idleBoxes[2] = CM::get()->find_config_as_box("BOX_TENT_TIP");


	slamBoxes[0] = CM::get()->find_config_as_box("BOX_TENT_BASE_SLAM"); 
	slamBoxes[1] = CM::get()->find_config_as_box("BOX_TENT_MID_SLAM");
	slamBoxes[2] = CM::get()->find_config_as_box("BOX_TENT_TIP_SLAM");

	spikeBox = CM::get()->find_config_as_box("BOX_TENT_SPIKE");

	CollisionModel *cm = getCollisionModel();

	for (int i=0; i<3; i++) {
		assert((cm->add(new AabbElement(idleBoxes[i])) == i) && "Your physics model is out of sync with the rest of the world...");
	}

	// modelAnimationState = M_IDLE; // the boxes will be rotated appropriately within the idle part of update()
	
	//std::default_random_engine generator;
	//std::uniform_int_distribution<int> distribution(1,50);
	//attackCounter = distribution(generator);
	//idleCounter = 0;
	// Configuration options
	//attackBuffer = CM::get()->find_config_as_int("TENTACLE_ATTACK_BUF");
	//attackFrames = CM::get()->find_config_as_int("TENTACLE_ATTACK_FRAMES");
	//pushForce = CM::get()->find_config_as_int("TENTACLE_PUSH_FORCE");
}


TentacleSObj::~TentacleSObj(void)
{
}


void TentacleSObj::idle() {
	modelAnimationState = M_IDLE;

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

	// reset your state
	if (stateCounter == 0) {
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
	else if(stateCounter < 15) {
		changeProportionM.y+=7;
		changePosM.y--;
		changePosT.y++;
	}
	else {
		changeProportionM.y-=7;
		changePosM.y++;
		changePosT.y--;
	}

	// we're done!
	currStateDone = (stateCounter == 30);

	//idleCounter = (idleCounter + 1) % 31;
	
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
}

// TODO PROBE!!!
void TentacleSObj::probe() {
	idle();
}

/*
 * Attack means slam for the tentacle
 */
void TentacleSObj::attack() {
	// First, rotate ourselves to the player
	if (stateCounter == 0) {
		// If there was no player, rotate ourselves to a random angle
		if (!this->playerFound) this->playerAngle = rand()%(int)(M_PI*2);

		Vec3f rotationAxis = Vec3f(0,0,1);
		Vec4f qAngle = Vec4f(rotationAxis, playerAngle);
		lastRotation = this->getPhysicsModel()->ref->getRot();
		this->getPhysicsModel()->ref->rotate(qAngle);
	}

	slamMotion();

	// we're done!
	//if((attackCounter - attackBuffer)%CYCLE == CYCLE - 1)
	if(stateCounter == CYCLE - 1)
	{
		// reset our rotation
		this->getPhysicsModel()->ref->setRot(lastRotation);

		// establish that we're done
		currStateDone = true;
	}
}

#define NUM_SLAMS 16
#define SLAM_ANGLE 2*M_PI/NUM_SLAMS

/**
 * Slam Combo!
 */
void TentacleSObj::combo() {
	// First, save our initial rotation and reset our angle
	if (stateCounter == 0) {
		lastRotation = this->getPhysicsModel()->ref->getRot();
	}

	// Then, every new slam cycle, rotate our tentacle
	//if (stateCounter%CYCLE == 0) {
		Vec3f rotationAxis = Vec3f(0,0,1);
		//Vec4f qAngle = Vec4f(rotationAxis, SLAM_ANGLE);
		Vec4f qAngle = Vec4f(rotationAxis, SLAM_ANGLE/CYCLE);
		this->getPhysicsModel()->ref->rotate(qAngle);
	//}

	slamMotion();

	// We're done!
	if (stateCounter >= CYCLE*NUM_SLAMS)
	{
		// reset our rotation
		this->getPhysicsModel()->ref->setRot(lastRotation);

		// establish that we're done
		currStateDone = true;
	}
}

void TentacleSObj::slamMotion() {
	modelAnimationState = M_ATTACK;

	// Keep the base and middle, make the tip grow and move back
	CollisionModel *cm = getCollisionModel();
	Box base = ((AabbElement*)cm->get(0))->bx; // this->getPhysicsModel()->colBoxes.at(0);
	Box middle = ((AabbElement*)cm->get(1))->bx;// this->getPhysicsModel()->colBoxes.at(1);
	Box tip = ((AabbElement*)cm->get(2))->bx; // this->getPhysicsModel()->colBoxes.at(2);
	Vec3f changePosT = Vec3f();

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	if (stateCounter%CYCLE == 0) {
		Box origBase = slamBoxes[0];
		Box origMiddle = slamBoxes[1];
		Box origTip = slamBoxes[2];

		base.setPos(axis.rotateToThisAxis(origBase.getPos()));
		base.setSize(axis.rotateToThisAxis(origBase.getSize()));

		middle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
		middle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));

		tip.setPos(axis.rotateToThisAxis(origTip.getPos()));
		tip.setSize(axis.rotateToThisAxis(origTip.getSize()));
	}

	changePosT.z+=15;

	// Rotate the relative change according to where we're facing
	tip.setRelPos(axis.rotateToThisAxis(changePosT));
	
	// Set new collision boxes
	((AabbElement*)cm->get(0))->bx = *(base.fix());
	((AabbElement*)cm->get(0))->bx = *(middle.fix());
	((AabbElement*)cm->get(0))->bx = *(tip.fix());

//	/* Cycle logic:
//	 * CYCLE*1/2 = The tentacle is extended
//	 * CYCLE = when the tentacle is back at the default position
//	 */
//	Box base = this->getPhysicsModel()->colBoxes.at(0);
//	Box middle = this->getPhysicsModel()->colBoxes.at(1);
//	Box tip = this->getPhysicsModel()->colBoxes.at(2);
//	Vec3f changePosT = Vec3f(), changeProportionT = Vec3f();
//	Vec3f changePosM = Vec3f(), changeProportionM = Vec3f();
//
//	//get the actual axis
//	Vec4f axis = this->getPhysicsModel()->ref->getRot();
//
//	//if (((attackCounter - attackBuffer))%CYCLE == 0) {
//	if (stateCounter%CYCLE == 0) {
//		Box origBase = slamBoxes[0];
//		Box origMiddle = slamBoxes[1];
//		Box origTip = slamBoxes[2];
//
//		base.setPos(axis.rotateToThisAxis(origBase.getPos()));
//		base.setSize(axis.rotateToThisAxis(origBase.getSize()));
//
//		middle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
//		middle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));
//
//		tip.setPos(axis.rotateToThisAxis(origTip.getPos()));
//		tip.setSize(axis.rotateToThisAxis(origTip.getSize()));
//	}
//	/*
//		* What I want when I start slamming:
//		* BOX_TENM_BASE = -12, -20, 0, 28, 28, 75
//		* BOX_TENM_MID = -12, -50, -95, 28, 90, 90
//		* BOX_TENM_TIP = -12, 30, -165, 28, 30, 40
//		*
//		* When I'm in the middle of slamming:
//		* BOX_TENM_BASE = -12, -20, 28, 28, 28, 35
//		* BOX_TENM_MID = -12, -20, -28, 28, 70, 50
//		* BOX_TENM_TIP = -12, 8, 28, 28, 105, 35
//		*
//		* Base z: 0 -> 28 (-28, or -2 * 2 per 5 + a remainder)
//		* Base d: 75 -> 35 (-40, or -4 * 2 per 5)
//		*
//		* Mid y: -50 -> -20 (+30 or +6 per 5)
//		* Mid z: -95 -> -28 (+67 or +12 per 5 + a remainder)
//		* Mid h: 90 -> 70 (-20 or -4 per 5)
//		* Mid d: 90 -> 50 (-40 or -8 per 5)
//		*
//		* Tip y: 30 -> 8 (-22 or -4 per 5)
//		* Tip z: -165 -> 28 (+193 or +38 per 5)
//		* Tip h: 28 -> 105 (+77 or +14 per 5)
//		* Tip d: 40 -> 35 (-5 or -1 per 5)
//		* 
//		* Algorithm: 
//		*  1. at the beginning and end, move DIF % 10 units
//		*  2. per CYCLE / 10 iterations move everything DIF / 10 units.
//		* 
//		* With Cycle = 50, that means we need to get there in 25
//		* 
//		*/
//	Vec3f pos;
////	if ( ((attackCounter - attackBuffer))%5 == 0 )
//	if ( stateCounter%5 == 0 )
//	{
//		//if ((attackCounter - attackBuffer)%CYCLE < CYCLE/2) {
//		if (stateCounter%CYCLE < CYCLE/2) {
//			//Base z
//			//Base d
//
//			//Mid y
//			changePosM.y -= 5;
//			//Mid z
//			changePosM.z += 14;
//			//Mid d
//			//changeProportionM.z -= 20;
//				
//			//Tip y
//			changePosT.y += 4;
//			//Tip z
//			changePosT.z += 39;
//			//Tip h
//			changeProportionT.y -= 30;
//				
//		//} else if ((attackCounter - attackBuffer)%CYCLE < CYCLE) {
//		} else if (stateCounter%CYCLE < CYCLE) {
//			//Mid y
//			changePosM.y += 5;
//			//Mid z
//			changePosM.z -= 14;
//			//Mid d
//			//changeProportionM.z += 20;
//				
//			//Tip y
//			changePosT.y -= 4;
//			//Tip z
//			changePosT.z -= 39;
//			//Tip h
//			changeProportionT.y += 30;
//				
//		}
//	}
//	
//	// Rotate the relative change according to where we're facing
//	changePosT = axis.rotateToThisAxis(changePosT);
//	changeProportionT = axis.rotateToThisAxis(changeProportionT);
//	changePosM = axis.rotateToThisAxis(changePosM);
//	changeProportionM = axis.rotateToThisAxis(changeProportionM);
//	
//	tip.setRelPos(changePosT);
//	tip.setRelSize(changeProportionT);
//
//	middle.setRelPos(changePosM);
//	middle.setRelSize(changeProportionM);
//	
//	// Set new collision boxes
//	pm->colBoxes[0] = *(base.fix());
//	pm->colBoxes[1] = *(middle.fix());
//	pm->colBoxes[2] = *(tip.fix());
}

void TentacleSObj::spike() {
	modelAnimationState = M_SPIKE;

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	Box spike;

	spike.setPos(axis.rotateToThisAxis(spikeBox.getPos()));
	spike.setSize(axis.rotateToThisAxis(spikeBox.getSize()));

	// Spike is one big collision box
	// middle and tip are dimmension-less
	// Set new collision boxes
	CollisionModel *cm = getCollisionModel();	
	((AabbElement*)cm->get(0))->bx = *(spike.fix());
	((AabbElement*)cm->get(0))->bx = Box();
	((AabbElement*)cm->get(0))->bx = Box();

	// I'm randomly making spike last 51 cycles, feel free to change this xD
	currStateDone = (stateCounter == 50);
}

void TentacleSObj::rage() {
	modelAnimationState = M_RAGE;

	// First, we create the wave object
	if (stateCounter == 0) {
		Vec4f axis = this->getPhysicsModel()->ref->getRot();
		Vec3f changePos = Vec3f(0,0,-120);
		changePos = axis.rotateToThisAxis(changePos);
		SOM::get()->add(new RageSObj(SOM::get()->genId(), pm->ref->getPos() + changePos));
	}

	// for now, keep our initial idle collision boxes
	Box origBase = idleBoxes[0];
	Box origMiddle = idleBoxes[1];
	Box origTip = idleBoxes[2];

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	origBase.setPos(axis.rotateToThisAxis(origBase.getPos()));
	origBase.setSize(axis.rotateToThisAxis(origBase.getSize()));

	origMiddle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
	origMiddle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));

	origTip.setPos(axis.rotateToThisAxis(origTip.getPos()));
	origTip.setSize(axis.rotateToThisAxis(origTip.getSize()));

	CollisionModel *cm = getCollisionModel();
	((AabbElement*)cm->get(0))->bx = *(origBase.fix());
	((AabbElement*)cm->get(1))->bx = *(origMiddle.fix());
	((AabbElement*)cm->get(2))->bx = *(origTip.fix());

	// when the object dies we're done raging
	currStateDone = stateCounter >= RageSObj::lifetime;
}

