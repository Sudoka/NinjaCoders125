#include "SmokeEffect.h"


SmokeEffect::SmokeEffect(void)
{
	vbSize = 2048;
	filename = "res/spiky.jpg";
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 5.0f;
	for(int i =0; i< 512; i++) addParticle();
}


SmokeEffect::~SmokeEffect(void)
{
}

void SmokeEffect::preRender(LPDIRECT3DDEVICE9 direct3dDevice)
{
	ParticleSystem::preRender(direct3dDevice);
   	direct3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
	//direct3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
}

void SmokeEffect::postRender(LPDIRECT3DDEVICE9 direct3dDevice)
{
	ParticleSystem::postRender(direct3dDevice);
	direct3dDevice->SetRenderState( D3DRS_FOGENABLE, true);
}



void SmokeEffect::resetParticle(ParticleAttributes* a)
{
	a->isAlive = true;
	a->pos = pos;

	D3DXVECTOR3 min = D3DXVECTOR3(-10.0f,-10.0f,-10.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(10.0f,10.0f,10.0f);
	this->getRandVec(&a->vel, &min, &max);
	//D3DXVec3Normalize(&a->vel, &a->vel);
	a->vel *= 15;  
	a->color = D3DXCOLOR(0.0f,0.0f,1.0f,1.0f);
	a->age = 0;
	a->lifetime = 20;
}

void SmokeEffect::update(float timeDelta)
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		i->age+=timeDelta;
		if(i->age > i->lifetime) resetParticle(&(*i));
			
		i->pos += i->vel * timeDelta;

		//if(i->pos.y < 10) resetParticle(&(*i));
	}
}

void SmokeEffect::setPosition(Vec3f pos)
{
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
}