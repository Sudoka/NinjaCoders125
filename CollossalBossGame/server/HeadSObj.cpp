#include "HeadSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "RageSObj.h"
#include "FireBallSObj.h"

HeadSObj::HeadSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master) :
											MonsterPartSObj(id, modelNum, pos, rot, master)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new HeadSObj %d\n", id);

	// Head config items
	this->targettingDist = CM::get()->find_config_as_int("FIREBALL_MIN_DIST");
	this->fireballForce = CM::get()->find_config_as_int("FIREBALL_FORCE");
	this->fireballDamage = CM::get()->find_config_as_int("FIREBALL_DAMAGE");
	this->fireballDiameter = CM::get()->find_config_as_int("FIREBALL_DIAMETER");
	this->headBoxSize = CM::get()->find_config_as_int("HEAD_BOX_SIZE");

	/////////////// Collision Boxes /////////////////
	idleBoxes[0] = CM::get()->find_config_as_box("BOX_HEAD_BASE"); 
	idleBoxes[1] = CM::get()->find_config_as_box("BOX_HEAD_MID");
	idleBoxes[2] = CM::get()->find_config_as_box("BOX_HEAD_TIP");

	shootBoxes[0] = CM::get()->find_config_as_box("BOX_HEAD_BASE_SHOOT"); 
	shootBoxes[1] = CM::get()->find_config_as_box("BOX_HEAD_MID_SHOOT");
	shootBoxes[2] = CM::get()->find_config_as_box("BOX_HEAD_TIP_SHOOT");

	CollisionModel *cm = getCollisionModel();

	for (int i=0; i<3; i++) {
		assert((cm->add(new AabbElement(idleBoxes[i])) == i) && "Your physics model is out of sync with the rest of the world...");
	}

	modelAnimationState = M_IDLE;
}


HeadSObj::~HeadSObj(void)
{
}

void HeadSObj::idle() {
	modelAnimationState = M_IDLE;

	CollisionModel *cm = getCollisionModel();
	Box base =	 ((AabbElement*)cm->get(0))->bx;
	Box middle = ((AabbElement*)cm->get(1))->bx;
	Box tip =	 ((AabbElement*)cm->get(2))->bx;

	Vec3f changePosT = Vec3f();

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	if (stateCounter == 0)
	{
		// Keep initial idle boxes
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
	else if (stateCounter < 55) { changePosT.x -= 1; }
	else if (stateCounter < 60) { /* stay still for a sec */ }
	else if (stateCounter < 74) { changePosT.x += 3; }
	else if (stateCounter < 104) { changePosT.x += 2; }
	else if (stateCounter < 136) { changePosT.x -= 1; }
	else {
		// Return to initial idle boxes
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

	// Rotate the relative change according to where we're facing
	changePosT = axis.rotateToThisAxis(changePosT);
	tip.setRelPos(changePosT);

	// Set new collision boxes
	((AabbElement*)cm->get(0))->bx = *(base.fix());
	((AabbElement*)cm->get(1))->bx = *(middle.fix());
	((AabbElement*)cm->get(2))->bx = *(tip.fix());

	currStateDone = (stateCounter == 163);
}

void HeadSObj::probe() {
	modelAnimationState = M_PROBE;

	CollisionModel *cm = getCollisionModel();
	Box base =	 ((AabbElement*)cm->get(0))->bx;
	Box middle = ((AabbElement*)cm->get(1))->bx;
	Box tip =	 ((AabbElement*)cm->get(2))->bx;

	Vec3f changePosT = Vec3f();

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	if (stateCounter == 0)
	{
		// Keep initial idle boxes
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
	else if (stateCounter < 30) { changePosT.x += .5; }
	else if (stateCounter < 44) { changePosT.x -= 1; }
	else if (stateCounter < 58) { changePosT.x -= .75; }
	else if (stateCounter < 60) { /* stay still */ }
	else if (stateCounter < 80) { changePosT.x += .5; }
	else {
		// Return to initial idle boxes
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

	// Rotate the relative change according to where we're facing
	changePosT = axis.rotateToThisAxis(changePosT);
	tip.setRelPos(changePosT);

	// Set new collision boxes
	((AabbElement*)cm->get(0))->bx = *(base.fix());
	((AabbElement*)cm->get(1))->bx = *(middle.fix());
	((AabbElement*)cm->get(2))->bx = *(tip.fix());

	currStateDone = (stateCounter == 85);
}

#define SHOOT_CYCLE 85

void HeadSObj::shootFireball() {
	modelAnimationState = M_ENTER; // M_ATTACK

	CollisionModel *cm = getCollisionModel();
	Box base =	 ((AabbElement*)cm->get(0))->bx;
	Box middle = ((AabbElement*)cm->get(1))->bx;
	Box tip =	 ((AabbElement*)cm->get(2))->bx;

	Vec3f changePosT = Vec3f(), changePosM = Vec3f();

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	// Set up initial collision boxes
	if (stateCounter % SHOOT_CYCLE == 0)
	{
		Box origBase = shootBoxes[0];
		Box origMiddle = shootBoxes[1];
		Box origTip = shootBoxes[2];

		base.setPos(axis.rotateToThisAxis(origBase.getPos()));
		base.setSize(axis.rotateToThisAxis(origBase.getSize()));

		middle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
		middle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));

		tip.setPos(axis.rotateToThisAxis(origTip.getPos()));
		tip.setSize(axis.rotateToThisAxis(origTip.getSize()));
	}
	else if (stateCounter % SHOOT_CYCLE < 10) { changePosT.y += 1;	/* middle stays */	}
	else if (stateCounter % SHOOT_CYCLE < 19) { changePosT.y += 1.75;	changePosM.y += 1;	}
	else if (stateCounter % SHOOT_CYCLE < 25) {  /* head stays */		changePosM.y += 1;	}
	else if (stateCounter % SHOOT_CYCLE < 30) { changePosT.y -= 5;	/* middle stays */	}
	else if (stateCounter % SHOOT_CYCLE < 35) { changePosT.y -= 2;	changePosM.y -= 3;	}
	else if (stateCounter % SHOOT_CYCLE < 50) { /* head stays */		/* middle stays */	}
	else if (stateCounter % SHOOT_CYCLE < 72) { changePosT.y += .5;	/* middle stays */	}
	else {
		// Return to initial boxes
		Box origBase = shootBoxes[0];
		Box origMiddle = shootBoxes[1];
		Box origTip = shootBoxes[2];

		base.setPos(axis.rotateToThisAxis(origBase.getPos()));
		base.setSize(axis.rotateToThisAxis(origBase.getSize()));

		middle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
		middle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));

		tip.setPos(axis.rotateToThisAxis(origTip.getPos()));
		tip.setSize(axis.rotateToThisAxis(origTip.getSize()));
	}

	// Rotate the relative change according to where we're facing
	changePosT = axis.rotateToThisAxis(changePosT);
	changePosM = axis.rotateToThisAxis(changePosM);

	tip.setRelPos(changePosT);
	middle.setRelPos(changePosM);

	// Set new collision boxes
	((AabbElement*)cm->get(0))->bx = *(base.fix());
	((AabbElement*)cm->get(1))->bx = *(middle.fix());
	((AabbElement*)cm->get(2))->bx = *(tip.fix());

	
	// We actually shoot in a specific frame
	if (stateCounter % SHOOT_CYCLE == 40)
	{
		// Find our head position
		Box headBox = tip;
		Vec3f headPos = headBox.getPos() + this->getPhysicsModel()->ref->getPos();

		// If there was no player, pick a random target
		if (!this->playerFound) {
			Vec3f randTarget = Vec3f(-100.f + rand()%200,-100.f + rand()%200, rand()%100);
			randTarget = axis.rotateToThisAxis(randTarget);
			this->playerPos = randTarget + headPos;
		}

		// Determine our bullet path
		Vec3f bulletPath = this->playerPos - headPos;
		bulletPath.normalize();

		// move the bullet a little bit along our path, just enough so it clears the head
		Vec3f offset = bulletPath * ((float)this->headBoxSize * 1.5f); // 1.5 is sqrt(2), ask franklin for the math behind it
		Vec3f bulletPos = headPos + offset;

		FireBallSObj * fbso = new FireBallSObj(	SOM::get()->genId(), 
												(Model)-1, bulletPos, 
												bulletPath * (float)this->fireballForce, 
												this->fireballDamage, 
												this->fireballDiameter);
		SOM::get()->add(fbso);
	}
}

