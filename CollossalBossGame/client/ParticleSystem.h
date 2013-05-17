#pragma once
// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9d.lib")
#include <d3dx9.h>
#include "defs.h"
#include <list>
#include <time.h>

struct ParticleAttributes
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 vel;
	D3DXVECTOR3 acc;
	float lifetime;
	float age;
    D3DCOLOR color;   // Color 
    D3DCOLOR colorFade;   // Color 
	bool isAlive;
};

class ParticleSystem
{
public:
	ParticleSystem();
	virtual ~ParticleSystem(void);

	virtual void reset();
	virtual void resetParticle(ParticleAttributes* a) = 0;
	virtual void addParticle();
	virtual void preRender(LPDIRECT3DDEVICE9 direct3dDevice);
	virtual void postRender(LPDIRECT3DDEVICE9 direct3dDevice);
	virtual void removeDeadParticles();
	virtual void update(float timeDelta) = 0;
	virtual void render(LPDIRECT3DDEVICE9 direct3dDevice);
	virtual void init(LPDIRECT3DDEVICE9 pDevice);
	virtual void kill();

	bool isDead();
	bool isEmpty();
	float getRandFloat(float low, float high);
	void getRandVec(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);

	float pointSize;

	IDirect3DVertexBuffer9* vb;
	IDirect3DTexture9* texture;
	list<ParticleAttributes> particles; 
	int maxParticles; 
	DWORD vbSize;
	DWORD vbOffset;
	DWORD vbBatchSize;
};

