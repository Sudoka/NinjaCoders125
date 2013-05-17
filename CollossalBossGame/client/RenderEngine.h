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
#include "HeadsUpDisplay.h"
#include "ParticleSystem.h"
#include <time.h>
#include "Snow.h"
#include "CollisionBoxPoints.h"
#include "ChargeEffect.h"

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
	void renderThis(ClientObject *obj);
	
	Camera * getCamera() { return cam; }
	CollisionBoxPoints* getColBxPts() { return colBxPts; }

	void setHUDText(string newText, int health, float charge) { hudText = newText; healthPts = health; this->charge = charge;}
	void setMonsterHUDText(string newText, int health) { monsterHUDText = newText; monsterHealthPts = health; }
	void addParticleEffect(ParticleSystem* ps) { ps->init(this->direct3dDevice); this->particleSystems.push_back(ps); }
	void removeParticleEffect(ParticleSystem* ps) { this->particleSystems.remove(ps); delete ps; }
	//Models
	void animate(int id, const D3DXMATRIX &pos);

	bool loadModel(const char * filename, int * idAddr, const D3DXMATRIX &rootMat);

	bool debugFlag;
	//Debug
	IXAnimator *getAnim() { return xAnimator; }

	bool gamestarted; // begins as false, when everyone's pressed start, then set this to true.

private:
	void startWindow ();
	void renderInitalization();	//the stuff that can't be pulled from here
	void sceneDrawing();
	void drawHUD();
	void gamestartdisplayinit();
	void gamestartdisplaylogic();

	RenderEngine();
	virtual ~RenderEngine();

	//Static members
	static RenderEngine *re;
	static IXAnimator* xAnimator;
	D3DXMATRIX world;
	string hudText;
	string monsterHUDText;
	int healthPts;
	int monsterHealthPts;
	float charge;

	HWND windowHandle;	
	list<ClientObject *> lsObjs;

	Camera* cam;
	HeadsUpDisplay* hud;
	list<ParticleSystem*> particleSystems;
	CollisionBoxPoints* colBxPts;
	//Configuration fields
	float cameraDist;
};
typedef RenderEngine RE;

#endif

