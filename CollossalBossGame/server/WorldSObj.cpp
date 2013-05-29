#include "WorldSObj.h"
#include "PhysicsEngine.h"
#include "ConfigurationManager.h"
#include "NetworkData.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

WorldSObj::WorldSObj(uint id) : ServerObject(id) {
	setFlag(IS_STATIC, true);
	setFlag(IS_PASSABLE, true);
	gravityInterval = CM::get()->find_config_as_int("GRAVITY_SWITCH_INTERVAL");
	nullInterval = CM::get()->find_config_as_int("GRAVITY_NULL_INTERVAL");
	gravityTimer = 0;
	srand(time(NULL));
	gravSwitchEnabled = true;
}


WorldSObj::~WorldSObj(void) {
}

enum GRAV_ORDER {
	GRAV_ORDER_N,
	GRAV_ORDER_E,
	GRAV_ORDER_W,
	GRAV_ORDER_S,
	GRAV_ORDER_U,
	GRAV_ORDER_D,
	GRAV_ORDER_END
};

bool WorldSObj::update() {
	if(gravSwitchEnabled) {
		DC::get()->print(CONSOLE, "Gravity timer = %d     \r", gravityTimer);

		if(gravityTimer == nullInterval) {
			PE::get()->setGravDir(NONE);
		}

		if(gravityTimer > gravityInterval) {
			gravityTimer = 0;
			int newDir = rand() % GRAV_ORDER_END;
			switch(newDir) {
			case GRAV_ORDER_N:
				PE::get()->setGravDir(NORTH);
				break;
			case GRAV_ORDER_E:
				PE::get()->setGravDir(EAST);
				break;
			case GRAV_ORDER_U:
				PE::get()->setGravDir(UP);
				break;
			case GRAV_ORDER_S:
				PE::get()->setGravDir(SOUTH);
				break;
			case GRAV_ORDER_W:
				PE::get()->setGravDir(WEST);
				break;
			default:	//case GRAV_ORDER_D:
				PE::get()->setGravDir(DOWN);
				break;
			}
		} else {
			gravityTimer++;
		}
	} else {
		if(gravityTimer == gravityInterval) {
			PE::get()->setGravDir(SOUTH);
			gravityTimer++;
		} else if(gravityTimer < gravityInterval) {
			gravityTimer++;
		}
	}

	return false;
}

int WorldSObj::serialize(char * buf) {
	WorldState *state = (WorldState*)buf;
	state->gravDir = PE::get()->getGravDir();
	return sizeof(WorldState);
}
