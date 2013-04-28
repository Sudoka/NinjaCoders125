#include "PlayerCObj.h"
#include "RenderEngine.h"
#include "NetworkData.h"
#include "ClientObjectManager.h"
#include "ClientEngine.h"
#include "ConfigurationManager.h"
#include "defs.h"
#include <math.h>
#include <sstream>

#define DEFAULT_PITCH 0.174532925f	//10 degrees or stg like that

PlayerCObj::PlayerCObj(uint id, char *data) :
	ClientObject(id)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new PlayerCObj %d\n", id);
	PlayerState *state = (PlayerState*)data;
	this->health = state->health;
	rm = new RenderModel(Point_t(300.f, 500.f, 0.f),Rot_t(0.f, 0.f, M_PI), state->modelNum, Vec3f(2.f,2.f,2.f));
	cameraPitch = 0;
}

PlayerCObj::~PlayerCObj(void)
{
	delete rm;

	//Quit the game
	CE::get()->exit();
}

void PlayerCObj::showStatus()
{
	std::stringstream status;
	status << "Player " << getId() << "\n";
	//std::string s1 ("[");
	//std::string s2 (floor(health/20 + 0.5f), '~');
	//std::string s3 ("]");
	//status << s1 << s2 << s3;
	if (health <= 0) status << "\nGAME OVER";
	RE::get()->setHUDText(status.str(), health);
}

bool PlayerCObj::update() {
	//Move the camera to follow the player
	if(COM::get()->player_id == getId()) {
		XboxController *xctrl = CE::getController();
		if(xctrl->isConnected()) {
			if(xctrl->getState().Gamepad.bLeftTrigger) {
				cameraPitch = DEFAULT_PITCH; //10
			} else if(fabs((float)xctrl->getState().Gamepad.sThumbRY) > DEADZONE) {
				cameraPitch += atan(((float)xctrl->getState().Gamepad.sThumbRY / (JOY_MAX * 8)));
				if (cameraPitch > M_PI / 2.f) {
					cameraPitch = M_PI / 2.f;
				} else if(cameraPitch < -M_PI / 4) {
					cameraPitch = -M_PI / 4.f;
				}
			}
		}

		Point_t objPos = rm->getFrameOfRef()->getPos();
		Quat_t objDir = rm->getFrameOfRef()->getRot();
		//objDir.x = cameraPitch;
		//RE::get()->updateCamera(objPos, Rot_t());
		RE::get()->getCamera()->update(objPos, objDir, cameraPitch);
		showStatus();
	}
	return false;
}

void PlayerCObj::deserialize(char* newState) {
	PlayerState *state = (PlayerState*)newState;
	this->health = state->health;
	this->getRenderModel()->setModelState(state->animationstate);
	//rm->getFrameOfRef()->setPos(state->pos);
	//rm->getFrameOfRef()->setRot(Quat_t());
	//RE::get()->getCamera()->setTargetUp(state->up);
	rm->getFrameOfRef()->deserialize(newState + sizeof(PlayerState));
}