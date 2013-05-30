#pragma once

#include <vector>
#include <string>

using namespace std;

class GameState
{
public:
	GameState(void);
	~GameState(void);

	void reset();
	int serialize(char * buf);
	void deserialize(char * buf);

	int getplayerindex(int playerid);

	void playerconnect(int playerid);
	void playerdeath(int playerid);
	void monsterspawn();
	void monsterdeath();

	void playerready(int playerid);
	void clientready(int playerid);

	// World Statistics
	int totalMonsterCount;
	int totalPlayerCount;
	int playerDeathCount;
	int monsterDeathCount;

	int playersconnected[4];
	bool clientsready[4];
	bool playersready[4];
	int playerdeathstat[4];
	bool playersallready;
	bool clientsallready;
	int gameover;
};