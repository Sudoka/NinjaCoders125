#include "TentacleCObj.h"
#include "ClientObjectManager.h"
#include "RenderEngine.h"

TentacleCObj::TentacleCObj(uint id, char *data) : ClientObject(id, OBJ_TENTACLE)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new TentacleCObj %d\n", id);
	TentacleState *state = (TentacleState*)data;
	rm = new RenderModel(Point_t(), Quat_t(), state->modelNum);
	smoking = new SmokeEffect();
	RE::get()->addParticleEffect(smoking);
	portal = new PortalEffect();
	RE::get()->addParticleEffect(portal);
	startedFogging = false;
	fogging = false;
	density = 0.f;
	densityCounter = 0.f;
}

TentacleCObj::~TentacleCObj(void)
{
	if(density > 0) RE::get()->stopFog(0);
	RE::get()->removeParticleEffect(smoking);
	RE::get()->removeParticleEffect(portal);
	delete rm;
}

RenderModel* TentacleCObj::getBox() {
	//1. get the box model
	//2. return the model
	return box;
}

bool TentacleCObj::update() {
	portal->setPosition(rm->getFrameOfRef()->getPos());
	portal->update(.33);
	if(fogging || startedFogging)
	{
		smoking->fogging = true;
		startedFogging = true;

		float change = .00002;
		if(densityCounter < .01)
		{
			RE::get()->startFog(density);
			smoking->setPosition(rm->getFrameOfRef()->getPos());
			smoking->update(.33); 
			density += change;
		}
		else
		{
			if(density <= .00002) 
			{
				smoking->kill();
				density = 0;
				startedFogging = false;
				densityCounter = 0.f;
				change = 0.f;
				smoking->fogging = false;
			}
			else
			{
				density -= 2*change;
			}
			RE::get()->stopFog(density);
		}
		densityCounter += change;
	}
	return false;
}

void TentacleCObj::deserialize(char* newState) {
	TentacleState *state = (TentacleState*)newState;
	this->getRenderModel()->setModelState(state->animationState);
	this->fogging = state->fog;
	if (COM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(newState + sizeof(TentacleState));

		rm->colBoxes.clear();
		for (int i=0; i<collState->totalBoxes; i++)
		{
			rm->colBoxes.push_back(collState->boxes[i]);
		}

		rm->getFrameOfRef()->deserialize(newState + sizeof(TentacleState) + sizeof(CollisionState));
	}
	else
	{
		rm->getFrameOfRef()->deserialize(newState + sizeof(TentacleState));
	}
}