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
	bool start; // Start
	bool camLock;
	bool zoom;	//right joy button
	float rightDist, forwardDist; // Left Joystick
	float rotAngle; // Right Joystick
	float rotVert, rotHoriz;
	bool d_north, d_east, d_up, d_south, d_west, d_down;
};