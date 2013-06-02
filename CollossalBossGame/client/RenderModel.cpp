/*
 * RenderModel.cpp
 * Contains the information necessary for rendering
 *
 * Author: Bryan
 */
#include "RenderModel.h"
#include "RenderEngine.h"
#include "ConfigurationManager.h"
#include <sstream>

RenderModel::RenderModel(Point_t pos, Quat_t rot, Model modelNum)
{
	//Create the reference frame
	ref = new Frame(pos, rot);
	char *filename = NULL;
	Vec3f scale;
	Vec3f initRot;
	isInvisible = false;
	modelState = IDLE;
	switch(modelNum) {
	case -1: 
		//container object
		isInvisible = true;
		modelId = -1;
		break;
	case MDL_TENTACLE_1:
		filename = CM::get()->find_config("MODEL_TENTACLE1");
		scale = CM::get()->find_config_as_point("MODEL_TENTACLE1_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TENTACLE1_ROTATION");
		break;
	case MDL_TENTACLE_2:
		filename = CM::get()->find_config("MODEL_TENTACLE2");
		scale = CM::get()->find_config_as_point("MODEL_TENTACLE2_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TENTACLE2_ROTATION");
		break;
	case MDL_TENTACLE_3:
		filename = CM::get()->find_config("MODEL_TENTACLE3");
		scale = CM::get()->find_config_as_point("MODEL_TENTACLE3_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TENTACLE3_ROTATION");
		break;
	case MDL_TENTACLE_4:
		filename = CM::get()->find_config("MODEL_TENTACLE4");
		scale = CM::get()->find_config_as_point("MODEL_TENTACLE4_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TENTACLE4_ROTATION");
		break;
	case MDL_TENTACLE_5:
		filename = CM::get()->find_config("MODEL_TENTACLE5");
		scale = CM::get()->find_config_as_point("MODEL_TENTACLE5_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TENTACLE5_ROTATION");
		break;
	case MDL_HEAD_1:
		filename = CM::get()->find_config("MODEL_HEAD1");
		scale = CM::get()->find_config_as_point("MODEL_HEAD1_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_HEAD1_ROTATION");
		break;
	case MDL_HEAD_2:
		filename = CM::get()->find_config("MODEL_HEAD2");
		scale = CM::get()->find_config_as_point("MODEL_HEAD2_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_HEAD2_ROTATION");
		break;
	case MDL_HEAD_3:
		filename = CM::get()->find_config("MODEL_HEAD3");
		scale = CM::get()->find_config_as_point("MODEL_HEAD3_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_HEAD3_ROTATION");
		break;
	case MDL_HEAD_4:
		filename = CM::get()->find_config("MODEL_HEAD4");
		scale = CM::get()->find_config_as_point("MODEL_HEAD4_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_HEAD4_ROTATION");
		break;
	case MDL_HEAD_5:
		filename = CM::get()->find_config("MODEL_HEAD5");
		scale = CM::get()->find_config_as_point("MODEL_HEAD5_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_HEAD5_ROTATION");
		break;
	case MDL_FLOOR:
		filename = CM::get()->find_config("MODEL_FLOOR");
		scale = CM::get()->find_config_as_point("MODEL_FLOOR_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_FLOOR_ROTATION");
		break;
	case MDL_CEILING:
		filename = CM::get()->find_config("MODEL_CEILING");
		scale = CM::get()->find_config_as_point("MODEL_CEILING_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_CEILING_ROTATION");
		break;
	case MDL_EAST_WALL:
		filename = CM::get()->find_config("MODEL_EAST_WALL");
		scale = CM::get()->find_config_as_point("MODEL_EAST_WALL_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_EAST_WALL_ROTATION");
		break;
	case MDL_WEST_WALL:
		filename = CM::get()->find_config("MODEL_WEST_WALL");
		scale = CM::get()->find_config_as_point("MODEL_WEST_WALL_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_WEST_WALL_ROTATION");
		break;
	case MDL_NORTH_WALL:
		filename = CM::get()->find_config("MODEL_NORTH_WALL");
		scale = CM::get()->find_config_as_point("MODEL_NORTH_WALL_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_NORTH_WALL_ROTATION");
		break;
	case MDL_SOUTH_WALL:
		filename = CM::get()->find_config("MODEL_SOUTH_WALL");
		scale = CM::get()->find_config_as_point("MODEL_SOUTH_WALL_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_SOUTH_WALL_ROTATION");
		break;
	case MDL_ELEVATOR:
		filename = CM::get()->find_config("MODEL_ELEVATOR");
		scale = CM::get()->find_config_as_point("MODEL_ELEVATOR_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_ELEVATOR_ROTATION");
		break;
	case MDL_PLAYER_1:
		filename = CM::get()->find_config("MODEL_PLAYER1");
		scale = CM::get()->find_config_as_point("MODEL_PLAYER1_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_PLAYER1_ROTATION");
		break;
	case MDL_PLAYER_2:
		filename = CM::get()->find_config("MODEL_PLAYER2");
		scale = CM::get()->find_config_as_point("MODEL_PLAYER2_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_PLAYER2_ROTATION");
		break;
	case MDL_PLAYER_3:
		filename = CM::get()->find_config("MODEL_PLAYER3");
		scale = CM::get()->find_config_as_point("MODEL_PLAYER3_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_PLAYER3_ROTATION");
		break;
	case MDL_PLAYER_4:
		filename = CM::get()->find_config("MODEL_PLAYER4");
		scale = CM::get()->find_config_as_point("MODEL_PLAYER4_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_PLAYER4_ROTATION");
		break;
	case MDL_TEST_BOX:
		filename = CM::get()->find_config("MODEL_TEST_BOX");
		scale = CM::get()->find_config_as_point("MODEL_TEST_BOX_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TEST_BOX_ROTATION");
		break;
	case MDL_TEST_CRATE:
		filename = CM::get()->find_config("MODEL_TEST_CRATE");
		scale = CM::get()->find_config_as_point("MODEL_TEST_CRATE_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TEST_CRATE_ROTATION");
		break;
	case MDL_TEST_PYRAMID:
		filename = CM::get()->find_config("MODEL_TEST_PYRAMID");
		scale = CM::get()->find_config_as_point("MODEL_TEST_PYRAMID_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TEST_PYRAMID_ROTATION");
		break;
	case MDL_TEST_PLANE:
		filename = CM::get()->find_config("MODEL_TEST_PLANE");
		scale = CM::get()->find_config_as_point("MODEL_TEST_PLANE_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TEST_PLANE_ROTATION");
		break;
	case MDL_TEST_BALL:
		filename = CM::get()->find_config("MODEL_TEST_BALL");
		scale = CM::get()->find_config_as_point("MODEL_TEST_BALL_SCALE");
		initRot = CM::get()->find_config_as_point("MODEL_TEST_BALL_ROTATION");
		break;
	default:
		if(modelNum > NUM_MDLS) DC::get()->print("ERROR: Model %d not known\n", modelNum);
	}

	if(filename != NULL && !isInvisible) {
		//Get initial rotation matrix
		D3DXMATRIX  rotX, rotY, rotZ, scaleMat;
		D3DXMatrixIdentity(&rotX);
		D3DXMatrixIdentity(&rotY);
		D3DXMatrixIdentity(&rotZ);

		D3DXMatrixRotationX(&rotX, initRot.x);
		D3DXMatrixRotationY(&rotY, initRot.y);
		D3DXMatrixRotationZ(&rotZ, initRot.z);

		D3DXMatrixScaling(&scaleMat,scale.x,scale.y,scale.z); 

		if (HRESULT hr = !RE::get()->loadModel(filename, &modelId, scaleMat * rotX * rotY * rotZ)) {
			DC::get()->print("Didn't load the model!\n");
		} else {
			if (RE::get()->debugFlag) DC::get()->print("Successfully loaded model %d\n",modelNum);
		}
	}
	prevModelState = -1;
}

