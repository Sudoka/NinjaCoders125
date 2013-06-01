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

	moveBoxes[0] = CM::get()->find_config_as_box("BOX_TENT_BASE_SLAM"); 
	moveBoxes[1] = CM::get()->find_config_as_box("BOX_TENT_MID_SLAM");
	moveBoxes[2] = CM::get()->find_config_as_box("BOX_TENT_TIP_SLAM");

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
	else if(stateCounter < 16*2) {
		changePosT.y -= 2;
		changePosT.z += 3;
		changeProportionT.z -= 3;
	}
	else if (stateCounter < 47) {
		changePosT.y += 2;
		changePosT.z -= 3;
		changeProportionT.z += 3;
	}
	else {
		changePosT.y += 2;
		changePosT.z += 3;
		changeProportionT.z -= 3;
	}
	
	// we're done!
	currStateDone = (stateCounter == 61);

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
	CollisionModel *cm = getCollisionModel();
	Box base =	 ((AabbElement*)cm->get(0))->bx; //this->getPhysicsModel()->colBoxes.at(0);
	Box middle = ((AabbElement*)cm->get(1))->bx; //this->getPhysicsModel()->colBoxes.at(1);
	Box tip =	 ((AabbElement*)cm->get(2))->bx; //this->getPhysicsModel()->colBoxes.at(2);
	Vec4f axis = this->getPhysicsModel()->ref->getRot();
		
	Vec3f changePosT = Vec3f(), changeProportionT = Vec3f();
	Vec3f changePosM = Vec3f(), changeProportionM = Vec3f();

	if (stateCounter == 0)
	{
		// Keep initial idle boxes
		Box origBase = idleBoxes[0];
		Box origMiddle = idleBoxes[1];
		Box origTip = idleBoxes[2];

		//get the actual axis
		origBase.rotate(axis);
		origMiddle.rotate(axis);
		origTip.rotate(axis);

		CollisionModel *cm = getCollisionModel();
		((AabbElement*)cm->get(0))->bx = origBase;
		((AabbElement*)cm->get(1))->bx = origMiddle;
		((AabbElement*)cm->get(2))->bx = origTip;
	} else {		
		if ( stateCounter < (30) ) {
			changePosT.y -= 2;
			changePosM.y -= 2;

		} else {
			changePosT.y += 2;
			changePosM.y += 2;
		}
		changePosT = axis.rotateToThisAxis(changePosT);
		changePosM = axis.rotateToThisAxis(changePosM);
	
		tip.setRelPos(changePosT);
		middle.setRelPos(changePosM);
		
		((AabbElement*)cm->get(1))->bx = *(middle.fix());	//pm->colBoxes[1] = *(middle.fix());
		((AabbElement*)cm->get(2))->bx = *(tip.fix());		//pm->colBoxes[2] = *(tip.fix());
	}

	currStateDone = (stateCounter == 59);
	modelAnimationState = M_PROBE;
}

/*
 * Attack means slam for the tentacle
 */
