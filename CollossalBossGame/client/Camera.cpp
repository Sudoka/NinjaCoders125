#include "Camera.h"
#include "DebugConsole.h"
#include "AudioEngine.h"

Camera::Camera()
{
	viewDistance = 0;	//Should be set on first update
}


Camera::~Camera(void)
{
}


void Camera::update(const Point_t &tarPos, const Quat_t &tarRot, float pitch, float distance) {
	Vec3f camRight = rotateRight(tarRot);
	Quat_t pitchRot = Quat_t(camRight, pitch);	//Rotate by pitch amount

	Vec3f camUp = rotate(rotateUp(tarRot), pitchRot),
		  camFwd = rotate(rotateFwd(tarRot), pitchRot);
	viewDistance = distance;
	

	//Correct vectors
	cross(&camRight, camUp, camFwd);
	camRight.normalize();
	cross(&camUp, camFwd, camRight);
	camUp.normalize();

	Point_t camPos = tarPos - camFwd * viewDistance;

	//Construct the view matrix
	(viewMatrix)(0, 0) = camRight.x;
	(viewMatrix)(1, 0) = camRight.y;
	(viewMatrix)(2, 0) = camRight.z;
	(viewMatrix)(3, 0) = -(camRight ^ camPos);	//D3DXVec3Dot(&camRight, &camPos);

	(viewMatrix)(0, 1) = camUp.x;
	(viewMatrix)(1, 1) = camUp.y;
	(viewMatrix)(2, 1) = camUp.z;
	(viewMatrix)(3, 1) = -(camUp ^ camPos);		//D3DXVec3Dot(&camUp, &camPos);

	(viewMatrix)(0, 2) = camFwd.x;
	(viewMatrix)(1, 2) = camFwd.y;
	(viewMatrix)(2, 2) = camFwd.z;
	(viewMatrix)(3, 2) = -(camFwd ^ camPos);	//D3DXVec3Dot(&camView, &camPos);

	(viewMatrix)(0, 3) = 0.0f;
	(viewMatrix)(1, 3) = 0.0f;
	(viewMatrix)(2, 3) = 0.0f;
	(viewMatrix)(3, 3) = 1.0f;

	//update the audio engine with new camera data
	AE::get()->setListenerPos(tarPos,camFwd,camUp);
}
