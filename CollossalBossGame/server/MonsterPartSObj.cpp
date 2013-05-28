#include "MonsterPartSObj.h"
#include "PlayerSObj.h"
#include "BulletSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"

MonsterPartSObj::MonsterPartSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master) : ServerObject(id)
{
	this->overlord = master;
	overlord->addPart(this);
	this->modelNum = modelNum;

	this->health = CM::get()->find_config_as_int("INIT_HEALTH");

	pm = new PhysicsModel(pos, rot, 50*CM::get()->find_config_as_float("PLAYER_MASS"));

	this->setFlag(IS_STATIC, 1);

	isFogging = false; 

	stateCounter = 0;
	attacked = false; // haven't been attacked yet
	currStateDone = true; // no states have started yet
}


MonsterPartSObj::~MonsterPartSObj(void)
{
	delete pm;
}

bool MonsterPartSObj::update() {
	stateCounter++;

	////////////////// State transitions /////////////////////
	// These should maybe be moved to the monster...
	// Only change states when our current state has gone through a whole cycle
	// This should be set by the individual state methods when their cycle is over
	// i.e. idle(), slam(), spike(), etc...
	if (currStateDone)
	{
		// we're about to start a new state =)
		stateCounter = 0;
		currStateDone = false; 

		// If you're dead, you're dead xD
		if (health <= 0) {
			health = 0;

			// If my previous state was death, I already did my fancy animation
			if (actionState == DEATH_ACTION) {
				overlord->removePart(this);
				return true; // I died!
			}
			// Otherwise, do my fancy animation before actually dying
			else
			{
				actionState = DEATH_ACTION;
			}
		}
		else
		{
			int angryProb = attacked ? 85 : 60;
		
			// we're angry!
			if ((rand() % 100) < angryProb) 
			{
				// fight or flight?
				int moveProb = 15;

				// Flight!
				if ((rand() % 100) < moveProb)
				{
					this->setFlag(IS_HARMFUL, 0);
					actionState = MOVE_ACTION;
				}
				// Fight!!
				else
				{
					this->setFlag(IS_HARMFUL, 1);

					// This sets all player info in our fields
					this->findPlayer();

					int targetAttackProb = this->playerFound ? 90 : 25;

					// targetted attack
					if ((rand() % 100) < targetAttackProb)
					{
						actionState = ATTACK_ACTION;
					}
					// non-targetted attack
					else
					{
						// randomly pick between combo attack, spike, and defense rage
						switch(rand() % 3)
						{
						case 0:		actionState = COMBO_ACTION; break;
						case 1:		actionState = SPIKE_ACTION; break;
						default:	actionState = RAGE_ACTION; break;
						}
					}
				}
			}
			// we're not attacking!
			else
			{
				this->setFlag(IS_HARMFUL, 0);

				// randomly pick between idle and probing
				if (rand() % 2) { actionState = IDLE_ACTION; }
				else { actionState = PROBE_ACTION; }
			}
		}
	}

	///////////////////// State logic ///////////////////////
	actionState = MOVE_ACTION;

	switch(actionState)
	{
	case IDLE_ACTION:
		idle();
		break;
	case PROBE_ACTION:
		probe();
		break;
	case ATTACK_ACTION:
		attack();
		break;
	case COMBO_ACTION:
		combo();
		break;
	case SPIKE_ACTION:
		spike();
		break;
	case RAGE_ACTION:
		rage();
		break;
	case MOVE_ACTION:
		move();
		break;
	case DEATH_ACTION:
		death();
		break;
	default:
		if(actionState > NUM_MONSTER_ACTIONS) DC::get()->print("ERROR: Monster state %d not known\n", actionState);
		break;
	}

	// Reset attack every update loop, onCollision re-sets it
	attacked = false;

	return false;
}

