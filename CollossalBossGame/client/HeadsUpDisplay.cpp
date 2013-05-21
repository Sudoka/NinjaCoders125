#include "HeadsUpDisplay.h"
#include "ConfigurationManager.h"

HeadsUpDisplay::HeadsUpDisplay(LPDIRECT3DDEVICE9 direct3dDevice, bool * gs)
{
	gamestart = gs;
	hudTopX = CM::get()->find_config_as_int("HUD_TOP_X");
	hudTopY = CM::get()->find_config_as_int("HUD_TOP_Y");


	D3DXCreateSprite(direct3dDevice,&sprite);
	D3DXCreateSprite(direct3dDevice,&sprite1);
	D3DXCreateSprite(direct3dDevice,&sprite2);
	
	D3DXCreateTextureFromFile(direct3dDevice,   //Direct3D Device
                             "res/nebula.jpg",       //File Name
                             &g_texture);    //Texture handle
	D3DXCreateTextureFromFile(direct3dDevice,   //Direct3D Device
                            "res/gameover.png",       //File Name
                            &test1_texture);    //Texture handle

	D3DXCreateFont(	direct3dDevice,     //D3D Device
				    22,                       //Font height
					0,					      //Font width
					FW_NORMAL,                //Font Weight
					1,                        //MipLevels
					false,                    //Italic
					DEFAULT_CHARSET,          //CharSet
					OUT_DEFAULT_PRECIS,       //OutputPrecision
					ANTIALIASED_QUALITY,      //Quality
					DEFAULT_PITCH|FF_DONTCARE,//PitchAndFamily
					"Georgia",                //pFacename,
					&direct3dText);     //ppFont
	D3DXCreateLine(direct3dDevice, &healthLine);
	D3DXCreateLine(direct3dDevice, &monsterLine);
	D3DXCreateLine(direct3dDevice, &backgroundLine);
	D3DXCreateLine(direct3dDevice, &chargeLine);

	D3DXCreateTextureFromFile(direct3dDevice, "res/p1connect.png", &p1connecttxt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/p2connect.png", &p2connecttxt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/p3connect.png", &p3connecttxt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/p4connect.png", &p4connecttxt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/youarep1.png", &youarep1txt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/youarep2.png", &youarep2txt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/youarep3.png", &youarep3txt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/youarep4.png", &youarep4txt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/pressstart.png", &pressstarttxt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/playerready.png", &playerreadytxt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/blackbackground.png", &blackbackgroundtxt);
	D3DXCreateTextureFromFile(direct3dDevice, "res/youwin.png", &youwintxt);
	
	//D3DXCreateTextureFromFile(direct3dDevice, "res/particle.bmp", &p);

	D3DXCreateSprite(direct3dDevice,&p1connect);
	D3DXCreateSprite(direct3dDevice,&p2connect);
	D3DXCreateSprite(direct3dDevice,&p3connect);
	D3DXCreateSprite(direct3dDevice,&p4connect);
	D3DXCreateSprite(direct3dDevice,&youarep1);
	D3DXCreateSprite(direct3dDevice,&youarep2);
	D3DXCreateSprite(direct3dDevice,&youarep3);
	D3DXCreateSprite(direct3dDevice,&youarep4);
	D3DXCreateSprite(direct3dDevice,&pressstart);
	D3DXCreateSprite(direct3dDevice,&playerready);
	D3DXCreateSprite(direct3dDevice,&blackbackground);
	D3DXCreateSprite(direct3dDevice,&youwin);

	initTime = clock();
}

