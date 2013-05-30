/*
 * RenderModel.h
 * Contains the information necessary for rendering
 *
 * Authors: Nathan, Haro, Suman, Bryan
 */

#pragma once

#include "Frame.h"
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")

#include <d3dx9.h>
#include "NetworkData.h"
#include <vector>


class RenderModel
{
public:
	RenderModel(Point_t pos, Quat_t rot, Model modelNum);
	virtual ~RenderModel(void);
	virtual void render();
	virtual void setModelState(int state_id);
	virtual void setAnimationFrame(int frame);
	Frame *getFrameOfRef() { return ref; }
	vector<Box> colBoxes;

	void setInvisible(bool invis) { isInvisible = invis; }

private:
	Frame *ref;	//The skeleton
	int modelId;
	int modelState;
	int prevModelState;
	bool isInvisible;
};
