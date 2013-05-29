/*
 * game.cpp
 * All game initialization should be added here.  Basically, initial game
 * objects should be created in this file and added to the ServerObjectManager
 * appropriately.
 */
#include "defs.h"
#include "ServerObjectManager.h"

//Objects we can build
#include "TestSObj.h"
#include "WallSObj.h"
#include "BulletSObj.h"
#include "MonsterSObj.h"
#include "PhysicsEngine.h"
#include "WorldSObj.h"
#include "MonsterSObj.h"
#include "TentacleSObj.h"
#include "ConfigurationManager.h"
#include "ArenaWallSObj.h"

/*
 * buildRoom()
 * Builds our arena!
 */
void buildRoom() {
	// Get config measurements
	float width = CM::get()->find_config_as_float("ROOM_WIDTH");
	float height = CM::get()->find_config_as_float("ROOM_HEIGHT");
	float depth = CM::get()->find_config_as_float("ROOM_DEPTH");

	ServerObjectManager *som = SOM::get();

	WallSObj //*floor,
			 *ceiling,
			 //*north,
			 *south,
			 *east,
			 *west;
	Point_t pos = Point_t(0.f, height/2.f, -depth/2.f);
	ArenaWallSObj *floor, *north;
	floor = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_FLOOR").c_str(), MDL_FLOOR, Point_t(), UP);
	north = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_WALL").c_str(), MDL_NORTH_WALL, pos, NORTH);
	//floor   = new WallSObj(som->genId(), MDL_FLOOR, Point_t(), DOWN);
	ceiling = new WallSObj(som->genId(), MDL_CEILING, Point_t(0.f, (float)height, 0.f), UP);
	//north   = new WallSObj(som->genId(), MDL_NORTH_WALL, pos, NORTH);
	south   = new WallSObj(som->genId(), MDL_SOUTH_WALL, Point_t(0.f, (float)height/2.f, (float)depth/2.f), SOUTH);
	east    = new WallSObj(som->genId(), MDL_EAST_WALL, Point_t((float)width/2.f, (float)height/2.f, 0.f), EAST);
	west    = new WallSObj(som->genId(), MDL_WEST_WALL, Point_t((float)-width/2.f, (float)height/2.f, 0.f), WEST);
	
	som->add(floor);
	som->add(ceiling);
	som->add(east);
	som->add(west);
	som->add(north);
	som->add(south);
}

void gameInit() {
	ServerObjectManager *som = SOM::get();
	
	float xBase = 0, yBase = 10, zBase = -300;

	buildRoom();
								
	//This object manages the world state
	WorldSObj *wobj = new WorldSObj(som->genId());
	wobj->setGravitySwitch(false);
	som->add(wobj);

	PE::get()->setGravDir(SOUTH);

	//MonsterSObj* monster = new MonsterSObj(som->genId(), 2);
	MonsterSObj* monster = new MonsterSObj(som->genId(), 1); // 4
	som->add(monster);
/*
	Point_t pos = Point_t(0, 10, 15);
	Vec3f force = Vec3f(0, 0, 0);
	BulletSObj * bul = new BulletSObj(som->genId(), MDL_TEST_BOX, pos, force, 1);
	som->add(bul);
*/
}