void TentacleSObj::attack() {
	// First, rotate ourselves to the player
	if (stateCounter == 0) {
		slamCounter = 0;
		// If there was no player, rotate ourselves to a random angle
		if (!this->playerFound) this->playerAngle = (float)(rand()%(int)(M_PI*2));

		Vec3f rotationAxis = Vec3f(0,0,1);
		Vec4f qAngle = Vec4f(rotationAxis, playerAngle);
		lastRotation = this->getPhysicsModel()->ref->getRot();
		this->getPhysicsModel()->ref->rotate(qAngle);
	}

	slamMotion();

	slamCounter+= 1;

	// we're done!
	//if((attackCounter - attackBuffer)%CYCLE == CYCLE - 1)
	if(stateCounter == (CYCLE+2)*2 - 1)
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
		slamCounter = 0;
		lastRotation = this->getPhysicsModel()->ref->getRot();
	} else {
		slamCounter += (stateCounter % (CYCLE+2) == 0) ? -CYCLE - 1 : 1;
	}

	// Then, every new slam cycle, rotate our tentacle
	//if (stateCounter%CYCLE == 0) {
		Vec3f rotationAxis = Vec3f(0,0,1);
		//Vec4f qAngle = Vec4f(rotationAxis, SLAM_ANGLE);
		Vec4f qAngle = Vec4f(rotationAxis, (float)(SLAM_ANGLE/CYCLE));
		this->getPhysicsModel()->ref->rotate(qAngle);
	//}

	slamMotion();

	// We're done!
	if (stateCounter >= (CYCLE+2)*NUM_SLAMS)
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
	Vec3f changePosM = Vec3f(), changeProportionM = Vec3f();
	Vec3f changePosB = Vec3f();
	Vec3f changePosT = Vec3f(), changeProportionT = Vec3f();

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	if (slamCounter == 0) {
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

	if (slamCounter < CYCLE ) 
	{
		changePosB.z+=2;

		changePosM.z+=6;
		changeProportionM.z+=2;
		changePosM.y+=3;

		changePosT.z+=14;
		changeProportionT.z-=2;
		changePosT.y+=4;
		changeProportionT.y+=4;
	} else if (slamCounter < CYCLE * 2) {
		changePosB.z-=2;

		changePosM.z-=6;
		changeProportionM.z-=2;
		changePosM.y-=3;

		changePosT.z-=14;
		changeProportionT.z+=2;
		changePosT.y-=4;
		changeProportionT.y-=4;
	}
	// Rotate the relative change according to where we're facing
	base.setRelPos(axis.rotateToThisAxis(changePosB));
	
	middle.setRelPos(axis.rotateToThisAxis(changePosM));
	middle.setRelSize(axis.rotateToThisAxis(changeProportionM));

	tip.setRelPos(axis.rotateToThisAxis(changePosT));
	tip.setRelSize(axis.rotateToThisAxis(changeProportionT));
	// Set new collision boxes
	((AabbElement*)cm->get(0))->bx = *(base.fix());
	((AabbElement*)cm->get(1))->bx = *(middle.fix());
	((AabbElement*)cm->get(2))->bx = *(tip.fix());
	
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
	((AabbElement*)cm->get(1))->bx = Box();
	((AabbElement*)cm->get(2))->bx = Box();

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

void TentacleSObj::move() {
	// move in 16
	// move out 18
	int counter = 10;
	CollisionModel *cm = getCollisionModel();
	Box base = ((AabbElement*)cm->get(0))->bx; // this->getPhysicsModel()->colBoxes.at(0);
	Box middle = ((AabbElement*)cm->get(1))->bx;// this->getPhysicsModel()->colBoxes.at(1);
	Box tip = ((AabbElement*)cm->get(2))->bx; // this->getPhysicsModel()->colBoxes.at(2);
	Vec3f changePosM = Vec3f();
	Vec3f changePosB = Vec3f();
	Vec3f changePosT = Vec3f();

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	if (stateCounter == 0) {
 		Box origBase = moveBoxes[0];
		Box origMiddle = moveBoxes[1];
		Box origTip = moveBoxes[2];

		base.setPos(axis.rotateToThisAxis(origBase.getPos()));
		base.setSize(axis.rotateToThisAxis(origBase.getSize()));
		changePosB.z -= counter;

		middle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
		middle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));
		changePosM.z -= counter;

		tip.setPos(axis.rotateToThisAxis(origTip.getPos()));
		tip.setSize(axis.rotateToThisAxis(origTip.getSize()));
		changePosT.z -= counter;
		modelAnimationState = M_EXIT;
	}
	else if (stateCounter < 16)
	{
		changePosB.z -= counter;
		changePosM.z -= counter;
		changePosT.z -= counter;
		modelAnimationState = M_EXIT;
	}
	// Switch positions
	else if (stateCounter == 16)
	{
		Frame* currFrame = this->getPhysicsModel()->ref;
		Frame newFrame = this->overlord->updatePosition(*currFrame, this->getType());
		currFrame->setPos(newFrame.getPos());
		currFrame->setRot(newFrame.getRot());
		
		//Now, set the collision boxes
		Box origBase = moveBoxes[0];
		Box origMiddle = moveBoxes[1];
		Box origTip = moveBoxes[2];

		base.setPos(axis.rotateToThisAxis(origBase.getPos()));
		base.setSize(axis.rotateToThisAxis(origBase.getSize()));
		changePosB.z -= counter * 15;
		
		middle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
		middle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));
		changePosM.z -= counter * 15;

		tip.setPos(axis.rotateToThisAxis(origTip.getPos()));
		tip.setSize(axis.rotateToThisAxis(origTip.getSize()));
		changePosT.z -= counter * 15;
		modelAnimationState = M_ENTER;
	}
	// Wriggle back in
	else 
	{
		if (stateCounter < 16*2) 
		{
			changePosB.z += counter;
			changePosM.z += counter;
			changePosT.z += counter;
		}
		modelAnimationState = M_ENTER;
	}

	// Rotate the relative change according to where we're facing
	base.setRelPos(axis.rotateToThisAxis(changePosB));
	
	middle.setRelPos(axis.rotateToThisAxis(changePosM));
	
	tip.setRelPos(axis.rotateToThisAxis(changePosT));
	
	// Set new collision boxes
	((AabbElement*)cm->get(0))->bx = *(base.fix());
	((AabbElement*)cm->get(1))->bx = *(middle.fix());
	((AabbElement*)cm->get(2))->bx = *(tip.fix());

	currStateDone = (stateCounter == 33);
}

void TentacleSObj::death() {
	modelAnimationState = M_DEATH;

	// No collision boxes in death
	if (stateCounter == 0)
	{
		CollisionModel *cm = getCollisionModel();
		((AabbElement*)cm->get(0))->bx = Box();
		((AabbElement*)cm->get(1))->bx = Box();
		((AabbElement*)cm->get(2))->bx = Box();
	}

	currStateDone = (stateCounter == 20);
}

/*fastForwardAnimation
 *	We have gotten a collision with a static object, so to make sure we don't 
 *    make the collision box go through the object we need to fast forward by calculating
 *    the frame we want to go to and sending that to the client side object.
 *
 *  Author: Bryan
 */

void TentacleSObj::fastForward()
{
	//1. Get the current animation frame
	//2. Get the amount of frames the current animation takes
	//3. If we are in the first half of the animation, go to the opposite side
	//4. Otherwise, go to half way to the last frame?

}