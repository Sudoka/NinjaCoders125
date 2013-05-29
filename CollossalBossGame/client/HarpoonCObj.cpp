#include "HarpoonCObj.h"
#include "ClientEngine.h"
#include "ClientObjectManager.h"
#include <math.h>
#include <Windows.h>
#include "RenderEngine.h"
#include "NetworkData.h"

HarpoonCObj::HarpoonCObj(uint id, char *serializedData) :
	ClientObject(id)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new TestObject %d\n", id);

	ObjectState *state = (ObjectState*)serializedData;

	// todo send -1
	rm = new RenderModel(Point_t(),Quat_t(), (Model)-1);
	deserialize(serializedData);
	pewPew = new HarpoonEffect();
	RE::get()->addParticleEffect(pewPew);

	master = COM::get()->find(this->creatorid);
}


HarpoonCObj::~HarpoonCObj(void)
{
	// delete xctrl;
	delete rm;
	RE::get()->removeParticleEffect(pewPew);
}

bool HarpoonCObj::update() {
	
	pewPew->setPosition(rm->getFrameOfRef()->getPos(), master->getRenderModel()->getFrameOfRef()->getPos(), this->diameter);
    pewPew->update(.33);
	return false;
}

void HarpoonCObj::deserialize(char* newState) {
	diameter = *(int *)newState;
	newState = (newState + 4);
	creatorid = *(int *)newState;
	newState = (newState + 4);

	ObjectState *state = (ObjectState*)newState;

	if (COM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(newState + sizeof(ObjectState));

		rm->colBoxes.clear();
		for (int i=0; i<collState->totalBoxes; i++)
		{
			rm->colBoxes.push_back(collState->boxes[i]);
		}

		rm->getFrameOfRef()->deserialize(newState + sizeof(ObjectState) + sizeof(CollisionState));
	}
	else
	{
		rm->getFrameOfRef()->deserialize(newState + sizeof(ObjectState));
	}
}
