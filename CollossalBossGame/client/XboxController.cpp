#include "ClientNetworkManager.h"
#include "ClientObjectManager.h"
#include "RenderEngine.h"
#include "XboxController.h"
#include "Action.h"
#include "defs.h"
#include <stdio.h>
#include <math.h>

/*
 * XBOX Controller
 * Class for handling controller input
 * Code thanks to http://www.codeproject.com/Articles/26949/Xbox-360-Controller-Input-in-C-with-XInput
 */
XboxController::XboxController(int player)
{
    // Set the Controller Number
    controllerNum = player - 1;
}

XboxController::~XboxController(void)
{
}

XINPUT_STATE XboxController::getState()
{
    // Zeroise the state
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // Get the state
    XInputGetState(controllerNum, &state);

    return state;
}

void XboxController::sendInput() {
	// Get the input and send it to the server
	inputstatus istat;
	SecureZeroMemory(&istat, sizeof(istat));
	if(isConnected()) {
		XINPUT_GAMEPAD gamepad = getState().Gamepad;

		// Map input to actions
		istat.jump =			(gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
		istat.specialPower =	false;//(gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
		istat.quit =			(gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
		istat.start =			(gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
		istat.attack =			(gamepad.bRightTrigger) != 0;
		istat.camLock =			(gamepad.bLeftTrigger != 0);
		istat.zoom =            (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
		istat.switchPhase =     (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
	
		istat.d_north = (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) 
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_Y)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_X)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_B);

		istat.d_east =  (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_B);


		istat.d_up =    (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) 
			&& (gamepad.wButtons & XINPUT_GAMEPAD_Y) 
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_X)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_B);

		istat.d_south = (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_Y)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_X)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_B);

		istat.d_west =  (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_B);

		istat.d_down =  (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
			&& (gamepad.wButtons & XINPUT_GAMEPAD_Y) 
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_X)
			&& !(gamepad.wButtons & XINPUT_GAMEPAD_B);

		// Get joystick positions
		float x = gamepad.sThumbLX,
		      y = gamepad.sThumbLY;
		float x2 = gamepad.sThumbRX,
		      y2 = gamepad.sThumbRY;

		// Set position
		if(fabs(x) > DEADZONE || fabs(y) > DEADZONE) {
			istat.rightDist = x / DIV;
			istat.forwardDist = y / DIV;
		} else {
			istat.rightDist = 0;
			istat.forwardDist = 0;
		}
		// Set rotation
		istat.rotAngle = atan2(x / DEADZONE, y / DEADZONE);
		istat.rotHoriz = 0;
		istat.rotVert = 0;

		if (fabs(x2) > DEADZONE)
		{
			istat.rotHoriz = atan(x2 / (JOY_MAX * 16));
		}
		if (fabs(y2) > DEADZONE)
		{
			istat.rotVert = atan(y2 / (JOY_MAX * 16));
		}

		/*istat.rotHoriz = 0;
		istat.rotVert = 0;
		float angle, magnitude;
		if(fabs(x2) > DEADZONE || fabs(y2) > DEADZONE) {
			istat.rotAngle = -atan2(x2 / DEADZONE, y2 / DEADZONE);
		}
		if(x2 * x2 + y2 * y2 < DEADZONE * DEADZONE) {
			angle = atan2(x2, y2);
			magnitude = (M_PI / 12) * (sqrt(x2 * x2 + y2 * y2) - DEADZONE) / DEADZONE;

			istat.rotHoriz = magnitude * cos(angle);
			istat.rotVert  = magnitude * sin(angle);
		}*/
	}
	//Send the input data, zero'd if nothing is there
	ClientNetworkManager::get()->sendData(OBJECT_MANAGER, reinterpret_cast<char*>(&istat), sizeof(inputstatus), COM::get()->player_id);
	ClientNetworkManager::get()->sendData(GAMESTATE_MANAGER, reinterpret_cast<char*>(&istat), sizeof(inputstatus), COM::get()->player_id);
}

bool XboxController::isConnected()
{
    // Zeroise the state
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // Get the state
    DWORD result = XInputGetState(controllerNum, &state);

    if(result == ERROR_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void XboxController::vibrate(int leftVal, int rightVal)
{
    // Create a Vibraton State
    XINPUT_VIBRATION Vibration;

    // Zeroise the Vibration
    ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

    // Set the Vibration Values
    Vibration.wLeftMotorSpeed = leftVal;
    Vibration.wRightMotorSpeed = rightVal;

    // Vibrate the controller
    XInputSetState(controllerNum, &Vibration);
}
