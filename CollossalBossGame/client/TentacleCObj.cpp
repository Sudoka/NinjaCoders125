#include "TentacleCObj.h"
#include "ClientObjectManager.h"
#include "RenderEngine.h"

TentacleCObj::TentacleCObj(uint id, char *data) : ClientObject(id, OBJ_TENTACLE)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new TentacleCObj %d\n", id);
	MonsterPartState *state = (MonsterPartState*)data;
	rm = new RenderModel(Point_t(), Quat_t(), state->modelNum);
	smoking = new SmokeEffect();
	RE::get()->addParticleEffect(smoking);
	//portal = new PortalEffect();
	//RE::get()->addParticleEffect(portal);
	startedFogging = false;
	fogging = false;
	density = 0.f;
	densityCounter = 0.f;

	//set up sounds
	ss = new SoundSource();
	DC::get()->print("[Audio] Loading Tentacle Roar...\n");
	char* s1 = CM::get()->find_config("TENTACLE_ROAR");
	float atten = CM::get()->find_config_as_float("TENTACLE_ROAR_ATTEN");
	tentRoarVol = CM::get()->find_config_as_float("TENTACLE_ROAR_VOL");
	roarsound = ss->addSound(s1,true,atten);
	DC::get()->print("[Audio] Loading Head Roar...\n");
	char* s2 = CM::get()->find_config("HEAD_ROAR");
	atten = CM::get()->find_config_as_float("HEAD_ROAR_ATTEN");
	headRoarVol = CM::get()->find_config_as_float("HEAD_ROAR_VOL");
	headRoarSound = ss->addSound(s2,true,atten);
	DC::get()->print("[Audio] Loading Fireball Sound...\n");
	char* s3 = CM::get()->find_config("FIREBALL_SOUND");
	atten = CM::get()->find_config_as_float("FIREBALL_SOUND_ATTEN");
	headShootVol = CM::get()->find_config_as_float("FIREBALL_SOUND_VOL");
	shootSound = ss->addSound(s3,true,atten);
}

TentacleCObj::~TentacleCObj(void)
{
	if(density > 0) RE::get()->stopFog(0);
	if(RE::get() != NULL)
	{
		RE::get()->removeParticleEffect(smoking);
		//RE::get()->removeParticleEffect(portal);
	}
	delete rm;
}

RenderModel* TentacleCObj::getBox() {
	//1. get the box model
	//2. return the model
	return box;
}

bool TentacleCObj::update() {

	Vec3f soundPos;
	switch(this->sTrig) {
	case SOUND_TENTACLE_ROAR:
		soundPos = this->getRenderModel()->getFrameOfRef()->getPos();
		ss->playOneShot3D(roarsound,tentRoarVol,soundPos);
		break;
	case SOUND_HEAD_ROAR:
		soundPos = this->getRenderModel()->getFrameOfRef()->getPos();
		ss->playOneShot3D(headRoarSound,headRoarVol,soundPos);
		break;
	case SOUND_HEAD_SHOOT:
		soundPos = this->getRenderModel()->getFrameOfRef()->getPos();
		ss->playOneShot3D(shootSound,headShootVol,soundPos);
	}

	if(rm->getFrameOfRef()->getPos().y > 2900) {
		rm->setInvisible(true);
	} else {
		rm->setInvisible(false);
	}
	//portal->setPosition(rm->getFrameOfRef()->getPos());
	//portal->update(.33);
	//rm->setInvisible(true);
	if(fogging || startedFogging)
	{
		smoking->fogging = true;
		startedFogging = true;

		float change = .00002f;
		// NOTE: to make fog super quick change .01 to .00004
		if(densityCounter < .004) { 
			RE::get()->startFog(density);
			smoking->setPosition(rm->getFrameOfRef()->getPos());
			smoking->update(.33f); 
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
	MonsterPartState *state = (MonsterPartState*)newState;
	this->getRenderModel()->setModelState(state->animationState);
	//Collision sensing: Client side. Just set the animation frame equal to the sent animation frame
	if (state->animationFrame >= 0)
		this->getRenderModel()->setAnimationFrame(state->animationFrame);
	this->fogging = state->fog;
	if (COM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(newState + sizeof(MonsterPartState));

		rm->colBoxes.clear();
		for (int i=0; i<collState->totalBoxes; i++)
		{
			rm->colBoxes.push_back(collState->boxes[i]);
		}

		rm->getFrameOfRef()->deserialize(newState + sizeof(MonsterPartState) + sizeof(CollisionState));
	}
	else
	{
		rm->getFrameOfRef()->deserialize(newState + sizeof(MonsterPartState));
	}
	this->sTrig = state->sTrig;
	this->sState = state->sState;
}