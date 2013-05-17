#include "ClientNetworkManager.h"
#include "ConfigurationManager.h"
#include "ClientObjectManager.h"
#include "TestObject.h"
#include <iostream>
#include <string>
#include <vector>
#include "Action.h"

ClientNetworkManager * ClientNetworkManager::CNM;

/*
	This object handles networking for the client

	Constructor Phases:
		0. Initial Variables
		1. Initialize Components to make connection
		2. Connect
		3. If we didn't connect, exit
		4. Set the mode of the socket to be nonblocking.
		5. disable nagle... don't know what it is, the tutorial said this was optional

*/
ClientNetworkManager::ClientNetworkManager(void) {
	connected = false;
	debugFlag = CM::get()->find_config_as_int("NETWORK_DEBUG_FLAG");

	char * HOST = CM::get()->find_config("HOST");
	char * PORT = CM::get()->find_config("PORT");

	printf("Host: %s\nPort: %s\n", HOST, PORT);
	iteration_count = 0;
	response_packet_number = -1;
	// 0. Inital Variables
    // create WSADATA object
    WSADATA wsaData;

    // the socket
    ConnectSocket = INVALID_SOCKET;

    // holds address info for socket to connect to
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

	// 1. Initialize Components to make connection

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("pause");
        exit(1);
    }

    // set address info
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!

	//resolve server address and port 
	//Address to connect to is here. Local host is good for default
    iResult = getaddrinfo(HOST, PORT, &hints, &result);

    if( iResult != 0 ) 
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
		system("pause");
        exit(1);
    }

	// 2. Connect
    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
			system("pause");
            exit(1);
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            printf("The server is down... did not connect.\n");
        }
    }

    // no longer need address info for server
    freeaddrinfo(result);

	// 3. If we didn't connect, exit
    // if connection failed
    if (ConnectSocket == INVALID_SOCKET) 
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
		system("pause");
        exit(1);
    }
	
	// 4. Set the mode of the socket to be nonblocking.
	//  if we really want the server to block everything, we'd need to replace the next
	//  10 lines of code

    u_long iMode = 1;
	if(!CM::get()->find_config_as_bool("NETWORK_CLIENT_USE_NONBLOCKING")) {
		printf("Setting Client Network to be blocking.");
		iMode = 0;
	}

    iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);

    if (iResult == SOCKET_ERROR)
    {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
		system("pause");
        exit(1);        
    }

	//5. disable nagle... You want control over when your packets are sent.
    char value = 1;
    setsockopt( ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof( value ) );
}

ClientNetworkManager::~ClientNetworkManager(void) {
	// TODO: close socket
}

bool ClientNetworkManager::isConnected()
{
	return connected;
}

int ClientNetworkManager::receivePackets(char * recvbuf) 
{
    iResult = NetworkServices::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

    if ( iResult == 0 )
    {
        DC::get()->print("Connection closed\n");
        closesocket(ConnectSocket);
        WSACleanup();
        exit(1);
    }

    return iResult;
}

bool ClientNetworkManager::update()
{
    Packet packet;
    int data_length = receivePackets(network_data);
	bool ret = true;

    while (data_length <= 0) 
    {
        //no data recieved
        //return;
		data_length = receivePackets(network_data);
    }

    int i = 0;
	//
    while ((unsigned int)i < (unsigned int)data_length) 
    {
        packet.deserialize(&(network_data[i]));
		this->response_packet_number = packet.packet_number;
        i += sizeof(Packet);
		//cout << "Iteration: " << packet.iteration << " packet_type: " << packet.packet_type << " object_id: " << packet.object_id << " packet_number: " << packet.packet_number << " command_type: " << packet.command_type << endl;
		if(debugFlag) DC::get()->print(TIMESTAMP | LOGFILE, "Iteration: %d packet_type: %d object_id: %d packet_number: %d command_type: %d\n", packet.iteration, packet.packet_type, packet.object_id, packet.packet_number, packet.command_type);
        switch (packet.packet_type) {
			case INIT_CONNECTION:
				COM::get()->player_id = packet.object_id;
				if(debugFlag) DC::get()->print("PLAYER ID RECEIVED! %d\n", COM::get()->player_id);
				connected = true;
				ret = false;
				break;
            case OBJECT_MANAGER:
                //DC::get()->print("client received action event packet from server\n");
					
				//memcpy(&(((TestObject*)COM::get()->find(0))->istat), &packet.packet_data, sizeof(inputstatus));
				//COM::get()->find(packet.object_id)->deserialize(packet.packet_data);
				if(debugFlag) DC::get()->print(CONSOLE | LOGFILE, "%s %d: Action event received\n", __FILE__, __LINE__);
				COM::get()->serverUpdate(packet.object_id, packet.command_type, packet.packet_data);
                break;
			case GAMESTATE_MANAGER:
				// ClientEngine::get()->serverUpdate(packet.packet_data);
				break;
			case RESET:
				// RESET LOGIC. Like destroying everything and resetting.
				// ClientEngine::get()->reset();
				break;
			case COMPLETE:
				if(debugFlag) DC::get()->print(CONSOLE | LOGFILE, "%s %d: Complete packet received\n", __FILE__, __LINE__);
				ret = false;
				break;
            default:
                DC::get()->print("error in packet types\n");
                break;
        }
    }
	iteration_count++;

	// keep receiving packets until we get one that says COMPLETE
	return ret; 
}

void ClientNetworkManager::sendData(PacketTypes messagetype, char * data, int datalen, int objectID) {
	// std::cout << data << std::endl;
	
    char packet_data[sizeof(Packet)];

	Packet packet;
	packet.iteration = this->iteration_count;
	packet.packet_type = messagetype;
	packet.object_id = objectID;
	packet.command_type = CMD_ACTION;
	packet.data_size = datalen;
	packet.packet_number = this->response_packet_number;
    
	memcpy(packet.packet_data, data, datalen);
    packet.serialize(packet_data);

    NetworkServices::sendMessage(ConnectSocket, packet_data, sizeof(Packet));
}