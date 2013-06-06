#include "ScientistCObj.h"
#include "RenderEngine.h"


ScientistCObj::ScientistCObj(uint id, char *data) : PlayerCObj(id, data+16)
{
	this->transformdelay = *(int *)data; data += 4;
	this->transformduration = *(int *)data; data += 4;
	this->transformtargetid = *(int *)data; data += 4;
	this->transformedclass = (CharacterClass)*(int *)data; data += 4;
	this->transformedclassprevious = this->transformedclass;

	be = new BeamEffect();
	RE::get()->addParticleEffect(be);

	rm1 = new RenderModel(Point_t(),Quat_t(), MDL_PLAYER_1_1);
	rm2 = new RenderModel(Point_t(),Quat_t(), MDL_PLAYER_2_1);
	rm3 = new RenderModel(Point_t(),Quat_t(), MDL_PLAYER_3_1);
	rm4 = rm;
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
	if(this->transformedclass != this->transformedclassprevious) {
		switch(transformedclass) {
			case CHAR_CLASS_CYBORG: rm = rm1; break;
			case CHAR_CLASS_SHOOTER: rm = rm2; break;
			case CHAR_CLASS_MECHANIC: rm = rm3; break;
			case CHAR_CLASS_SCIENTIST: rm = rm4; break;
		}
		
		this->transformedclassprevious = this->transformedclass;
	}

	ClientObject * co = COM::get()->find(transformtargetid);
	if(co) { 
		Vec3f gravity = dirVec(COM::get()->getWorldState()->gravDir)*-1;
		Vec3f pos = co->getRenderModel()->getFrameOfRef()->getPos()  + gravity*(float)15;
		if(!(bStates & PLAYER_ZOOM))
		{
			Point_t objPos = rm->getFrameOfRef()->getPos() + gravity*(float)15;
			be->setPosition(objPos, pos, transformduration / 25);
		}
		else be->particles.clear();
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