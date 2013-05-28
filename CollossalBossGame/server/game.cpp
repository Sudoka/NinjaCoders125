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

	//---------------------------------------Moving---------------------------------------------

	vector<Point_t> moving_box_placements; //make sure divisible by 5
	moving_box_placements.push_back(Point_t(10,40,10));
	moving_box_placements.push_back(Point_t(110,10,110));
	moving_box_placements.push_back(Point_t(120,80,-110));
	moving_box_placements.push_back(Point_t(160,90,-110));
	moving_box_placements.push_back(Point_t(110,110,110));
	moving_box_placements.push_back(Point_t(120,180,-110));
	moving_box_placements.push_back(Point_t(120,140,-140));
	moving_box_placements.push_back(Point_t(160,190,-110));
	moving_box_placements.push_back(Point_t(10,170,-110));
	moving_box_placements.push_back(Point_t(210,240,210));
	moving_box_placements.push_back(Point_t(210,210,210));
	moving_box_placements.push_back(Point_t(220,280,-210));
	moving_box_placements.push_back(Point_t(220,240,-240));
	moving_box_placements.push_back(Point_t(260,290,-210));
	moving_box_placements.push_back(Point_t(360,290,-310));

	for(int i = 0; i < moving_box_placements.size(); i+=5)
	{
		TestSObj * platform_east = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i], Quat_t(), TEST_EAST);
		platform_east->setFlag(IS_FLOATING, true);
		som->add(platform_east);

		TestSObj * platform_north = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+1], Quat_t(), TEST_NORTH);
		platform_north->setFlag(IS_FLOATING, true);
		som->add(platform_north);

		TestSObj * platform_west = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+2], Quat_t(), TEST_WEST);
		platform_west->setFlag(IS_FLOATING, true);
		som->add(platform_west);

		TestSObj * platform_south = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+3], Quat_t(), TEST_SOUTH);
		platform_south->setFlag(IS_FLOATING, true);
		som->add(platform_south);

		TestSObj * platform = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+4], Quat_t());
		platform->setFlag(IS_FLOATING, true);
		som->add(platform);

	}
	
	// ---------------------------------- Static -------------------------------------------------------------

	vector<Point_t> static_box_placements; //make sure divisible by 4
	static_box_placements.push_back(Point_t(10,10,10));
	static_box_placements.push_back(Point_t(510,210,210));
	static_box_placements.push_back(Point_t(420,80,-300));
	static_box_placements.push_back(Point_t(560,90,-110));
	static_box_placements.push_back(Point_t(610,110,300));
	static_box_placements.push_back(Point_t(610,180,-300));
	static_box_placements.push_back(Point_t(-520,240,-240));
	static_box_placements.push_back(Point_t(-160,190,-210));
	static_box_placements.push_back(Point_t(-10,170,-300));
	static_box_placements.push_back(Point_t(-210,240,-110));
	static_box_placements.push_back(Point_t(610,190,300));
	static_box_placements.push_back(Point_t(610,180,-300));
	static_box_placements.push_back(Point_t(-610,240,-270));
	static_box_placements.push_back(Point_t(-560,190,-210));
	static_box_placements.push_back(Point_t(460,190,-110));
	static_box_placements.push_back(Point_t(-260,190,-280));

	for(int i = 0; i < static_box_placements.size(); i++)
	{
		TestSObj * platform = new TestSObj(som->genId(), MDL_TEST_BOX, static_box_placements[i], Quat_t());
		platform->setFlag(IS_STATIC, true);
		som->add(platform);
	}
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
	MonsterSObj* monster = new MonsterSObj(som->genId(), 4); // 4
	som->add(monster);

	//addPlatforms();
/*
	Point_t pos = Point_t(0, 10, 15);
	Vec3f force = Vec3f(0, 0, 0);
	BulletSObj * bul = new BulletSObj(som->genId(), MDL_TEST_BOX, pos, force, 1);
	som->add(bul);
*/
}

