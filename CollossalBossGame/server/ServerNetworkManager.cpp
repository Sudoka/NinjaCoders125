#include "ServerNetworkManager.h"
#include "Action.h"
#include "ServerObjectManager.h"
#include "PlayerSObj.h"
#include "CyborgSObj.h"
#include "ShooterSObj.h"
#include "ConfigurationManager.h"
#include "GameServer.h"
#include <iostream>

unsigned int ServerNetworkManager::client_id;
ServerNetworkManager * ServerNetworkManager::SNM;

/*
 *	This object handles networking for the server
 *
 *	0. Variable Initialization
 *	1. Initialize Components to make connection
 *	2. Create the socket
 *	3. Set the mode of the socket to be nonblocking
 *	4. Setup the TCP listening socket
 *	5. start listening for new clients attempting to connect
 */
ServerNetworkManager::ServerNetworkManager(void)
{
	debugFlag = CM::get()->find_config_as_bool("NETWORK_DEBUG_FLAG");

	char * PORT = CM::get()->find_config("PORT");
	printf("Listening at port %s\n", PORT);

	client_id = 0;
	iteration = 0;
	//0. Variable Initialization
	// create WSADATA object
    WSADATA wsaData;

    // our sockets for the server
    ListenSocket = INVALID_SOCKET;
    ClientSocket = INVALID_SOCKET;

    // address info for the server to listen to
    struct addrinfo *result = NULL;
    struct addrinfo hints;

	// 1. Initialize Components to make connection
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        exit(1);
    }

    // set address information
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
    hints.ai_flags = AI_PASSIVE;

	 // Resolve the server address and port
    iResult = getaddrinfo(NULL, PORT, &hints, &result);

    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        exit(1);
    }

	// 2. Create the socket
    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }

    // 3. Set the mode of the socket to be nonblocking
    u_long iMode = 1;	
	if(!CM::get()->find_config_as_bool("NETWORK_SERVER_USE_NONBLOCKING")) {
		printf("Setting Server Network to be blocking.");
		iMode = 0;
	}
    iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

    if (iResult == SOCKET_ERROR) {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }

    // 4. Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }

    // no longer need address information
    freeaddrinfo(result);

    // 5. start listening for new clients attempting to connect
    iResult = listen(ListenSocket, SOMAXCONN);

    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }
}

// Destructor - does nothing.
ServerNetworkManager::~ServerNetworkManager(void) {
	// TODO: Close all open sockets
}

SOCKET ServerNetworkManager::getSocketById(int cid) {
	std::map<unsigned int, SOCKET>::iterator iter;
    for(iter = sessions.begin(); iter != sessions.end(); iter++) {
		if(iter->first == cid) {
			return iter->second;
		}
	}
	return -1;
}

// Update loop - accepts new connections
// Collects and stores data 
void ServerNetworkManager::update() {
	// get new clients
	do {
		unsigned int temp_c_id = client_id;
		if(acceptNewClient(temp_c_id)) {
			if(debugFlag) DC::get()->print("client %d has been connected to the server\n",client_id);
			PlayerSObj * o;
			if(temp_c_id == client_id) {
				// Create a Test Server Object for them (for now)
				SOM *som = SOM::get();
				// Ok, since we should only have one object on both sides, the id's will match
				// but how do we get them matching later? maybe the server should send
				// the client the id back or something?
				o = new ShooterSObj(som->genId(), client_id);
				som->add(o);
				sessionsobjid.insert( pair<unsigned int, unsigned int>(temp_c_id, o->getId()) );
			} else {
				o = reinterpret_cast<PlayerSObj *>(SOM::get()->find(sessionsobjid.find(temp_c_id)->second));
			}

			this->getSendBuffer();	// Need to call this before each send, regardless of whether or not you have a message.
			this->sendToClient(sessions[temp_c_id], INIT_CONNECTION, o->getId(), 0);			

			if(debugFlag) DC::get()->print("client %d has been assigned client_id... Moving onto the rest of the loop.\n",client_id);
			if(temp_c_id == client_id) {
				client_id++;
			}
			GameServer::get()->event_connection(o->getId());
		}
	} while (sessions.empty());
	// Collect data from clients
    receiveFromClients();
	iteration++;
}

