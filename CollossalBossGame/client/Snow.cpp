#include "Snow.h"


Snow::Snow(int numParticles) : ParticleSystem()
{
	vbSize = 2048;
	vbOffset = 0;
	vbBatchSize = 512;
	for(int i= 0; i < numParticles; i++) addParticle();
}

Snow::~Snow(void)
{
}

void Snow::resetParticle(ParticleAttributes* a)
{
	a->isAlive = true;
	float minx = -632; 
	float miny = 0; 
	float minz = -362; 
	float maxx = 632; 
	float maxy = 290; 
	float maxz = 362; 
	
	D3DXVECTOR3 min = D3DXVECTOR3(minx, miny, minz);
	D3DXVECTOR3 max = D3DXVECTOR3(maxx, maxy, maxz);

	getRandVec(&(a->pos),&min, &max); 

	a->pos.y = maxy;
	a->vel.x = (getRandFloat(0,1)) * -3;
	a->vel.y = (getRandFloat(0,1)) * -10;
	a->vel.z = 0;
	a->color = D3DXCOLOR(1.0f,1.0f,0.0f,1.0f);
}

void Snow::update(float timeDelta)
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		i->pos += i->vel * timeDelta;
		//inside?
		if(i->pos.y < 0)
		{
			resetParticle(&(*i));
		}
	}
	
}
