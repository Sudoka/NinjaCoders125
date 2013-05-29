#include "RageCObj.h"
#include "ClientObjectManager.h"

RageCObj::RageCObj(uint id, char *serializedData) :
	ClientObject(id, OBJ_RAGE)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new RageCObj %d\n", id);

	ObjectState *state = (ObjectState*)serializedData;

	rm = new RenderModel(Point_t(), Quat_t(), state->modelNum);
	deserialize(serializedData);
	push = new PushAwayEffect();
	RE::get()->addParticleEffect(push);
}


RageCObj::~RageCObj(void)
{
	RE::get()->removeParticleEffect(push);
	delete rm;
}

bool RageCObj::update() {
	push->setPosition(rm->getFrameOfRef()->getPos());
	push->update(.33);
	return false;
}

void RageCObj::deserialize(char* newState) {
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