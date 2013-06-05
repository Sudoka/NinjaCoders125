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

	ArenaWallSObj *floor, *north, *south, *east, *west, *ceiling;																			//Last is collision direction, e.g. direction objects get pushed
	floor = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_FLOOR").c_str(), MDL_FLOOR, Point_t(), UP);
	north = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_WALL").c_str(), MDL_NORTH_WALL, Point_t(0.f, height/2.f, -depth/2.f), NORTH);
	south = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_WALL").c_str(), MDL_SOUTH_WALL, Point_t(0.f, height/2.f,  depth/2.f), SOUTH);
	east = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_BACK_WALL").c_str(), MDL_EAST_WALL,  Point_t(width/2.f, height/2.f, 0.f), WEST);
	west = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_WINDOW").c_str(), MDL_WEST_WALL, Point_t(-width/2.f, height/2.f, 0.f), EAST);
	ceiling = new ArenaWallSObj(som->genId(), CM::get()->find_config_as_string("HMAP_CEILING").c_str(), MDL_CEILING, Point_t(0.f, height, 0.f), DOWN);
	//floor   = new WallSObj(som->genId(), MDL_FLOOR, Point_t(), DOWN);
	//ceiling = new WallSObj(som->genId(), MDL_CEILING, Point_t(0.f, (float)height, 0.f), UP);
	//north   = new WallSObj(som->genId(), MDL_NORTH_WALL, pos, NORTH);
	//south   = new WallSObj(som->genId(), MDL_SOUTH_WALL, Point_t(0.f, (float)height/2.f, (float)depth/2.f), SOUTH);
	//east    = new WallSObj(som->genId(), MDL_EAST_WALL, Point_t((float)width/2.f, (float)height/2.f, 0.f), EAST);
	//west    = new WallSObj(som->genId(), MDL_WEST_WALL, Point_t((float)-width/2.f, (float)height/2.f, 0.f), WEST);
	
	TestSObj *elevatorS = new TestSObj(som->genId(), MDL_ELEVATOR, Point_t(-318.5f, 2.153f,  depth/2.f + 30), Quat_t());
	TestSObj *elevatorN = new TestSObj(som->genId(), MDL_ELEVATOR, Point_t(318.5f, 2.153f,  -depth/2.f - 30), Quat_t(Vec3f(0,1,0), (float)-M_PI));

	som->add(floor);
	som->add(ceiling);
	som->add(east);
	som->add(west);
	som->add(north);
	som->add(south);
	som->add(elevatorS);
	som->add(elevatorN);
}

void addPlatforms()
{
	ServerObjectManager *som = SOM::get();

	//---------------------------------------Moving---------------------------------------------

	vector<Point_t> moving_box_placements; //make sure divisible by 5
	moving_box_placements.push_back(Point_t(600,120,110));
	moving_box_placements.push_back(Point_t(-600,120,-110));
	//moving_box_placements.push_back(Point_t(110,10,110));
	//moving_box_placements.push_back(Point_t(120,80,-110));
	//moving_box_placements.push_back(Point_t(160,90,-110));
	//moving_box_placements.push_back(Point_t(110,110,110));
	//moving_box_placements.push_back(Point_t(120,180,-110));
	//moving_box_placements.push_back(Point_t(120,140,-140));
	//moving_box_placements.push_back(Point_t(160,190,-110));
	//moving_box_placements.push_back(Point_t(10,170,-110));
	//moving_box_placements.push_back(Point_t(210,240,210));
	//moving_box_placements.push_back(Point_t(210,210,210));
	//moving_box_placements.push_back(Point_t(220,280,-210));
	//moving_box_placements.push_back(Point_t(220,240,-240));
	//moving_box_placements.push_back(Point_t(260,290,-210));
	//moving_box_placements.push_back(Point_t(360,290,-310));

	for(unsigned int i = 0; i < moving_box_placements.size(); i++)
	{
		TestSObj * platform_east = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i], Quat_t(), TEST_WEST, true);//, TEST_EAST, true);
		//platform_east->setFlag(IS_FLOATING, true);
		platform_east->setFlag(IS_STATIC, true);
		som->add(platform_east);

		//TestSObj * platform_north = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+1], Quat_t(), TEST_NORTH, true);
		//platform_north->setFlag(IS_FLOATING, true);
		//som->add(platform_north);

		//TestSObj * platform_west = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+2], Quat_t(), TEST_WEST, true);
		//platform_west->setFlag(IS_FLOATING, true);
		//som->add(platform_west);

		//TestSObj * platform_south = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+3], Quat_t(), TEST_SOUTH, true);
		//platform_south->setFlag(IS_FLOATING, true);
		//som->add(platform_south);

		//TestSObj * platform = new TestSObj(som->genId(), MDL_TEST_BOX, moving_box_placements[i+4], Quat_t(), true);
		//platform->setFlag(IS_FLOATING, true);
		//som->add(platform);

	}
	
	// ---------------------------------- Static -------------------------------------------------------------

	vector<Point_t> static_box_placements; //make sure divisible by 2
	static_box_placements.push_back(Point_t(30,150,-160));
	static_box_placements.push_back(Point_t(520,100,160));
	//static_box_placements.push_back(Point_t(560,90,-110));
	//static_box_placements.push_back(Point_t(610,110,260));
	//static_box_placements.push_back(Point_t(510,180,-250));
	//static_box_placements.push_back(Point_t(-520,240,-240));
	//static_box_placements.push_back(Point_t(-160,190,-210));
	//static_box_placements.push_back(Point_t(-10,140,-250));
	//static_box_placements.push_back(Point_t(-210,240,-110));
	//static_box_placements.push_back(Point_t(610,160,260));
	//static_box_placements.push_back(Point_t(610,160,-270));
	//static_box_placements.push_back(Point_t(-610,240,-270));
	//static_box_placements.push_back(Point_t(-560,190,-210));
	//static_box_placements.push_back(Point_t(460,190,-110));
	//static_box_placements.push_back(Point_t(-260,190,-280));
	//static_box_placements.push_back(Point_t(260,190,280));

	for(unsigned int i = 0; i < static_box_placements.size() ; i++)
	{
		TestSObj * platform = new TestSObj(som->genId(), MDL_TEST_BOX, static_box_placements[i], Quat_t(), TEST_STILL);
		platform->setFlag(IS_STATIC, true);
		som->add(platform);

		//TestSObj * crate = new TestSObj(som->genId(), MDL_TEST_CRATE, static_box_placements[i], Quat_t(), TEST_STILL);
		//som->add(crate);
	}

}

void gameInit() {
	ServerObjectManager *som = SOM::get();
	
	float xBase = 0, yBase = 10, zBase = -300;

	//This object manages the world state 
	WorldSObj *wobj = new WorldSObj(som->genId());
	som->add(wobj);
	wobj->setGravitySwitch(CM::get()->find_config_as_bool("ENABLE_GRAVITY"));

	buildRoom();
								

	//PE::get()->setGravDir(EAST);

	MonsterSObj* monster = new MonsterSObj(som->genId(), CM::get()->find_config_as_int("NUMBER_MONSTER_PARTS")); // 4
	som->add(monster);
	addPlatforms();

/*
	Point_t pos = Point_t(0, 10, 15);
	Vec3f force = Vec3f(0, 0, 0);
	BulletSObj * bul = new BulletSObj(som->genId(), MDL_TEST_BOX, pos, force, 1);
	som->add(bul);
*/
}

