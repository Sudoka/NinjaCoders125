/*
 * RenderModel.cpp
 * Contains the information necessary for rendering
 *
 * Author: Bryan
 */
#include "RenderModel.h"
#include "RenderEngine.h"
#include "ConfigurationManager.h"

RenderModel::RenderModel(Point_t pos, Rot_t rot, Model modelNum)
{
	//Create the reference frame
	ref = new Frame(pos, rot);
	char *filename = NULL;
	switch(modelNum) {
	case MDL_0:
		filename = CM::get()->find_config("MODEL_0");
		break;
	case MDL_1:
		filename = CM::get()->find_config("MODEL_1");
		break;
	case MDL_2:
		filename = CM::get()->find_config("MODEL_2");
		break;
	case MDL_3:
		filename = CM::get()->find_config("MODEL_3");
		break;
	case MDL_4:
		filename = CM::get()->find_config("MODEL_4");
		break;
	case MDL_5:
		filename = CM::get()->find_config("MODEL_5");
		break;
	default:
		DC::get()->print("ERROR: Model %d not known\n", modelNum);
	}

	if(filename != NULL) {
		if (!RE::get()->loadModel(filename, &modelId)) {
			DC::get()->print("Didn't load the model!\n");
		} else {
			DC::get()->print("Successfully loaded model %d\n",modelNum);
		}
	}
}

RenderModel::~RenderModel(void)
{
}

void RenderModel::render() {
	Point_t pos = ref->getPos();
	Rot_t rot = ref->getRot();

	//Get translation/rotation matrix
	D3DXMATRIX trans, rotX, rotY, rotZ;
	D3DXMatrixIdentity(&trans);
	D3DXMatrixIdentity(&rotX);
	D3DXMatrixIdentity(&rotY);
	D3DXMatrixIdentity(&rotZ);

	D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);
	D3DXMatrixRotationX(&rotX, rot.x);
	D3DXMatrixRotationY(&rotY, rot.y);
	D3DXMatrixRotationZ(&rotZ, rot.z);

	//DC::get()->print("(%f,%f,%f), (%f,%f,%f)\n", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);

	//Render
	RE::get()->animate(modelId, rotX * rotY * rotZ * trans);
}

