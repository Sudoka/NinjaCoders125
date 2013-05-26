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
	int width = CM::get()->find_config_as_int("ROOM_WIDTH");
	int height = CM::get()->find_config_as_int("ROOM_HEIGHT");
	int depth = CM::get()->find_config_as_int("ROOM_DEPTH");

	ServerObjectManager *som = SOM::get();

	WallSObj //*floor,
			 *ceiling,
			 *north, *south,
			 *east, *west;
	ArenaWallSObj *floor;
	floor = new ArenaWallSObj(som->genId(), "../floor_hmap.bmp", MDL_FLOOR, Point_t());
	//floor   = new WallSObj(som->genId(), MDL_FLOOR, Point_t(), DOWN);
	ceiling = new WallSObj(som->genId(), MDL_CEILING, Point_t(0.f, (float)height, 0.f), UP);
	north   = new WallSObj(som->genId(), MDL_NORTH_WALL, Point_t(0.f, (float)height/2.f, (float)-depth/2.f), NORTH);
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

void addPlatforms()
{
	ServerObjectManager *som = SOM::get();
	TestSObj * platform = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(10,40,10), Quat_t());
	platform->setFlag(IS_FLOATING, true);
	som->add(platform);

	TestSObj * platform_east = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(110,10,110), Quat_t(), TEST_EAST);
	platform_east->setFlag(IS_FLOATING, true);
	som->add(platform_east);

	TestSObj * platform_north = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(120,80,-110), Quat_t(), TEST_NORTH);
	platform_north->setFlag(IS_FLOATING, true);
	som->add(platform_north);

	TestSObj * platform_west = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(120,40,-140), Quat_t(), TEST_WEST);
	platform_west->setFlag(IS_FLOATING, true);
	som->add(platform_west);

	TestSObj * platform_south = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(160,90,-110), Quat_t(), TEST_SOUTH);
	platform_south->setFlag(IS_FLOATING, true);
	som->add(platform_south);

	TestSObj * platform1 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(10,140,10), Quat_t());
	platform1->setFlag(IS_FLOATING, true);
	som->add(platform1);

	TestSObj * platformeast = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(110,110,110), Quat_t(), TEST_EAST);
	platformeast->setFlag(IS_FLOATING, true);
	som->add(platformeast);

	TestSObj * platformnorth = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(120,180,-110), Quat_t(), TEST_NORTH);
	platformnorth->setFlag(IS_FLOATING, true);
	som->add(platformnorth);

	TestSObj * platformwest = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(120,140,-140), Quat_t(), TEST_WEST);
	platformwest->setFlag(IS_FLOATING, true);
	som->add(platformwest);

	TestSObj * platformsouth = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(160,190,-110), Quat_t(), TEST_SOUTH);
	platformsouth->setFlag(IS_FLOATING, true);
	som->add(platformsouth);

	TestSObj * platform2 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(10,170,-110), Quat_t(), TEST_NORTH);
	platform2->setFlag(IS_FLOATING, true);
	som->add(platform2);

	TestSObj * platform3 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(210,240,210), Quat_t());
	platform3->setFlag(IS_FLOATING, true);
	som->add(platform3);

	TestSObj * platform_east_1 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(210,210,210), Quat_t(), TEST_EAST);
	platform_east_1->setFlag(IS_FLOATING, true);
	som->add(platform_east_1);

	TestSObj * platform_north_1 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(220,280,-210), Quat_t(), TEST_NORTH);
	platform_north_1->setFlag(IS_FLOATING, true);
	som->add(platform_north_1);

	TestSObj * platform_west_1 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(220,240,-240), Quat_t(), TEST_WEST);
	platform_west_1->setFlag(IS_FLOATING, true);
	som->add(platform_west_1);

	TestSObj * platform_south_1 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(260,290,-210), Quat_t(), TEST_SOUTH);
	platform_south_1->setFlag(IS_FLOATING, true);
	som->add(platform_south_1);

	TestSObj * platform4 = new TestSObj(som->genId(), MDL_TEST_BOX, Point_t(10,170,-110), Quat_t(), TEST_NORTH);
	platform4->setFlag(IS_FLOATING, true);
	som->add(platform4);

}

void gameInit() {
	ServerObjectManager *som = SOM::get();
	
	float xBase = 0, yBase = 10, zBase = -300;

	buildRoom();
								
	//This object manages the world state 
	WorldSObj *wobj = new WorldSObj(som->genId());
	som->add(wobj);
	wobj->setGravitySwitch(CM::get()->find_config_as_bool("ENABLE_GRAVITY"));

	//MonsterSObj* monster = new MonsterSObj(som->genId(), 2);
	MonsterSObj* monster = new MonsterSObj(som->genId(), 1); // 4
	som->add(monster);

	addPlatforms();
/*
	Point_t pos = Point_t(0, 10, 15);
	Vec3f force = Vec3f(0, 0, 0);
	BulletSObj * bul = new BulletSObj(som->genId(), MDL_TEST_BOX, pos, force, 1);
	som->add(bul);
*/
}

