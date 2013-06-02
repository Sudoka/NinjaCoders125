#include "RageSObj.h"
#include "ServerObjectManager.h"

int RageSObj::lifetime;

RageSObj::RageSObj(uint id, Point_t pos) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new RageSObj %d\n", id);
	this->modelNum = (Model)-1; // these guys are invisible
	pm = new PhysicsModel(pos, Quat_t(), 1);
	getCollisionModel()->add(new AabbElement(Box()));
	age = 0;
	this->setFlag(IS_STATIC, 1);
	lifetime = 80;
}


RageSObj::~RageSObj(void)
{
	delete pm;
}

#define GROW_UNIT 4.0
bool RageSObj::update()
{
	// Grow our box!
	AabbElement* colE = (AabbElement*)(getCollisionModel()->get(0));
	Vec4f axis = this->getPhysicsModel()->ref->getRot();
	Vec3f changePos = Vec3f(), changeProportion = Vec3f();

	changeProportion += Vec3f(GROW_UNIT,GROW_UNIT,GROW_UNIT);
	changePos -= Vec3f(GROW_UNIT/2.f, GROW_UNIT/2.f, GROW_UNIT/2.f);

	changeProportion = axis.rotateToThisAxis(changeProportion);
	changePos = axis.rotateToThisAxis(changePos);

	colE->bx.setRelSize(changeProportion);
	colE->bx.setRelPos(changePos);
	colE->bx.fix();

	age++;
	return age >= lifetime;
}

int RageSObj::serialize(char * buf) {
	ObjectState *state = (ObjectState*)buf;
	state->modelNum = modelNum;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(ObjectState));

		vector<CollisionElement*>::iterator cur = getCollisionModel()->getStart(),
			end = getCollisionModel()->getEnd();

		collState->totalBoxes = min(end - cur, maxBoxes);

		for(int i=0; i < collState->totalBoxes; i++, cur++) {
			//The collision box is relative to the object's frame-of-ref.  Get the non-relative collision box
			collState->boxes[i] = ((AabbElement*)(*cur))->bx + pm->ref->getPos();
		}

		return pm->ref->serialize(buf + sizeof(ObjectState) + sizeof(CollisionState)) + sizeof(ObjectState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
	}
}