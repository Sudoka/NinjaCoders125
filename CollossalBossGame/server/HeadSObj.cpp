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

	currStateDone = true;
}

void HeadSObj::probe() {
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

	currStateDone = true;
}

void HeadSObj::shootFireball() {
	// For now, keep initial idle boxes
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

	// Find our head position
	Box headBox = ((AabbElement*)getCollisionModel()->get(2))->bx;
	Vec3f headPos = headBox.getPos() + this->getPhysicsModel()->ref->getPos();

	// If there was no player, pick a random target
	if (!this->playerFound) this->playerPos = Vec3f(-100 + rand()%200,-100 + rand()%200,-100 + rand()%200) + headPos;

	// Determine our bullet path
	Vec3f bulletPath = this->playerPos - headPos;
	bulletPath.normalize();

	// move the bullet a little bit along our path, just enough so it clears the head
	Vec3f offset = bulletPath * this->headBoxSize * 1.5; // 1.5 is sqrt(2), ask franklin for the math behind it
	Vec3f bulletPos = headPos + offset;

	FireBallSObj * fbso = new FireBallSObj(	SOM::get()->genId(), 
											(Model)-1, bulletPos, 
											bulletPath * this->fireballForce, 
											this->fireballDamage, 
											this->fireballDiameter);
	SOM::get()->add(fbso);
}

void HeadSObj::attack() {
	// First, create our bullet
	if (stateCounter == 0)
	{
		shootFireball();
	}

	currStateDone = (stateCounter == SHOOT_CYCLE);
}

#define NUM_SHOTS 4
void HeadSObj::combo() {
	// Fire away!
	if (stateCounter%SHOOT_CYCLE == 0) {
		this->playerFound = false; // force shootFireball() to pick a random target xD
		shootFireball();
	}

	currStateDone = stateCounter >= SHOOT_CYCLE*NUM_SHOTS;
}

void HeadSObj::spike() {
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

	currStateDone = true;
}

// FOR NOW this is the same as in the tentacle
void HeadSObj::rage() {
	//modelAnimationState = M_RAGE;

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