// Loop through each client and checks messages sent from them.
void ServerNetworkManager::receiveFromClients() {
    Packet packet;
    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;
    for(iter = sessions.begin(); iter != sessions.end(); ) {
        int data_length = receiveData(iter->first, network_data);

        while (data_length <= 0) { //no data recieved
            //continue;
			data_length = receiveData(iter->first, network_data);
			if(data_length == CUSTOM_CLIENT_CRASH_ERROR_CODE) {
				break;
			}
        }
		if(data_length == CUSTOM_CLIENT_CRASH_ERROR_CODE) {
			int cli_id = iter->first;
			iter++;
			sessions.erase(sessions.find(cli_id));
			continue;
		}
        int i = 0;
        while ((unsigned int)i < (unsigned int)data_length) {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);
			// <Log Packet>
			if(debugFlag)
				DC::get()->print(TIMESTAMP | LOGFILE, "Iteration: %d packet_type: %d object_id: %d packet_number: %d command_type: %d\n", packet.iteration, packet.packet_type, packet.object_id, packet.packet_number, packet.command_type);
			// </Log Packet>
            switch (packet.packet_type) {
				ServerObject* destObject;
                case INIT_CONNECTION:
                    if(debugFlag) DC::get()->print("server received init packet from client %d\n", iter->first);
                    break;
                case OBJECT_MANAGER:
					if(debugFlag) DC::get()->print("server received action event packet from client %d (player id %d)\n", iter->first, packet.object_id);
					destObject = SOM::get()->find(packet.object_id);

					if (destObject != NULL) {
						destObject->deserialize(packet.packet_data);
					}
                    break;
				case GAMESTATE_MANAGER:
					// GameServer::get()->recieveInput(packet.packet_data);
					break;
				case CLIENT_READY:
					// GameServer::get()->state.clientready(packet.object_id);
					break;
                default:
                    DC::get()->print("error in packet types\n");
                    break;
            }
        }
		iter++;
    }
}

/* accept new connections
	id: supposed to be a unique id associated with the client
	
	returns true if "a new" client was added.
*/
bool ServerNetworkManager::acceptNewClient(unsigned int & id) {
    // if client waiting, accept the connection and save the socket
	int socklen = sizeof(sockaddr_in);
	sockaddr_in service;
	SecureZeroMemory(&service, socklen);
    ClientSocket = accept(ListenSocket, reinterpret_cast<sockaddr *>(&service), &socklen);
    if (ClientSocket != INVALID_SOCKET) {
		// cout << (int)service.sin_addr.S_un.S_un_b.s_b1 << (int)service.sin_addr.S_un.S_un_b.s_b2 << (int)service.sin_addr.S_un.S_un_b.s_b3 << (int)service.sin_addr.S_un.S_un_b.s_b4 << endl;
		// cout << service.sin_addr.S_un.S_addr << endl;
        //disable nagle on the client's socket
        char value = 1;
        setsockopt( ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof( value ) );

		// Reconnection Logic
		if(sessionsip.count(service.sin_addr.S_un.S_addr) > 0){
			id = sessionsip.find(service.sin_addr.S_un.S_addr)->second;
			sessions.insert( pair<unsigned int, SOCKET>(id, ClientSocket) );
		} else {
			// insert new client into session id table
			sessions.insert( pair<unsigned int, SOCKET>(id, ClientSocket) );
			sessionsip.insert( pair<long, unsigned int>(service.sin_addr.S_un.S_addr, id) );
		}
        return true;
    }
    return false;
}

// receive incoming data
int ServerNetworkManager::receiveData(unsigned int c_id, char * recvbuf) {
    if( sessions.find(c_id) != sessions.end() ) {
        SOCKET currentSocket = sessions[c_id];
        iResult = NetworkServices::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);
        if (iResult == 0) {
            DC::get()->print("Connection closed\n");
            closesocket(currentSocket);
        }
		if(iResult == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if(err == 10056 || err == 10035) {
				// Do Nothing
			} else {
				DC::get()->print("Client %d dropped.\n", c_id);
				closesocket(currentSocket);
				return CUSTOM_CLIENT_CRASH_ERROR_CODE;
			}
		}
        return iResult;
    }
    return 0;
}

