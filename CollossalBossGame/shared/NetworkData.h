#pragma once

#include <string.h>
#include <Windows.h>

#define MAX_PACKET_SIZE 1000000
#define PACKET_SIZE 1024

// The type of message sent between client and server.
enum PacketTypes {
    INIT_CONNECTION = 0,
    ACTION_EVENT = 1,
	MESSAGE = 2,
	COMPLETE
};

// Commands sent from the ServerObjectManager to the ClientObjectManager.
enum CommandTypes {
    CMD_CREATE,
    CMD_UPDATE,
	CMD_DELETE,
	CMD_ACTION
};

struct Packet {
	unsigned int iteration;		// The number of times through the loop for either client or server
    unsigned int packet_type;	// Type of Packet - Initialization, Actions, Messages, etc
	unsigned int object_id;		// ID of object to be updated on either client or server.
	unsigned int packet_number;	// The packet being sent back and forth.
	CommandTypes command_type;	// Command read by the ClientObjectManager
	unsigned int data_size;		// Extra field for messages that require the size to be known (if it doesn't fill out the whole buffer).
	char packet_data[PACKET_SIZE];		// Buffer to hold the data of the Packet.

	// Copies the data of this struct into a character buffer.
    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

	// Copies the data from the character buffer into this struct.
    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }

	void clear() {
		SecureZeroMemory(this, sizeof(Packet));
	}
};


/*
 * Model info
 * These enumerations are used for specifying models
 */
enum Model {
    MDL_0,
    MDL_1,
    MDL_2,
    MDL_3,
    MDL_4,
    MDL_5,
    NUM_MDLS
};

/*
 * Object types
 * These types exist to tell teh ClientObjectManager how to create an object
 * when the server declares a new object should be created
 */
enum ObjectType {
	OBJ_GENERAL,
	OBJ_PLAYER,
	NUM_OBJS
};

/*
 * Data format structures
 * These structures are used for formatting serialized data.  No actual
 * instance of these structures should be created; they should be used by
 * casting a pointer to the appropriate place in the buffer into a pointer to
 * one of these structures; from there, memory can be accessed safely.
 * All sizes should use sizeof(format_type) instead of constant numbers, so
 * that a change in the data sent will not require modifying any code, except
 * where the new field will be written and read.
 * All structs should be padded so that they are a multiple of four bytes for
 * memory-access safety reasons.
 */

/*
 * This header provides information necessary for creating an
 * object.  For now it will be applied to all objects, in case
 * the packet containing the create message is lost
 */
struct CreateHeader {
	ObjectType type;
};

/*
 * State information for the player not encoded by the position
 */
struct PlayerState {
    Model modelNum;
};

/*
 * State information for more general objects that might not be encoded
 */
struct ObjectState {
    Model modelNum;
};
