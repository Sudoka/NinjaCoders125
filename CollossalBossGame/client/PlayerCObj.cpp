#include "PlayerCObj.h"
#include "RenderEngine.h"
#define M_PI 3.14159 // TODO We should move this out somewhere

PlayerCObj::PlayerCObj(uint id) :
	ClientObject(id)
{
	DC::get()->print("Created new PlayerCObj %d\n", id);
	rm = new RenderModel(Point_t(300,500,0),Rot_t(0,0,M_PI), "bones_all.x");
}


PlayerCObj::~PlayerCObj(void)
{
	delete rm;
}

bool PlayerCObj::update() {
	// TODO: This should delete if the server told it to
	return false;
}

void PlayerCObj::deserialize(char* newState) {
	rm->getFrameOfRef()->deserialize(newState);
}