char* ServerNetworkManager::getSendBuffer() {
	send_buffer.clear();
	this->prepare_packet = false;
	return &(send_buffer.packet_data[0]);
}

int ServerNetworkManager::initPacketBuffer(unsigned int iteration, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size) {
	if(!prepare_packet) {
		send_buffer.iteration = iteration;
		send_buffer.packet_type = packet_type;
		send_buffer.object_id = object_id;
		send_buffer.command_type = command_type;
		send_buffer.data_size = data_size;
		send_buffer.packet_number = p_count;
		send_buffer.serialize(packet_buffer);
		prepare_packet = true;
	}
	p_count++;
	return prepare_packet;
}

#pragma region Send to Client and Send to All

int ServerNetworkManager::sendToClient(SOCKET sock_id, unsigned int packet_type, unsigned int data_size) {
	return sendToClient(sock_id, iteration, packet_type, -1, CMD_UPDATE, data_size);
}
int ServerNetworkManager::sendToClient(SOCKET sock_id, unsigned int packet_type, unsigned int object_id, unsigned int data_size) {
	return sendToClient(sock_id, iteration, packet_type, object_id, CMD_UPDATE, data_size);
}
int ServerNetworkManager::sendToClient(SOCKET sock_id, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size) {
	return sendToClient(sock_id, iteration, packet_type, object_id, command_type, data_size);
}
int ServerNetworkManager::sendToClient(SOCKET sock_id, unsigned int iteration, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size) {
	assert(data_size <= PACKET_SIZE && "Increase the PACKETSIZE in NetworkData.h");
	initPacketBuffer(iteration, packet_type, object_id, command_type, data_size);

    SOCKET client_socket = sock_id;
    int iSendResult;
	// http://bobobobo.wordpress.com/2008/11/09/resolving-winsock-error-10035-wsaewouldblock/
	// Issue seems to be that the buffer on the client is full, so you need to wait until all of the packets are processed.
	// Current fix is to keep attempting to send until it succeeds, with a max iterate count so it doesn't hang
	// NOTE: iSendResult might be less than sizeof(Packet)! Find out if this case needs to be considered.
		// Print out a debug statement at least.
	int loopcount = 0;
	int err;
    while((iSendResult = NetworkServices::sendMessage(client_socket, packet_buffer, sizeof(Packet))) == SOCKET_ERROR) {
		err = WSAGetLastError();
		/*if(loopcount > 50 ) {
			DC::get()->print("send looped %d times. Client possibly not processing packets fast enough. Error: %d\n", loopcount, err);
			// closesocket(client_socket);
			return 1; //break;
		} else */
		if(err == 10056 || err == 10035) {
			loopcount++;
		} else {
			DC::get()->print("send failed with error: %d\n", WSAGetLastError());
			return 1; // closesocket(client_socket);
			break;
		}
	}
	if(loopcount > 10) {
		DC::get()->print("send looped %d times. Client possibly not processing packets fast enough. Error: %d\n", loopcount, err);
	}
	return iSendResult;
}

void ServerNetworkManager::sendToAll(unsigned int packet_type, unsigned int data_size) {
	sendToAll(iteration, packet_type, 0, CMD_UPDATE, data_size);
}
void ServerNetworkManager::sendToAll(unsigned int packet_type, unsigned int object_id, unsigned int data_size) {
	sendToAll(iteration, packet_type, object_id, CMD_UPDATE, data_size);
}
void ServerNetworkManager::sendToAll(unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size) {
	sendToAll(iteration, packet_type, object_id, command_type, data_size);
}

// send data to all clients
void ServerNetworkManager::sendToAll(unsigned int iteration, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size) {
    SOCKET currentSocket;
    std::map<unsigned int, SOCKET>::iterator iter;
	int iSendResult;

    for (iter = sessions.begin(); iter != sessions.end(); iter++) {
        currentSocket = iter->second;

		iSendResult = sendToClient(currentSocket, iteration, packet_type, object_id, command_type, data_size);
    }
}

#pragma endregion
