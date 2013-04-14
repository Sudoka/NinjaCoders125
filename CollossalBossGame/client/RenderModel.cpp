/*
 * RenderModel.cpp
 * Contains the information necessary for rendering
 *
 * Author: Bryan
 */
#include "RenderModel.h"
#include "RenderEngine.h"



#define CUSTOMFVF (D3DFVF_XYZRHW)

struct RENDERVERTEX
{
	FLOAT x, y, z, rhw; // from the D3DFVF_XYZRHW flag
	//rhw is a perspective flag. not sure why it's a float
};

RenderModel::RenderModel(Point_t pos, Rot_t rot, const char * filename)
{
	//Create the reference frame
	ref = new Frame(pos, rot);
#if 0
	verts[0] = Point_t(-100.f, -100.f, 0.f);
	verts[1] = Point_t(100.f, -100.f, 0.0f);
	verts[2] = Point_t(0.f, 100.f, 0.0f);

	//now initialize any models we want to have first
	// create three vertices using the RENDERVERTEX struct built earlier
	RENDERVERTEX vertices[] =
	{
		{ verts[0].x, verts[0].y, verts[0].z, 1.0f, },
		{ verts[1].x, verts[1].y, verts[1].z, 1.0f, },
		{ verts[2].x, verts[2].y, verts[2].z, 1.0f, },
	};

	// create the vertex and store the pointer into vertexBuffer, which is created globally
	RE::get()->direct3dDevice->CreateVertexBuffer(
		3*sizeof(RENDERVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&vbuf,
		NULL
	);

	VOID* vertexInfo; // the void pointer

	vbuf->Lock(0, 0, (void**)&vertexInfo, 0); // lock the vertex buffer
	memcpy(vertexInfo, vertices, sizeof(vertices)); // copy the vertices to the locked buffer
	vbuf->Unlock(); // unlock the vertex buffer
#endif
	if (!RenderEngine::get()->loadModel("tiny.x", &skeletonGraphicId))
		DC::get()->print("Didn't load the model!");
}


RenderModel::~RenderModel(void)
{
}


void RenderModel::render() {
	//put in render call here
	RE::get()->animate(skeletonGraphicId, RE::get()->getViewOffset());
}

/*#include "RenderModel.h"

RenderModel::RenderModel
//1. Needs to keep track of all vertices
//2. 



RenderModel::update()
{
	
	vertices[] =
	{
		{ p0.x, p0.y, p0.z, 1.0f, },
		{ p1.x, p1.y, p1.z, 1.0f, },
		{ p2.x, p2.y, p2.z, 1.0f, },
	};
}
*/
