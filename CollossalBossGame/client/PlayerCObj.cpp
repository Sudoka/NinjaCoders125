#include "PlayerCObj.h"
#include "RenderEngine.h"
#include "NetworkData.h"
#include "ClientObjectManager.h"
#include "ClientEngine.h"
#include "ConfigurationManager.h"
#include "defs.h"
#include <math.h>
#include <sstream>

#define DEFAULT_PITCH_10 0.174532925f	//10 degrees or stg like that

PlayerCObj::PlayerCObj(uint id, char *data) :
	ClientObject(id, OBJ_PLAYER)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new PlayerCObj %d\n", id);
	PlayerState *state = (PlayerState*)data;
	this->health = state->health;
	this->charge = state->charge;
	rm = new RenderModel(Point_t(),Quat_t(), state->modelNum);
	ss = new SoundSource();
	char* s1 = CM::get()->find_config("LINK");
	jumpsound = ss->addSound(s1);
	cameraPitch = DEFAULT_PITCH_10;
	ready = false;
	chargingEffect = new ChargeEffect(10);
	// Register with RE, SO SMART :O
	RE::get()->addParticleEffect(chargingEffect);
	this->camHeight = 0;

#if HMAP_TEST
	///////////////////////////////////////////////////////////////
	//TEST
	HMap *hmap = new HMap("../floor_hmap.bmp", 6, 5.0f / 255);
	const float fxo = -CM::get()->find_config_as_float("ROOM_WIDTH") / 2,
				fyo = 0,//CM::get()->find_config_as_float("ROOM_HEIGHT") / 2,
				fzo = -CM::get()->find_config_as_float("ROOM_DEPTH") / 2;
	int skip = 10;
	Point_t pos = Point_t();
	for(int x = 0; x < hmap->getWidth(); x += skip) {
		pos.x = x * hmap->getUnitLength() + fxo;
		for(int z = 0; z < hmap->getLength(); z += skip) {
			pos.y = hmap->getHeightAt(x, z) + fyo;
			pos.z = z * hmap->getUnitLength() + fzo;
			hmapPts.push_back(pos);
		}
	}
	delete hmap;
	///////////////////////////////////////////////////////////////
#endif
}

PlayerCObj::~PlayerCObj(void)
{
	delete rm;
	delete ss;
	RE::get()->removeParticleEffect(chargingEffect);

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
		XboxController *xctrl = CE::getController();
		if(xctrl->isConnected()) {
			if(xctrl->getState().Gamepad.bLeftTrigger) {
				cameraPitch = DEFAULT_PITCH_10; //10
			} else if(fabs((float)xctrl->getState().Gamepad.sThumbRY) > DEADZONE) {
				cameraPitch += atan(((float)xctrl->getState().Gamepad.sThumbRY / (JOY_MAX * 8)));
				if (cameraPitch > M_PI / 2.f) {
					cameraPitch = (float)M_PI / 2.f;
				} else if(cameraPitch < -M_PI / 4) {
					cameraPitch = (float)-M_PI / 4.f;
				}
			}

			if (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) camHeight++;
			if (xctrl->getState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) camHeight--;
		}

		Vec3f gravity = dirVec(COM::get()->getWorldState()->gravDir)*-1;

		Point_t objPos = rm->getFrameOfRef()->getPos() + gravity*(float)camHeight;
		RE::get()->getCamera()->update(objPos, camRot, cameraPitch);
		showStatus();
		chargingEffect->setPosition(objPos, (int)charge);
		chargingEffect->update(.33f);


#if HMAP_TEST
		///////////////////////////////////////////////////////////////
		//TEST
		RE::get()->getColBxPts()->addParticles(hmapPts);
		///////////////////////////////////////////////////////////////
#endif
	}

	if(this->sTrig == SOUND_PLAYER_JUMP)
	{
		ss->playOneShot(jumpsound);
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
