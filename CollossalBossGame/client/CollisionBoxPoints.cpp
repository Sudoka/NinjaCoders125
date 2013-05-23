#include "CollisionBoxPoints.h"


CollisionBoxPoints::CollisionBoxPoints(void) : ParticleSystem()
{
	vbSize = 2048;
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 3.0f;
}


CollisionBoxPoints::~CollisionBoxPoints(void)
{
}

void CollisionBoxPoints::resetParticle(ParticleAttributes* a)
{
	// should be static
	a->isAlive = true;
	a->color = D3DXCOLOR(0.0f,1.0f,0.7f,1.0f);
}

void CollisionBoxPoints::update(float timeDelta)
{
	particles.clear();
}

void CollisionBoxPoints::addParticles(vector<Vec3f> pts)
{
	for(int i = 0; i < pts.size(); i++)
	{
		Vec3f pt = pts[i];
		ParticleAttributes a;
		resetParticle(&a);
		a.pos = D3DXVECTOR3(pt.x, pt.y, pt.z);
		particles.push_back(a);
	}
}

