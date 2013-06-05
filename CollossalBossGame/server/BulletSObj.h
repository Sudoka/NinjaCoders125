#pragma once

#include "ServerObject.h"

class BulletSObj : public ServerObject
{
public:
	BulletSObj(uint id, Model modelNum, Point_t pos, Vec3f initialForce, int dmg, int diameter, ServerObject* shooter);
	virtual ~BulletSObj(void);

	virtual bool update();
	virtual PhysicsModel *getPhysicsModel() { return pm; }
	virtual int serialize(char * buf);
	virtual ObjectType getType() { return OBJ_BULLET; }
	virtual void onCollision(ServerObject *obj, const Vec3f &collisionNormal);
	Vec3f getNormal() { return normal; }
	ServerObject* getShooter() { return creator; }
	char serialbuffer[100];

	static int TotalShooterBullets;
	static int TotalScientistBullets;

	int health;
	int damage;
	int diameter;
private:
	PhysicsModel *pm;
	Model modelNum;
	Vec3f normal;
	Vec3f lastdirection;
	float basevelocity;
	int lifetime;
	ServerObject* creator;
};