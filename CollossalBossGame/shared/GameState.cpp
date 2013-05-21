#include "GameState.h"
#include <windows.h>
#include "assert.h"

GameState::GameState(void)
{
	this->reset();
}


GameState::~GameState(void) { }

void GameState::reset() {
	SecureZeroMemory(this, sizeof(GameState));
}

int GameState::serialize(char * buf) {
	memcpy(buf, this, sizeof(GameState));
	return sizeof(GameState);
}

void GameState::deserialize(char * buf) {
	memcpy(this, buf, sizeof(GameState));
}

void GameState::playerready(int playerid) {
	if(this->currentState != GAME_START) {
		// Ignore it if it's not the right state.
		return;
	}
	bool allready = true;
	for(int i = 0; i < totalPlayerCount; i++) {
		if(playersconnected[i] == playerid+1) {
			playersready[i] = true;
		}
		if(playersready[i] == false) {
			allready = false;
		}
	}
	if(allready) {
		this->currentState = GAME_RUNNING;
	}
}

void GameState::clientready(int playerid) {
	if(this->currentState != GAME_LOADING) {
		// Dunno where this is coming from. IGNORE
		return;
	}
	playersready[getplayerindex(playerid)] = true;

	bool allready = true;
	for(int i = 0; i < totalPlayerCount; i++) {
		if(playersready[i] == false) {
			allready = false;
		}
	}
	if(allready) {
		this->currentState = GAME_START;
	}
}

int GameState::getplayerindex(int playerid) {
	for(int i = 0; i < 4; i++) {
		if(playersconnected[i] == playerid+1) {
			return i;
		}
	}
	return -1;
}

void GameState::classselect(int playerid, bool inc, bool dec) {
	bool done = false;
	int i = (inc) ? 1 : -1;
	while(!done) {
		int playerloc = getplayerindex(playerid);
		int searchvalue = (classselection[playerloc]+i)%5;
		bool acceptablevalue = true;
		for(int j = 0; i < 4; j++) {
			if(classselection[j] == searchvalue) {
				acceptablevalue = false;
			}
		}
		if(acceptablevalue) {
			classselection[playerloc] = (classselection[playerloc]+i)%5;
		} else {
			(inc) ? i++ : i--;
		}
	}
}

#pragma region World Events

void GameState::playerconnect(int playerid) {
	for(int i = 0; i < 4; i++) {
		if(playersconnected[i] == playerid+1) {
			return;
		}
	}
	playersconnected[totalPlayerCount] = playerid+1;
	totalPlayerCount++;
}

void GameState::playerdeath(int playerid) {
	playerDeathCount++;
	assert((playerDeathCount <= totalPlayerCount) && "Implementation Error");
}

void GameState::monsterdeath() {
	monsterDeathCount++;
	if(monsterDeathCount == totalMonsterCount) {
		currentState = GAME_END;
	}
	assert((monsterDeathCount <= totalMonsterCount) && "Implementation Error");
}

void GameState::monsterspawn() {
	totalMonsterCount++;
}

#pragma endregion END World Events