void MonsterPartSObj::move() {
	// move in 16
	// move out 18

	// Wriggle out
	if (stateCounter < 16)
	{
		modelAnimationState = M_EXIT;
	}
	// Switch positions
	else if (stateCounter == 16)
	{
		Frame* currFrame = this->getPhysicsModel()->ref;
		Frame newFrame = this->overlord->updatePosition(*currFrame);
		currFrame->setPos(newFrame.getPos());
		currFrame->setRot(newFrame.getRot());
	}
	// Wriggle back in
	else
	{
		modelAnimationState = M_ENTER;
	}

	currStateDone = (stateCounter == 33);
}

void MonsterPartSObj::death() {
	modelAnimationState = M_DEATH;

	// No collision boxes in death
	if (stateCounter == 0)
	{
		CollisionModel *cm = getCollisionModel();
		((AabbElement*)cm->get(0))->bx = Box();
		((AabbElement*)cm->get(1))->bx = Box();
		((AabbElement*)cm->get(2))->bx = Box();
	}

	currStateDone = (stateCounter == 20);
}

void MonsterPartSObj::onCollision(ServerObject *obj, const Vec3f &collisionNormal) {
	int damage = 0;

	// if I collided against the player, AND they're attacking me, loose health
	if(obj->getType() == OBJ_PLAYER)
	{	
		PlayerSObj* player = reinterpret_cast<PlayerSObj*>(obj);
		damage = player->damage;
	}

	if(obj->getType() == OBJ_BULLET) {
		BulletSObj* bullet = reinterpret_cast<BulletSObj*>(obj);
		damage = bullet->damage;
	}

	if(obj->getType() == OBJ_HARPOON) {
		// HarpoonSObj* bullet = reinterpret_cast<HarpoonSObj*>(obj);
		// damage = bullet->damage;
	}

	health -= damage;

	if(this->health < 0) health = 0;
	if(this->health > 100) health = 100; // would this ever be true? o_O

	// I have been attacked! You'll regret it in the next udpate loop player! >_>
	attacked = damage > 0;
}

int MonsterPartSObj::serialize(char * buf) {
	MonsterPartState *state = (MonsterPartState*)buf;
	state->modelNum = this->modelNum;
	state->animationState = this->modelAnimationState;
	state->fog = this->isFogging;
	state->animationFrame = -1;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(MonsterPartState));

		vector<CollisionElement*>::iterator cur = getCollisionModel()->getStart(),
			end = getCollisionModel()->getEnd();

		collState->totalBoxes = min(end - cur, maxBoxes);

		for(int i=0; i < collState->totalBoxes; i++, cur++) {
			//The collision box is relative to the object's frame-of-ref.  Get the non-relative collision box
			collState->boxes[i] = ((AabbElement*)(*cur))->bx + pm->ref->getPos();
		}

		return pm->ref->serialize(buf + sizeof(MonsterPartState) + sizeof(CollisionState)) + sizeof(MonsterPartState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(MonsterPartState)) + sizeof(MonsterPartState);
	}
}

/**
 * Finds player nearest to us and sets targetting 
 * information in fields according to its position.
 * 
 * If no player found, it sets playerFound to false.
 * attack() and combo() methods (or any method using
 * playerPos and playerAngle) are responsible for 
 * checking playerFound and, if false, setting angle and
 * pos to some default value (or some random value)
 *
 * Author: Haro
 */
void MonsterPartSObj::findPlayer()
{
	this->playerFound = false;

	// Find a player with minimum distance to me
	vector<ServerObject *> players;
	SOM::get()->findObjects(OBJ_PLAYER, &players);

	float minDist = this->targettingDist;
	float currDist;
	Vec3f difference, playerPos;

	Vec3f myPos = this->getPhysicsModel()->ref->getPos();

	for(vector<ServerObject *>::iterator it = players.begin(); it != players.end(); ++it) {
		playerPos = (*it)->getPhysicsModel()->ref->getPos();
		difference = playerPos - myPos;
		currDist = magnitude(difference);
		if (currDist < minDist) {
			minDist = currDist;
			this->playerFound = true;
			this->playerAngle = atan2(difference.x, -1*difference.y);
			this->playerPos = playerPos;
		}
	}
}