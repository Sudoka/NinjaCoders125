/*
 * ClientEngine.cpp
 */
#include "ClientNetworkManager.h"
#include "ClientEngine.h"
#include "defs.h"
#include "RenderEngine.h"
#include "ClientObjectManager.h"
#include "AudioEngine.h"

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
	DC::init("clientLog.txt");
	CNM::init();
	RE::init();
	COM::init();
	AE::init();
	xctrl = new XboxController(1); // For now, we can decide later if we want to change the id
}

/*
 * Engine destructor.  Cleans the other engines.
 */
ClientEngine::~ClientEngine() {
	//Clean engines
	COM::clean();
	AE::clean();
	CNM::clean();
	DC::clean();
	RE::clean();

}


/*
 * End the game with an optional error message
 */
void ClientEngine::exit(int i) {
	DC::get()->print("Exiting with error code %d\n", i);
	isRunning = false;
	exitStatus = i;
}

/*
 * The main game loop of the engine.  Initializes and runs the game.
 */
void ClientEngine::run() {

	// Wait until we connect to a server
	while(!ClientNetworkManager::get()->isConnected()) { 
		ClientNetworkManager::get()->update(); 
	}

	while(isRunning) {
		
		//Send event information to the server
		xctrl->sendInput();
		
		// Fetch all Data From the Server 
		while(ClientNetworkManager::get()->update()) {}

		//Update game logic/physics (will be moved to the server)
		COM::get()->update();

		//Render
		RE::get()->render();

		//Update Sound Events
		AE::get()->update();
		
		//Poll events
		

		//Sleep(10);
	}
}
