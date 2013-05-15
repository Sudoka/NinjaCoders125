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
	startedFogging = false;
	fogging = false;
}

TentacleCObj::~TentacleCObj(void)
{
	RE::get()->removeParticleEffect(smoking);
	delete rm;
}

RenderModel* TentacleCObj::getBox() {
	//1. get the box model
	//2. return the model
	return box;
}

bool TentacleCObj::update() {
	if(fogging || startedFogging)
	{
		startedFogging = true;
		static float density = 0.f;
		static float densityCounter = 0.f;
		float change = .00002;
		if(densityCounter < .01)
		{
			smoking->setPosition(rm->getFrameOfRef()->getPos());
			smoking->update(.33); 
			density += change;
			RE::get()->startFog(density);
		}
		else
		{
			RE::get()->stopFog(density);
			density -= 2*change;
			if(density <= 0) 
			{
				smoking->kill();
				density = 0;
				startedFogging = false;
			}
		}
		densityCounter += change;
	}
	return false;
}

void TentacleCObj::deserialize(char* newState) {
	TentacleState *state = (TentacleState*)newState;
	this->getRenderModel()->setModelState(state->animationState);
	//this->fogging = state->isFogging;
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