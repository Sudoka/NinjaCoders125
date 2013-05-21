#include "GameState.h"
#include <time.h> 

class GameServer
{

public:
	static void init() { gs = new GameServer(); }
	static void clean() { delete gs; }
	static GameServer * get() { return gs; }

	void run();
/*	TODO: Merge World Manager and GameServer into GameServer */
	void event_player_death(int playerid);
	void event_monster_death();
	void event_reset(int playerid);
	void event_hard_reset(int playerid);
	void event_connection(int playerid);
	void event_monster_spawn();
	void event_disconnect(int playerid);

	GameState state;

private:
	static GameServer * gs;
    GameServer(void);
    ~GameServer(void);

	void sendGameState();
	void recieveInput(char * buf);

	void gameLoop();
	void initializeSubModules();
	void cleanSubModules();

	time_t beginLoopTimer;
	time_t endLoopTimer;
	double totalLoopTime;
};