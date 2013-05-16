#pragma once

#include <string.h>
#include <Windows.h>
#include "defs.h"

#define MAX_PACKET_SIZE 1000000
#define PACKET_SIZE 256

// The type of message sent between client and server.
enum PacketTypes {
    INIT_CONNECTION = 0,
    OBJECT_MANAGER = 1,
    GAMESTATE_MANAGER = 2,
    RESET = 3,
    CLIENT_READY = 4,
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
    MDL_TENTACLE_1,
    MDL_TENTACLE_2,
    MDL_TENTACLE_3,
    MDL_TENTACLE_4,
    MDL_TENTACLE_5,
    MDL_FLOOR,
    MDL_CEILING,
	MDL_EAST_WALL,
	MDL_WEST_WALL,
	MDL_NORTH_WALL,
	MDL_SOUTH_WALL,
    MDL_PLAYER_1,
	MDL_PLAYER_2,
	MDL_PLAYER_3,
	MDL_PLAYER_4,
	MDL_TEST_BOX,
	MDL_TEST_PYRAMID,
	MDL_TEST_PLANE,
	MDL_TEST_BALL,
    NUM_MDLS
};

/*
 * Object types
 * These types exist to tell teh ClientObjectManager how to create an object
 * when the server declares a new object should be created
 */
enum ObjectType {
	OBJ_GENERAL,
	OBJ_WORLD,
	OBJ_PLAYER,
	OBJ_MONSTER,
	OBJ_TENTACLE,
	OBJ_RAGE,
	OBJ_BULLET,
	NUM_OBJS
};

/*
 * Character Classes
 * These tell the client which object to create.
 */
enum CharacterClass {
	CHAR_CLASS_GENERAL,
	CHAR_CLASS_CYBORG,
	CHAR_CLASS_SHOOTER,
	CHAR_CLASS_SCIENTIST,
	CHAR_CLASS_MECHANIC 
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
	CharacterClass cc;
};

/*
 * State information for the player not encoded by the position
 */
struct PlayerState {
    Model modelNum;
	int health;
	int ready;
	int charge;
	int animationstate;
	Quat_t camRot;
};

/*
 * Stores information on the collision boxes that need to be 
 * rendered on the client for testing
 */
const int maxBoxes = 5;
struct CollisionState {
	int totalBoxes; // so we know how many to actually draw
	Box boxes[maxBoxes]; // to keep it simple, you can have up to 5 collision boxes
};

/*
 * State information for more general objects that might not be encoded
 */
struct ObjectState {
    Model modelNum;
};

/*
 * State information for the WorldObject
 */
struct WorldState {
	DIRECTION gravDir;
};

/*
 * State information for the monster not encoded by the position
 */
struct MonsterState {
	//Model modelNum;
	int health;
};

/*
 * State information for the tentacle not encoded by the position
 */
struct TentacleState {
	Model modelNum;
	int animationState;
	bool fog;
};

enum TentacleActionState {
	T_IDLE,
	T_SLAM,
	T_SPIKE,
	T_PROBE,
	T_SHOOT,
	T_RAGE,
	NUM_T
};

/*
 * Types of player animation states
 */
enum PlayerAnimationState {
	IDLE = 0,
	WALK = 1,
	JUMP = 2,
	ATK  = 3,
	DEAD = 4
};