HeadsUpDisplay::~HeadsUpDisplay(void)
{
	sprite->Release();
	g_texture->Release();
	sprite1->Release();
	sprite2->Release();
    direct3dText->Release(); // close and release the Text
	healthLine->Release();
	backgroundLine->Release();
	monsterLine->Release();
	chargeLine->Release();
	p1connect->Release();
	p2connect->Release();
	p3connect->Release();
	p4connect->Release();
	youarep1->Release();
	youarep2->Release();
	youarep3->Release();
	youarep4->Release();
	pressstart->Release();
	playerready->Release();
	blackbackground->Release();
	youwin->Release();
	p1connecttxt->Release();
	p2connecttxt->Release();
	p3connecttxt->Release();
	p4connecttxt->Release();
	youarep1txt->Release();
	youarep2txt->Release();
	youarep3txt->Release();
	youarep4txt->Release();
	pressstarttxt->Release();
	playerreadytxt->Release();
	blackbackgroundtxt->Release();
	youwintxt->Release();
	//p->Release();
}

void HeadsUpDisplay::displayText(string hudText, string monsterHUDText)
{
	RECT font_rect;
	RECT monstr_rect;
	RECT charge_rect;

	//#define SCREEN_WIDTH 1024
    //#define SCREEN_HEIGHT 768 

	SetRect(&font_rect,
			hudTopX,
			hudTopY,
			1024,
			768);

	SetRect(&monstr_rect,
			hudTopX,
			hudTopY + 50,
			1024,
			768);

	SetRect(&charge_rect,
			hudTopX,
			hudTopY + 100,
			1024,
			768);

	sprite1->Begin(D3DXSPRITE_ALPHABLEND);
    direct3dText->DrawText(sprite1,        //pSprite
						   hudText.c_str(),	 //pString
                           -1,          //Count
                           &font_rect,  //pRect
                           DT_LEFT|DT_NOCLIP,//Format,
                           0xFFFFFFFF);//0xFF000000); //Color

    direct3dText->DrawText(sprite1,        //pSprite
						   monsterHUDText.c_str(),	 //pString
                           -1,          //Count
                           &monstr_rect,  //pRect
                           DT_LEFT|DT_NOCLIP,//Format,
                           0xFFFFFFFF);//0xFF000000); //Color

	direct3dText->DrawText(sprite1,        //pSprite
						  "Charge",	 //pString
                           -1,          //Count
                           &charge_rect,  //pRect
                           DT_LEFT|DT_NOCLIP,//Format,
                           0xFFFFFFFF);//0xFF000000); //Color

	sprite1->End();



}

void HeadsUpDisplay::displayHealthBars(int playerHealth, int monsterHealth, float charge)
{

	D3DXVECTOR2 blines[] = {D3DXVECTOR2(10.0f, 40.0f), D3DXVECTOR2(110.0f, 40.0f)};
	backgroundLine->SetWidth(15.0f);
	backgroundLine->Draw(blines, 2, D3DCOLOR_ARGB(255, 0, 0, 0));

	D3DXVECTOR2 hlines[] = {D3DXVECTOR2(10.0f, 40.0f), D3DXVECTOR2(playerHealth + 10.f , 40.0f)};
	healthLine->SetWidth(15.0f);
	healthLine->Draw(hlines, 2, D3DCOLOR_ARGB(255, (int)(255.0 * (100.0 - playerHealth) / 100.0), (int)(255.0 * playerHealth / 100.0), 0));

    blines[0] = D3DXVECTOR2(10.0f, 90.0f); blines[1] = D3DXVECTOR2(110.0f, 90.0f);
	backgroundLine->SetWidth(15.0f);
	backgroundLine->Draw(blines, 2, D3DCOLOR_ARGB(255, 0, 0, 0));

	D3DXVECTOR2 mlines[] = {D3DXVECTOR2(10.0f, 90.0f), D3DXVECTOR2(monsterHealth + 10.f , 90.0f)};
	monsterLine->SetWidth(15.0f);
	monsterLine->Draw(mlines, 2, D3DCOLOR_ARGB(255, (int)(255.0 * (100.0 - monsterHealth) / 100.0), (int)(255.0 * monsterHealth / 100.0), 0));

	blines[0] = D3DXVECTOR2(10.0f, 140.0f); blines[1] = D3DXVECTOR2(110.0f, 140.0f);
	backgroundLine->SetWidth(10.0f);
	backgroundLine->Draw(blines, 2, D3DCOLOR_ARGB(255, 0, 0, 0));

	charge = charge * 8;
	if (charge > 100) charge = 100;

	D3DXVECTOR2 clines[] = {D3DXVECTOR2(10.0f, 140.0f), D3DXVECTOR2(charge + 10.f , 140.0f)};
	chargeLine->SetWidth(10.0f);
	chargeLine->Draw(clines, 2, D3DCOLOR_ARGB(255, (int)(255.0 * (100.0 - charge) / 100.0), (int)(255.0 * charge / 100.0), (int)(charge * 2)));


	if(playerHealth == 0) displayGameOver();
	else if(monsterHealth == 0) displayVictory(); // todo Franklin fix so that we only Victory on last phase

	//D3DXVECTOR3 pos;
	//pos.x=0.0f;
	//pos.y=0.0f;
	//pos.z=0.0f;

	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
	//sprite->Draw(p,NULL,NULL,&pos,0xFFFFFFFF);
	//sprite->End();
}

