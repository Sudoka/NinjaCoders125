#pragma once
#include "RenderModel.h"
#include "ChargeEffect.h"
#include "ClientObject.h"
#include "SoundSource.h"

#define HMAP_TEST 0
#if HMAP_TEST
#include "HMap.h"
#endif


class PlayerCObj : public ClientObject
{
public:
	PlayerCObj(uint id, char *data);
	virtual ~PlayerCObj(void);

	virtual void showStatus();

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);

	virtual int getTypeInt();

	int ready;

protected:
	RenderModel * box;
	int camHeight;
	int health;
	int bStates;
	float charge;
	RenderModel *rm;
	SoundSource *ss;
	float camPitch;
	float camDist;
	Quat_t camRot;
	int camOffset;

	//sound ids
	uint jumpsound;
	uint chargeSound;
	uint rifleSound;
	uint swordSound;
	uint hookshotSound;	
	uint transformSound;

	//sound volumes
	float jumpVol;
	float chargeVol;
	float rifleVol;
	float swordVol;
	float hookshotVol;
	float transformVol;

	//channel ids
	uint hookshotChannel;
	bool hookshotPlaying;
	uint transformChannel;
	bool transformPlaying;
	

	PlayerSoundState sState;
	PlayerSoundTrigger sTrig;

#if HMAP_TEST
	///////////////////////////////////////////////////////////////
	//TEST
	vector<Point_t> hmapPts;
	///////////////////////////////////////////////////////////////
#endif
};

