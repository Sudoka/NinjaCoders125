#pragma once
#include "ServerObject.h"
#include "Action.h"
#include "CollisionModel.h"


class PlayerSObj : public ServerObject
{
public:
	PlayerSObj(uint id, uint clientId, CharacterClass cc);
	virtual ~PlayerSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual void deserialize(char* newInput);
	virtual ObjectType getType() { return OBJ_PLAYER; }
	virtual CharacterClass getCharacterClass() { return charclass; }
	virtual void initialize();
	virtual void onCollision(ServerObject *obj, const Vec3f &collNorm);
	int getHealth() { return health; } 
	void setAnimationState(int state) { modelAnimationState = state; }
	char serialbuffer[100];

	bool attacking, newAttack;
	uint jumpCounter, attackCounter;
	int health;
	int damage;
	bool ready;
	CharacterClass charclass;
protected:
	uint clientId;
	PhysicsModel *pm;
	inputstatus istat;
	Point_t lastCollision;
	bool jumping, newJump, appliedJumpForce;
	bool charging, newCharge;
	float charge;
	// Configuration options
	float jumpDist;
	float chargeForce, chargeUpdate;
	int movDamp;

	//Rotational tracking
	float t;
	float tRate;
	//Quat_t yawRot;			//Yaw about the default up vector
	//Quat_t camYawRot;		//Camera yaw about the default up vector
	float yaw;
	float camYaw;
	float camPitch;
	Quat_t camRot;
	Quat_t initUpRot;
	Quat_t finalUpRot;
	DIRECTION lastGravDir;
	bool camLocked;
	float camKp, camKpFast, camKpSlow;

	bool firedeath;
	int gravityTimer;
	int modelAnimationState;
	int swordDamage, chargeDamage;

	void  calcUpVector(Quat_t *upRot);
	void  controlCamera(const Quat_t &upRot);
	float controlAngles(float des, float cur);

	virtual void releaseCharge() = 0;
	virtual void actionAttack() = 0;
};

