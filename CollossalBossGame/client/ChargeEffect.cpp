#include "ChargeEffect.h"


ChargeEffect::ChargeEffect(int numParticles)
{
//	vbSize = 2048;
	vbSize = 128;
	vbOffset = 0;
	vbBatchSize = 32;
	pointSize = 5.0f;
	this->numParticles = numParticles;
	charge = 0; // todo horrendous
	killCounter = 2;
}


ChargeEffect::~ChargeEffect(void)
{
}


void ChargeEffect::resetParticle(ParticleAttributes* a)
{
	a->isAlive = true;
	a->pos = middle;

	min = D3DXVECTOR3(-1.0f,-1.0f,-1.0f);
	max = D3DXVECTOR3(1.0f,1.0f,1.0f);
	this->getRandVec(&a->vel, &min, &max);
	D3DXVec3Normalize(&a->vel, &a->vel);
	a->vel *= 7;  
	a->color = D3DXCOLOR(this->getRandFloat(0.0f,1.0f), 
						 this->getRandFloat(0.0f,1.0f),
						 this->getRandFloat(0.0f,1.0f), 
						 1.0f);
	a->age = 0;
	a->lifetime = 2;
}

void ChargeEffect::update(float timeDelta)
{
	if (charge == 0) killCounter--;
	else killCounter = 20;
	if (killCounter < 0) particles.clear();

	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		i->age+=timeDelta;
		if(i->age > i->lifetime) resetParticle(&(*i));
			
		i->pos += i->vel * timeDelta;
	}
}

void ChargeEffect::setPosition(Vec3f pos, int charge)
{
	this->charge = charge;
	middle = D3DXVECTOR3(pos.x, pos.y, pos.z);
	
	if(charge > 0 && charge < 14)
	{
		if(particles.size() + numParticles < this->vbBatchSize)
			for(int i = 0; i < numParticles; i++) addParticle();
	}
}
