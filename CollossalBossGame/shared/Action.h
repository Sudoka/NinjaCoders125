#pragma once

struct controllerstatus {
	bool A, B, X, Y;
	bool UP, Down, Left, Right;
	bool Start, Back, LShoulder, RShoulder;
	bool LTrigger, RTrigger, LPress, RPress;
	float x1, y1;
	float x2, y2;
};

/**
 * Maps the input state to actions that will
 * be sent to the server.
 * Author: Haro
 */
struct inputstatus {
	bool jump; // A
	bool attack; // Right Trigger
	bool specialPower; // B
	bool quit; // Back
	float xDist, yDist; // Left Joystick
	float rotAngle; // Right Joystick
};