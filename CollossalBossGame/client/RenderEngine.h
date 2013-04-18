/*
* RenderEngine.h
* Defines the render engine class.
* This object is specific to the client.
*/

#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9d.lib")
#include <d3dx9.h>

#include <list>
#include <stdio.h>
#include <string>
#include "ClientObject.h"
#include "XAnimator_lib.h"
#include "Camera.h"
using namespace std;


// define the screen resolution
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

#include "RenderModel.h"

class RenderEngine {
public:
	static void init() { re = new RenderEngine(); }
	static RenderEngine *get() { return re; }
	static void clean() { delete re; }

	void render();

	LPDIRECT3D9 direct3dInterface; // the pointer to our Direct3D interface
	LPDIRECT3DDEVICE9 direct3dDevice; // the pointer to the device class
	ID3DXFont* direct3dText; // the pointer to the device class
	LPD3DXLINE healthLine;
	LPD3DXLINE backgroundLine;

	void renderThis(ClientObject *obj);
	
	Camera * getCamera() { return cam; }
	void updateCamera(const Point_t &pos, const Rot_t &rot);

	void setHUDText(string newText, int health) { hudText = newText; healthPts = health; }

	//Models
	void animate(int id, const D3DXMATRIX &pos);
	bool loadModel(const char * filename, int * idAddr);

private:
	void startWindow ();
	void renderInitalization();	//the stuff that can't be pulled from here
	void sceneDrawing();
	void drawHUD();
	void HUDInitialization();

	RenderEngine();
	virtual ~RenderEngine();

	//Static members
	static RenderEngine *re;
	static IXAnimator* xAnimator;
	D3DXMATRIX world;
	string hudText;
	int healthPts;

	HWND windowHandle;	
	list<ClientObject *> lsObjs;

	Camera* cam;

	//Configuration fields
	int cameraDist;
	int hudTopX, hudTopY;
};
typedef RenderEngine RE;

#endif

