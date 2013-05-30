#pragma once

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9d.lib")
#include <d3dx9.h>
#include "defs.h"

/**
 * Camera that follows player around
 *
 * Based off of this tutorial:
 * http://sasquatchsoft.blogspot.com/2008/01/building-third-person-camera-with.html
 * Thanks sasquatch! =D
 *
 * Author(s): Suman, Haro
 */
class Camera
{
public:
	Camera();
	~Camera(void);

	//New camera update information
	void update(const Point_t &tarPos, const Quat_t &tarRot, float pitch, float distance);

	//void setViewProps(const Point_t &tarPos, const Vec3f &tarDir, const Vec3f &up);

	D3DXMATRIX* getViewMatrix() { return &viewMatrix; }

private:
	D3DXMATRIX viewMatrix;
	float viewDistance;
};

