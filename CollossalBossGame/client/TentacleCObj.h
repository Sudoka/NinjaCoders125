#pragma once
#include "ClientObject.h"
#include "SmokeEffect.h"
#include "PushAwayEffect.h"
#include "PortalEffect.h"
#include "SoundSource.h"

class TentacleCObj : public ClientObject
{
public:
	TentacleCObj(uint id, char *serializedData);
	virtual ~TentacleCObj(void);

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);
	virtual RenderModel * getBox();

private:
	RenderModel * box;
	RenderModel *rm;
	SoundSource *ss;
	SmokeEffect* smoking;
	PortalEffect* portal; 
	bool fogging;
	bool startedFogging;
	float density;
	float densityCounter;
	
	//sounds
	MonsterSoundTrigger sTrig;
	MonsterSoundState sState;

	//sound ids
	uint roarsound;
	uint headRoarSound;
	uint shootSound;
	
	//sound volumes
	float tentRoarVol;
	float headRoarVol;
	float headShootVol;
	//uint slamsound;
};

