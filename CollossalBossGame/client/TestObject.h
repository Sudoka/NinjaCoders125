#pragma once
#include "ClientObject.h"
#include "Frame.h"
#include "Action.h"
#include "XboxController.h"

class TestObject : public ClientObject
{
public:
	TestObject(uint id, char *serializedData);
	virtual ~TestObject(void);

	virtual bool update();

	virtual RenderModel* getRenderModel() { return rm; }

	virtual void deserialize(char* newState);
	
	virtual RenderModel * getBox();
	//inputstatus istat;

private:
	RenderModel *rm;
	RenderModel *box;
	// XboxController *xctrl;
};

