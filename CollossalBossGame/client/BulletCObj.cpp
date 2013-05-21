#include "BulletCObj.h"
#include "ClientEngine.h"
#include "ClientObjectManager.h"
#include <math.h>
#include <Windows.h>
#include "RenderEngine.h"
#include "NetworkData.h"

BulletCObj::BulletCObj(uint id, char *serializedData) :
	ClientObject(id)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new TestObject %d\n", id);

	ObjectState *state = (ObjectState*)serializedData;

	// todo send -1
	rm = new RenderModel(Point_t(),Quat_t(), (Model)-1);
	deserialize(serializedData);
	pewPew = new ShootingEffect();
	RE::get()->addParticleEffect(pewPew);
}


BulletCObj::~BulletCObj(void)
{
	// delete xctrl;
	delete rm;
	RE::get()->removeParticleEffect(pewPew);
}

bool BulletCObj::update() {
	pewPew->setPosition(rm->getFrameOfRef()->getPos());
	pewPew->update(.33);
	return false;
}

void BulletCObj::deserialize(char* newState) {
	ObjectState *state = (ObjectState*)newState;

	if (COM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(newState + sizeof(ObjectState));

		rm->colBoxes.clear();
		for (int i=0; i<collState->totalBoxes; i++)
		{
			rm->colBoxes.push_back(collState->boxes[i]);
		}

		rm->getFrameOfRef()->deserialize(newState + sizeof(ObjectState) + sizeof(CollisionState));
	}
	else
	{
		rm->getFrameOfRef()->deserialize(newState + sizeof(ObjectState));
	}
}
