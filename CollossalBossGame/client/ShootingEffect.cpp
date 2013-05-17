#include "ShootingEffect.h"


ShootingEffect::ShootingEffect(void)
{
	vbSize = 2046;
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 15.5f;
	addParticle();
}


ShootingEffect::~ShootingEffect(void)
{
	particles.clear();
}

void ShootingEffect::resetParticle(ParticleAttributes* a)
{
	a->isAlive = true;
	a->pos = pos;

	/*D3DXVECTOR3 min = D3DXVECTOR3(-1.0f,-1.0f,-1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f,1.0f,1.0f);
	this->getRandVec(&a->vel, &min, &max);
	D3DXVec3Normalize(&a->vel, &a->vel);
	a->vel *= 7;  
	*/
	a->color = D3DXCOLOR(0.0f,1.0f,0.25f,1.0f);
	//a->age = 0;
	//a->lifetime = 2;
}

void ShootingEffect::update(float timeDelta)
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
	//	i->age+=timeDelta;
	//	if(i->age > i->lifetime) resetParticle(&(*i));
		resetParticle(&(*i));
	//		
	//	//i->pos += D3DXVECTOR3(0,0,0);
	}
}

void ShootingEffect::setPosition(Vec3f pos)
{
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
}