#pragma region Display Methods

void HeadsUpDisplay::displayBackground()
{
	D3DXVECTOR3 pos;

	pos.x=0.0f;
	pos.y=0.0f;
	pos.z=1.0f;

	D3DXVECTOR2 spriteCentre=D3DXVECTOR2(1920.0f/2, 1920.0f/2);
	D3DXVECTOR2 trans=D3DXVECTOR2(-700.0f,-700.0f);
	float rotation=(clock()-initTime)/100000.0f;
	D3DXMATRIX mat;
	D3DXVECTOR2 scaling(1.f,1.f);
	D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&spriteCentre,rotation,&trans);
	sprite->SetTransform(&mat);

	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(g_texture,NULL,NULL,&pos,0xFFFFFFFF);
	sprite->End();

}

void HeadsUpDisplay::displayGameOver()
{
	D3DXVECTOR3 test1;

	test1.x= 0; //CM::get()->find_config_as_float("TEST1_X");
	test1.y= 100; //CM::get()->find_config_as_float("TEST1_Y");
	test1.z= 0; //CM::get()->find_config_as_float("TEST1_Z");
	
	sprite2->Begin(D3DXSPRITE_ALPHABLEND);
	sprite2->Draw(test1_texture,NULL,NULL,&test1,0xFFFFFFFF);
	sprite2->End();
}	

void HeadsUpDisplay::displayVictory()
{
	D3DXVECTOR3 test1;

	test1.x= 0; //CM::get()->find_config_as_float("TEST1_X");
	test1.y= 100; //CM::get()->find_config_as_float("TEST1_Y");
	test1.z= 0; //CM::get()->find_config_as_float("TEST1_Z");
	
	youwin->Begin(D3DXSPRITE_ALPHABLEND);
	youwin->Draw(youwintxt,NULL,NULL,&test1,0xFFFFFFFF);
	youwin->End();
}	

void displaytexture(LPD3DXSPRITE * sprite, D3DXVECTOR3 * pos, IDirect3DTexture9 ** texture) {
	(*sprite)->Begin(D3DXSPRITE_ALPHABLEND); 
	(*sprite)->Draw(*texture,NULL,NULL,pos,0xFFFFFFFF); 
	(*sprite)->End();
}

