#include "PlayerCObj.h"
#include "NetworkData.h"
#include "ClientObjectManager.h"
#include "ClientEngine.h"
#include "ConfigurationManager.h"
#include "defs.h"
#include <math.h>
#include <sstream>
#include "RenderEngine.h"

#define DEFAULT_PITCH_10 0.30550876f	//10 degrees or stg like that

PlayerCObj::PlayerCObj(uint id, char *data) :
	ClientObject(id, OBJ_PLAYER)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new PlayerCObj %d\n", id);
	PlayerState *state = (PlayerState*)data;
	this->health = state->health;
	this->charge = state->charge;
	rm = new RenderModel(Point_t(),Quat_t(), state->modelNum);

	//sound
	//TODO_HARO: These sounds are not being found any more. I didn't change anything in the config
	//but after I merged in the latest changes in develop FMOD says it can't find the files. 
	//HELP!!! I would stick around and figure it out, but its already 9:05 and I need to hit the road. :(
	//Thank you so much!
	ss = new SoundSource();
	DC::get()->print("[Audio] Loading jump sound...\n");
	char* s1 = CM::get()->find_config("LINK");
	float atten = CM::get()->find_config_as_float("LINK_ATTEN");
	jumpVol = CM::get()->find_config_as_float("LINK_VOL");
	jumpsound = ss->addSound(s1,true,atten);
	DC::get()->print("[Audio] Loading cyborg charge sound...\n");
	char* s2 = CM::get()->find_config("CHARGE_SOUND");
	atten = CM::get()->find_config_as_float("CHARGE_SOUND_ATTEN");
	chargeVol = CM::get()->find_config_as_float("CHARGE_SOUND_VOL");
	chargeSound = ss->addSound(s2,true,atten);
	DC::get()->print("[Audio] Loading rifle sound...\n");
	char* s3 = CM::get()->find_config("RIFLE_SOUND");
	atten = CM::get()->find_config_as_float("RIFLE_SOUND_ATTEN");
	rifleVol = CM::get()->find_config_as_float("RIFLE_SOUND_VOL");
	rifleSound = ss->addSound(s3,true,atten);
	DC::get()->print("[Audio] Loading sword sound...\n");
	char* s4 = CM::get()->find_config("SWORD_SOUND");
	atten = CM::get()->find_config_as_float("SWORD_SOUND_ATTEN");
	swordVol = CM::get()->find_config_as_float("SWORD_SOUND_VOL");
	swordSound = ss->addSound(s4,true,atten);
	DC::get()->print("[Audio] Loading harpoon sound...\n");
	char* s5 = CM::get()->find_config("HOOKSHOT_SOUND");
	atten = CM::get()->find_config_as_float("HOOKSHOT_SOUND_ATTEN");
	hookshotVol = CM::get()->find_config_as_float("HOOKSHOT_SOUND_VOL");
	hookshotSound = ss->addSound(s5,true,atten);
	hookshotPlaying = false;
	DC::get()->print("[Audio] Loading transformation sound...\n");
	char* s6 = CM::get()->find_config("SCIENTIST_SOUND");
	atten = CM::get()->find_config_as_float("SCIENTIST_SOUND_ATTEN");
	transformVol = CM::get()->find_config_as_float("SCIENTIST_SOUND_VOL");
	transformSound = ss->addSound(s6,true,atten);
	transformPlaying = false;

	camDist = 0;
	camPitch = DEFAULT_PITCH_10;
	ready = false;
	this->camHeight = CM::get()->find_config_as_int("CAM_HEIGHT");
	this->camOffset = 0;
}

PlayerCObj::~PlayerCObj(void)
{
	delete rm;
	delete ss;

	// todo, figure out what it should be then config
	camHeight = 0;

	//Quit the game
	CE::get()->exit();
}

void PlayerCObj::showStatus()
{
	std::stringstream status;
	status << "Health" << "\n";
	RE::get()->setHUDText(status.str(), health, charge);
}