RenderModel::~RenderModel(void)
{
}

void RenderModel::render() {
	//Don't render if invisible
	if(isInvisible) return;

	// negative id are for invisible/container objects
	if(modelId >= 0)
	{
		Point_t pos = ref->getPos();
		Quat_t rot = ref->getRot();

		//Get translation/rotation matrix
		D3DXMATRIX trans, rotMat;//rotX, rotY, rotZ;
		D3DXQUATERNION q(rot.x, rot.y, rot.z, rot.w);
		D3DXMatrixIdentity(&trans);
		D3DXMatrixIdentity(&rotMat);

		D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);
		D3DXMatrixRotationQuaternion(&rotMat, &q);

		//D3DXMatrixScaling(&scaleMat,scale.x,scale.y,scale.z);  

		//DC::get()->print("(%f,%f,%f), (%f,%f,%f)\n", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);

		//Render
		if (RE::get()->debugFlag) DC::get()->print("ANIMATION STATE %d\n", modelState);
		if(modelState != prevModelState) {
			RE::get()->getAnim()->ChangeAnimationSet(modelId, this->modelState);
			//RE::get()->getAnim()->ChangeAnimationPlaybackSpeed(modelId, 2.0);
		}
		RE::get()->animate(modelId, /*rotX * rotY * rotZ*/ rotMat * trans);
		prevModelState = modelState;
	}

	//Draw Boxes
	if(colBoxes.size() > 0)
	{
		for (unsigned int i=0; i<colBoxes.size(); i++)
		{
			Box curr = colBoxes[i];

			// calculate the box corners
			Point_t botLeftFront, botRightFront, botLeftBack, botRightBacl, topLeftFront, topRightFront, topLeftBack, topRightBacl;

			botLeftFront = Vec3f(curr.x, curr.y, curr.z);
			botRightFront = Vec3f(curr.x + curr.w, curr.y, curr.z);
			botLeftBack = Vec3f(curr.x, curr.y, curr.z + curr.l);
			botRightBacl = Vec3f(curr.x + curr.w, curr.y, curr.z + curr.l);
				
			topLeftFront = Vec3f(curr.x, curr.y + curr.h, curr.z);
			topRightFront = Vec3f(curr.x + curr.w, curr.y + curr.h, curr.z);
			topLeftBack = Vec3f(curr.x, curr.y + curr.h, curr.z + curr.l);
			topRightBacl = Vec3f(curr.x + curr.w, curr.y + curr.h, curr.z + curr.l);

			vector<Vec3f> pts;

			pts.push_back(botLeftFront);
			pts.push_back(botRightFront);
			pts.push_back(botLeftBack);
			pts.push_back(botRightBacl);

			pts.push_back(topLeftFront);
			pts.push_back(topRightFront);
			pts.push_back(topLeftBack);
			pts.push_back(topRightBacl);

			RE::get()->getColBxPts()->addParticles(pts);
		}
	}
	
}

void RenderModel::setModelState(int state_id) {
	int s = this->modelState;
	this->modelState = state_id;
	if (s != state_id)
		RE::get()->getAnim()->SetAnimationFrame(this->modelId, 0);

}

/* setAnimationFrame
 *  Frame: the absolute frame this needs to be set to
 * If the server needs this model to jump to a new part of the animation,
 *   it figures out the frame and sends that to the client.
 *   This is currently only being implemented for tentacles.
 *
 * Author: Bryan
 */
void RenderModel::setAnimationFrame(int frame) {
	RE::get()->getAnim()->SetAnimationFrame(this->modelId, frame);
}