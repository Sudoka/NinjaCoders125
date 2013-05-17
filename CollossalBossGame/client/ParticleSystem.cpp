#include "ParticleSystem.h"
#include <assert.h>
#include <string.h>

struct Particle
{
    D3DXVECTOR3 pt; // Position in 3d space
    D3DCOLOR color;   // Color 
	static const DWORD FVF; // i dunno
	//enum FVF
	//{
	//	FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE
	//};
};



const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;


ParticleSystem::ParticleSystem()
{
	this->pointSize = 10.0f;
	srand (time(NULL));
}

void ParticleSystem::init(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->CreateVertexBuffer(vbSize * sizeof(Particle),
								D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
								Particle::FVF, 
								D3DPOOL_DEFAULT,
								&vb,
								0);
	D3DXCreateTextureFromFile(pDevice,   //Direct3D Device
                             "res/particle.bmp",       //File Name
	                          &texture);    //Texture handle
					
}

ParticleSystem::~ParticleSystem(void)
{
	texture->Release();
	vb->Release();
}

void ParticleSystem::reset()
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		resetParticle(&(*i));
	}
}

void ParticleSystem::kill()
{
	particles.clear();
}

void ParticleSystem::addParticle()
{
	ParticleAttributes a;
	resetParticle(&a);
	particles.push_back(a);
}

void ParticleSystem::preRender(LPDIRECT3DDEVICE9 direct3dDevice)
{
	float minPointSize = 10.0f;
//	float maxPointSize = 5.0f;
    direct3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
    direct3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);
    direct3dDevice->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&pointSize));
    direct3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, *((DWORD*)&minPointSize));
  //  direct3dDevice->SetRenderState( D3DRS_POINTSIZE_MAX, *((DWORD*)&maxPointSize));

	float a = 0.0f;
	float b = 0.0f;
	float c = 1.0f;

    direct3dDevice->SetRenderState( D3DRS_POINTSCALE_A, *((DWORD*)&a));
    direct3dDevice->SetRenderState( D3DRS_POINTSCALE_B, *((DWORD*)&b));
    direct3dDevice->SetRenderState( D3DRS_POINTSCALE_C, *((DWORD*)&c));

  //  direct3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  //  direct3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
  //  direct3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  //  direct3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    direct3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true);
 //   direct3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE);
 //   direct3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
 //   direct3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  //  direct3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD);
 //  direct3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
 //   direct3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO);
    direct3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);

	//direct3dDevice->SetRenderState( D3DRS_SRCBLEND, 0);
    //direct3dDevice->SetRenderState( D3DRS_DESTBLEND, 1);

    direct3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    direct3dDevice->SetRenderState( D3DRS_LIGHTING, false);


}

void ParticleSystem::postRender(LPDIRECT3DDEVICE9 direct3dDevice)
{
    direct3dDevice->SetRenderState( D3DRS_LIGHTING, true);
    direct3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, false);
    direct3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE, false);
    direct3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false);

    direct3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

}

bool ParticleSystem::isDead()
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		if((*i).isAlive) return false;
	}

	return true;
}

bool ParticleSystem::isEmpty()
{
	return particles.empty();
}

void ParticleSystem::removeDeadParticles()
{
	list<ParticleAttributes>::iterator i;
	i = particles.begin();
	while(i != particles.end())
	{
		if(!i->isAlive) i = particles.erase(i);
		else i++;
	}
}

// the shits and giggles
void ParticleSystem::render(LPDIRECT3DDEVICE9 direct3dDevice)
{
	if(!particles.empty())
	{
		preRender(direct3dDevice);
		direct3dDevice->SetTexture(0,texture);	
		direct3dDevice->SetFVF(Particle::FVF);
		direct3dDevice->SetStreamSource(0, vb, 0, sizeof(Particle));

		if(vbOffset >= vbSize) vbOffset = 0;
		Particle* v = 0;

		vb->Lock( vbOffset * sizeof(Particle),
				  vbBatchSize * sizeof(Particle),
				  (void**)&v,
				  vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
		DWORD numParticlesInBatch = 0;

		list<ParticleAttributes>::iterator i;
		for(i = particles.begin(); i != particles.end(); i++)
		{
			if(i->isAlive)
			{
				v->pt = i->pos;
				v->color = i->color;
				v++;
				numParticlesInBatch++;
				if(numParticlesInBatch == vbBatchSize)
				{
					vb->Unlock();
					direct3dDevice->DrawPrimitive( D3DPT_POINTLIST, vbOffset, vbBatchSize);
					vbOffset += vbBatchSize;
					if(vbOffset >= vbSize) vbOffset = 0;
					vb->Lock( vbOffset * sizeof(Particle),
							  vbBatchSize * sizeof(Particle),
							  (void**)&v,
							  vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
					numParticlesInBatch = 0;
				}
			}
		}
	
		vb->Unlock();

		if(numParticlesInBatch)
		{
			direct3dDevice->DrawPrimitive( D3DPT_POINTLIST, vbOffset, numParticlesInBatch);
		}

		vbOffset += vbBatchSize;
		postRender(direct3dDevice);

	}
}

float ParticleSystem::getRandFloat(float low, float high)
{
	if(low >= high) return low;
	float f = (rand() % 10000) * 0.0001f;
	return (f * (high - low) + low);
}

void ParticleSystem::getRandVec(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	out->x = getRandFloat(min->x, max->x);
	out->y = getRandFloat(min->y, max->y);
	out->z = getRandFloat(min->z, max->z);
}