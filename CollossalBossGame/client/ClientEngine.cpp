/*
 * ClientEngine.cpp
 */

#include "ClientEngine.h"
#include "defs.h"
#include "RenderEngine.h"
#include "ClientObjectManager.h"
#include "game.h"

//Static members
ClientEngine *ClientEngine::ce;
int ClientEngine::exitStatus;


/*
 * Engine constructor.  Initializes the other engines.
 */
ClientEngine::ClientEngine() {
	isRunning = true;
	exitStatus = 0;

	//Initialize engines
	RE::init();
	COM::init();
}

/*
 * Engine destructor.  Cleans the other engines.
 */
ClientEngine::~ClientEngine() {
	//Clean engines
	RE::clean();
	COM::clean();
}


/*
 * End the game with an optional error message
 */
void ClientEngine::exit(int i) {
	isRunning = false;
	exitStatus = i;
}

/*
 * The main game loop of the engine.  Initializes and runs the game.
 */
void ClientEngine::run() {
	//Initialize the client side game information
	gameInit();

	while(isRunning) {
		//Update game logic/physics (will be moved to the server)
		COM::get()->update();

		//Render
		RE::get()->render();
		
		//Poll events

		//Send event information to the server
	}
	gameClean();
}