bool PlayerCObj::update() {
	//Move the camera to follow the player
	if(COM::get()->player_id == getId()) {
		rm->setInvisible(bStates & PLAYER_ZOOM);
		RE::get()->setFPV(bStates & PLAYER_ZOOM); 
		XboxController *xctrl = CE::getController();
		if(xctrl->isConnected()) {
			/*
			if(xctrl->getState().Gamepad.bLeftTrigger) {
				camPitch = DEFAULT_PITCH_10; //10
			} else if(fabs((float)xctrl->getState().Gamepad.sThumbRY) > DEADZONE) {
				camPitch += atan(((float)xctrl->getState().Gamepad.sThumbRY / (JOY_MAX * 8)));
				if (camPitch > M_PI / 2.f) {
					camPitch = (float)M_PI / 2.f;
				} else if(camPitch < -M_PI / 4) {
					camPitch = (float)-M_PI / 4.f;
				}
			}
			*/

			//camHeight
			if ((xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_X)) RE::get()->brightness+= 0.01f;
			if ((xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_X)) RE::get()->brightness-= 0.01f;

			if ((xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) camHeight++;
			else if ((xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) camHeight--;
			else if ((xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) camOffset--;
			else if ((xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) camOffset++;
			else if ((xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_X)) 
			{
				camHeight = CM::get()->find_config_as_int("CAM_HEIGHT");
				camOffset = 0;
			}
		}

		Vec3f gravity = dirVec(COM::get()->getWorldState()->gravDir)*-1;

		Point_t objPos = rm->getFrameOfRef()->getPos() + gravity*(float)camHeight;
		RE::get()->getCamera()->update(objPos, camRot, camPitch, camDist + camOffset);
		showStatus();
	}

	Vec3f playerPos = rm->getFrameOfRef()->getPos();
	//one shot sfx
	switch(this->sTrig) {
	case SOUND_PLAYER_JUMP:
		ss->playOneShot3D(jumpsound,jumpVol,playerPos);
		break;
	case SOUND_CYBORG_CHARGE:
		ss->playOneShot3D(chargeSound,chargeVol,playerPos);
		break;
	case SOUND_SHOOTER_FIRE:
		ss->playOneShot3D(rifleSound,rifleVol,playerPos);
		break;
	case SOUND_CYBORG_SWORD:
		ss->playOneShot3D(swordSound,swordVol,playerPos);
		break;
	}

	//looping sfx
	switch(this->sState) {
	case SOUND_MECHANIC_HARPOON_ON:
		if(hookshotPlaying) {
			ss->updateSoundPos(hookshotChannel,playerPos);
		} else {
			hookshotChannel = ss->playLoop3D(hookshotSound,hookshotVol,playerPos,5150,20150);
			hookshotPlaying = true;
		}
		break;
	case SOUND_MECHANIC_HARPOON_OFF:
		if(hookshotPlaying) {
			ss->stopLoop(hookshotChannel);
			hookshotPlaying = false;
		}
		break;
	case SOUND_SCIENTIST_COPY_ON:
		if(transformPlaying) {
			ss->updateSoundPos(transformChannel,playerPos);
		} else {
			transformChannel = ss->playLoop3D(transformSound,transformVol,playerPos,2000,24000);
			transformPlaying = true;
		}
		break;
	case SOUND_SCIENTIST_COPY_OFF:
		if(transformPlaying) {
			ss->stopLoop(transformChannel);
			transformPlaying = false;
		}
	}

	return false;
}

void PlayerCObj::deserialize(char* newState) {
	PlayerState *state = (PlayerState*)newState;
	this->health = state->health;
	this->ready = state->ready;
	this->charge = state->charge;
	this->sState = state->sState;
	this->sTrig = state->sTrig;
	camRot = state->camRot;
	camPitch = state->camPitch;
	camDist = state->camDist;
	bStates = state->bStates;

	if(this->ready == false) {
		RE::get()->gamestarted = false;
		// chargingEffect->kill();
	}

	this->getRenderModel()->setModelState(state->animationstate);

	if (COM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(newState + sizeof(PlayerState));

		rm->colBoxes.clear();
		for (int i=0; i<collState->totalBoxes; i++)
		{
			rm->colBoxes.push_back(collState->boxes[i]);
		}

		rm->getFrameOfRef()->deserialize(newState + sizeof(PlayerState) + sizeof(CollisionState));
	}
	else
	{
		rm->getFrameOfRef()->deserialize(newState + sizeof(PlayerState));
	}
}

int PlayerCObj::getTypeInt()
{
	return -1;
}
