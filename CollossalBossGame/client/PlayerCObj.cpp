#include "PlayerCObj.h"
#include "RenderEngine.h"
#include "NetworkData.h"
#include "ClientObjectManager.h"
#include "ClientEngine.h"
#include "ConfigurationManager.h"
#include "defs.h"
#include <math.h>
#include <sstream>

PlayerCObj::PlayerCObj(uint id, char *data) :
	ClientObject(id)
{
	DC::get()->print("Created new PlayerCObj %d\n", id);
	rm = new RenderModel(Point_t(300,500,0),Rot_t(0,0,M_PI), MDL_0);
	cameraPitch = 0;
	health = CM::get()->find_config_as_int("INIT_HEALTH");
}

PlayerCObj::~PlayerCObj(void)
{
	delete rm;
}

void PlayerCObj::showStatus()
{
	std::stringstream status;
	status << "Player " << getId() << "\n";
	std::string s1 ("[");
	std::string s2 (floor(health/20 + 0.5f), '~');
	std::string s3 ("]");
	status << s1 << s2 << s3;
	if (health == 0) status << "\nGAME OVER";
	RE::get()->setHUDText(status.str());
}

bool PlayerCObj::update() {
	//Move the camera to follow the player
	if(COM::get()->player_id == getId()) {
		XboxController *xctrl = CE::getController();
		if(xctrl->isConnected()) {
			
			if(xctrl->getState().Gamepad.bLeftTrigger) {
				cameraPitch = 0;
			} else if(fabs((double)xctrl->getState().Gamepad.sThumbRY) > DEADZONE) {
				cameraPitch += atan(((double)xctrl->getState().Gamepad.sThumbRY / (JOY_MAX * 8)));
				if (cameraPitch > M_PI / 2) {
					cameraPitch = M_PI / 2;
				} else if(cameraPitch < -M_PI / 4) {
					cameraPitch = -M_PI / 4;
				}
			}
		}
		Point_t objPos = rm->getFrameOfRef()->getPos();
		Rot_t objDir = rm->getFrameOfRef()->getRot();
		objDir.x = cameraPitch;
		RE::get()->updateCamera(objPos, objDir);
		showStatus();
	}
	return false;
}

void PlayerCObj::deserialize(char* newState) {
	ObjectState *state = (ObjectState*)newState;
	rm->getFrameOfRef()->deserialize(newState + sizeof(ObjectState));
}