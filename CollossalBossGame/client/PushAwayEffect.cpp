#include "PushAwayEffect.h"


PushAwayEffect::PushAwayEffect(void)
{
	vbSize = 2048;
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 15.5f;
	filename = "res/spiky.jpg";
	for(int i =0; i< 64; i++) addParticle();
}


PushAwayEffect::~PushAwayEffect(void)
{
}

void PushAwayEffect::resetParticle(ParticleAttributes* a)
{
	a->isAlive = true;
	a->pos = pos;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f,-1.0f,-1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f,1.0f,1.0f);
	this->getRandVec(&a->vel, &min, &max);
	D3DXVec3Normalize(&a->vel, &a->vel);
	a->vel *= 15;//30;  
	a->color = D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f);
	a->age = 0;
	a->lifetime = 15;//10;//3;
}

void PushAwayEffect::update(float timeDelta)
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		i->age+=timeDelta;
		if(i->age > i->lifetime) resetParticle(&(*i));
			
		i->pos += i->vel * timeDelta;
	}
}

void PushAwayEffect::setPosition(Vec3f pos)
{
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
}