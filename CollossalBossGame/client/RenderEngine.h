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
#include "ClientObject.h"
#include "XAnimator_lib.h"
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

	D3DXMATRIX getWorld() { return world; }

	void renderThis(ClientObject *obj);
	
	void moveCamera(float x, float y, float z);

	//put in loading here
	#define timeSinceLastUpdate 4

	static void animate(int id) { 	RenderEngine::xAnimator->Render(id,RenderEngine::world,timeSinceLastUpdate); }
	static bool loadModel(char * filename, int * idAddr) { 
		return RenderEngine::xAnimator->LoadXFile(filename,idAddr);
	}

private:
	void startWindow ();
	void renderInitalization();	//the stuff that can't be pulled from here
	void sceneDrawing();

	RenderEngine();
	virtual ~RenderEngine();

	//Static members
	static RenderEngine *re;
	static IXAnimator* xAnimator;
	static D3DXMATRIX world;

	float xpos, ypos, zpos;

	HWND windowHandle;	
	list<ClientObject *> lsObjs;
};
typedef RenderEngine RE;

#endif

