#include "ScientistCObj.h"
#include "RenderEngine.h"


ScientistCObj::ScientistCObj(uint id, char *data) : PlayerCObj(id, data+16)
{
	this->transformdelay = *(int *)data; data += 4;
	this->transformduration = *(int *)data; data += 4;
	this->transformtargetid = *(int *)data; data += 4;
	this->transformedclass = (CharacterClass)*(int *)data; data += 4;

	be = new BeamEffect();
	RE::get()->addParticleEffect(be);
}


ScientistCObj::~ScientistCObj(void)
{
	RE::get()->removeParticleEffect(be);
}

int ScientistCObj::getTypeInt()
{
	return 2;
}

bool ScientistCObj::update()
{
	ClientObject * co = COM::get()->find(transformtargetid);
	if(co) { 
		Vec3f gravity = dirVec(COM::get()->getWorldState()->gravDir)*-1;
		Vec3f pos = co->getRenderModel()->getFrameOfRef()->getPos()  + gravity*(float)15;
		Point_t objPos = rm->getFrameOfRef()->getPos() + gravity*(float)15;
		be->setPosition(objPos, pos, transformduration / 15);
	} else {
		be->particles.clear();
	}
	be->update(.33f);

	return PlayerCObj::update();
}

void ScientistCObj::deserialize(char * newState) {
	this->transformdelay = *(int *)newState; newState += 4;
	this->transformduration = *(int *)newState; newState += 4;
	this->transformtargetid = *(int *)newState; newState += 4;
	this->transformedclass = (CharacterClass)*(int *)newState; newState += 4;
	PlayerCObj::deserialize(newState);
}