void HeadsUpDisplay::displayStart()
{
	if (!*gamestart) {
		int pid = COM::get()->player_id;
		int playercount = 0;
		int playernumber = 0;
		int ready[5]; ready[1] = 0; ready[2] = 0; ready[3] = 0; ready[4] = 0;
		vector<ClientObject *> l;
		COM::get()->findObjects(OBJ_PLAYER, &l);
		for(vector<ClientObject *>::iterator it = l.begin(); it != l.end(); ++it) {
			PlayerCObj * pc = (PlayerCObj *)(*it);
			playercount++;
			if (pc->getId() == pid) {
				playernumber = playercount;
				ready[playercount] = pc->ready;
			} else {
				ready[playercount] = pc->ready;
			}
		}
		if(playercount >= 1) {
			D3DXVECTOR3 p1c(0,0,0.25);
			D3DXVECTOR3 p1r(0,0,0);
			if (ready[1]) { displaytexture(&playerready, &p1r, &playerreadytxt); }
			(playernumber == 1) ? displaytexture(&youarep1, &p1c, &youarep1txt) : displaytexture(&p1connect, &p1c, &p1connecttxt);
		}
		if(playercount >= 2) {
			D3DXVECTOR3 p2c(300,0,0.25);
			D3DXVECTOR3 p2r(300,0,0);
			if (ready[2]) { displaytexture(&playerready, &p2r, &playerreadytxt); }
			(playernumber == 2) ? displaytexture(&youarep2, &p2c, &youarep2txt) : displaytexture(&p2connect, &p2c, &p2connecttxt);
		}
		if(playercount >= 3) {
			D3DXVECTOR3 p3c(0,300,0.25);
			D3DXVECTOR3 p3r(0,300,0);
			if (ready[3]) { displaytexture(&playerready, &p3r, &playerreadytxt); }
			(playernumber == 3) ? displaytexture(&youarep3, &p3c, &youarep3txt) : displaytexture(&p3connect, &p3c, &p3connecttxt);
		}
		if(playercount >= 4) {
			D3DXVECTOR3 p4c(300,300,0.25);
			D3DXVECTOR3 p4r(300,300,0);
			if (ready[4]) { displaytexture(&playerready, &p4r, &playerreadytxt); }
			(playernumber == 4) ? displaytexture(&youarep4, &p4c, &youarep4txt) : displaytexture(&p4connect, &p4c, &p4connecttxt);
		}
		if(!ready[playernumber]) {
			D3DXVECTOR3 rdy(300,200,0);
			displaytexture(&pressstart, &rdy, &pressstarttxt);
		}
		D3DXVECTOR3 blk(0,0,0.5);
		displaytexture(&blackbackground, &blk, &blackbackgroundtxt);

		bool allready = true;
		for(int j = 1; j < playercount+1; j++) {
			if(ready[j] != 1) {
				allready = false;
			}
		}
		if(allready) {
			*gamestart = true;
		}
	}

}

void HeadsUpDisplay::displayClassSelect() {
	D3DXVECTOR3 blk(0,0,0.5);
	displaytexture(&blackbackground, &blk, &blackbackgroundtxt);

	RECT middleofscreen;
	SetRect(&middleofscreen, hudTopX+300, hudTopY+300, 1024, 768);
	sprite1->Begin(D3DXSPRITE_ALPHABLEND);
    direct3dText->DrawText(sprite1, "CLASS SELECT SCREEN", -1, &middleofscreen, DT_LEFT|DT_NOCLIP, 0xFFFFFFFF);
	sprite1->End();
}

void HeadsUpDisplay::displaySceneSelect() {
	D3DXVECTOR3 blk(0,0,0.5);
	displaytexture(&blackbackground, &blk, &blackbackgroundtxt);

	RECT middleofscreen;
	SetRect(&middleofscreen, hudTopX+300, hudTopY+300, 1024, 768);
	sprite1->Begin(D3DXSPRITE_ALPHABLEND);
    direct3dText->DrawText(sprite1, "SCENE SELECT SCREEN", -1, &middleofscreen, DT_LEFT|DT_NOCLIP, 0xFFFFFFFF);
	sprite1->End();
}

void HeadsUpDisplay::displayLoadingScreen() {
	D3DXVECTOR3 blk(0,0,0.5);
	displaytexture(&blackbackground, &blk, &blackbackgroundtxt);

	RECT middleofscreen;
	SetRect(&middleofscreen, hudTopX+300, hudTopY+300, 1024, 768);
	sprite1->Begin(D3DXSPRITE_ALPHABLEND);
    direct3dText->DrawText(sprite1, "WE'RE LOADING", -1, &middleofscreen, DT_LEFT|DT_NOCLIP, 0xFFFFFFFF);
	sprite1->End();
}

#pragma endregion