void HeadSObj::attack() {
	// Shoot!
	shootFireball();

	currStateDone = (stateCounter == SHOOT_CYCLE - 1);
}

#define NUM_SHOTS 4
void HeadSObj::combo() {
	// Fire away!
	this->playerFound = false; // force shootFireball() to pick a random target xD
	shootFireball();

	// This makes us fire multiple times!
	currStateDone = stateCounter >= SHOOT_CYCLE*NUM_SHOTS;
}

void HeadSObj::spike() {
	modelAnimationState = M_EXIT; // M_SPIKE;

	// Keep initial idle boxes
	Box origBase = idleBoxes[0];
	Box origMiddle = idleBoxes[1];
	Box origTip = idleBoxes[2];

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();
	origBase.rotate(axis);
	origMiddle.rotate(axis);
	origTip.rotate(axis);

	CollisionModel *cm = getCollisionModel();
	((AabbElement*)cm->get(0))->bx = origBase;
	((AabbElement*)cm->get(1))->bx = origMiddle;
	((AabbElement*)cm->get(2))->bx = origTip;

	currStateDone = stateCounter >= 45;
}

// FOR NOW this is the same as in the tentacle
void HeadSObj::rage() {
	modelAnimationState = M_DEATH; // M_RAGE;

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
	currStateDone = stateCounter >= max(RageSObj::lifetime, 60);
}

void HeadSObj::move() {
	// Wriggle out
	if (stateCounter <= 27)
	{
		modelAnimationState = M_SPIKE; // M_EXIT;
	}
	// Switch positions
	if (stateCounter == 27)
	{
		Frame* currFrame = this->getPhysicsModel()->ref;
		Frame newFrame = this->overlord->updatePosition(*currFrame, this->getType());
		currFrame->setPos(newFrame.getPos());
		currFrame->setRot(newFrame.getRot());
	}
	// Wriggle back in
	if (stateCounter > 27)
	{
		modelAnimationState = M_ATTACK; // M_ENTER;
	}

	currStateDone = (stateCounter == 60);
}

void HeadSObj::death() {
	modelAnimationState = M_RAGE; // M_DEATH;

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