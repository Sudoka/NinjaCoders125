#include "HeadsUpDisplay.h"
#include "ConfigurationManager.h"

HeadsUpDisplay::HeadsUpDisplay(LPDIRECT3DDEVICE9 direct3dDevice, bool * gs)
{
	gamestart = gs;
	hudTopX = CM::get()->find_config_as_int("HUD_TOP_X");
	hudTopY = CM::get()->find_config_as_int("HUD_TOP_Y");

	playerType = -1;

	D3DXCreateSprite(direct3dDevice,&sprite);
	D3DXCreateSprite(direct3dDevice,&sprite1);
	D3DXCreateSprite(direct3dDevice,&sprite2);
	D3DXCreateSprite(direct3dDevice,&monsterHealthSprite);
	D3DXCreateSprite(direct3dDevice,&playerHealthSprite);
	
	D3DXCreateTextureFromFile(direct3dDevice,   //Direct3D Device
                             "res/nebula1.jpg",       //File Name
                             &g_texture);    //Texture handle
	D3DXCreateTextureFromFile(direct3dDevice,   //Direct3D Device
                            "res/gameover.png",       //File Name
                            &test1_texture);    //Texture handle

	D3DXCreateTextureFromFile(direct3dDevice,
							"res/tentacleMenu.png",
							&monsterHealth_texture);
	D3DXCreateTextureFromFile(direct3dDevice,
							"res/playerMenu.png",
							&playerHealth_texture);
	
	D3DXCreateTextureFromFile(direct3dDevice,
							"res/cyborgHelp.png",
							&cyborgHelp_texture);
	
	D3DXCreateTextureFromFile(direct3dDevice,
							"res/mechanicHelp.png",
							&mechanicHelp_texture);
	
	D3DXCreateTextureFromFile(direct3dDevice,
							"res/scientistHelp.png",
							&scientistHelp_texture);
	
	D3DXCreateTextureFromFile(direct3dDevice,
							"res/shooterHelp.png",
							&shooterHelp_texture);


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
	D3DXCreateTextureFromFile(direct3dDevice, "res/startMenu.png", &blackbackgroundtxt);
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

void HeadsUpDisplay::establishType(void) 
{
	//get the type of object
	vector<ClientObject *> players;
	COM::get()->findObjects(OBJ_PLAYER, &players);
	int pid = COM::get()->player_id;
	for(vector<ClientObject *>::iterator it = players.begin(); it != players.end(); ++it) {
			PlayerCObj * pc = (PlayerCObj *)(*it);
			if (pc->getId() == pid) {
				this->playerType = pc->getTypeInt();
			}
	}
}

HeadsUpDisplay::~HeadsUpDisplay(void)
{
	sprite->Release();
	g_texture->Release();
	sprite1->Release();
	sprite2->Release();
	monsterHealthSprite->Release();
	playerHealthSprite->Release();
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
	RECT charge_rect;

	//#define SCREEN_WIDTH 1024
    //#define SCREEN_HEIGHT 768 

	SetRect(&charge_rect,
			hudTopX + 800 ,
			hudTopY + 130 ,
			1024,
			768);

	sprite1->Begin(D3DXSPRITE_ALPHABLEND);
	D3DXVECTOR3 test1;
	
	test1.x= 1000; //CM::get()->find_config_as_float("TEST1_X");
	test1.y= 200; //CM::get()->find_config_as_float("TEST1_Y");
	test1.z= 0; //CM::get()->find_config_as_float("TEST1_Z");

	D3DXVECTOR2 trans=D3DXVECTOR2(0.0f,0.0f);
	D3DXVECTOR2 scaling(0.8f,0.75f);
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,NULL,0.f,&trans);

	sprite1->SetTransform(&mat);
	
	switch (this->playerType) {
		case 0:
			sprite1->Draw(cyborgHelp_texture,NULL,NULL,&test1,0xFFFFFFFF);
			break;
		case 1:
			sprite1->Draw(mechanicHelp_texture,NULL,NULL,&test1,0xFFFFFFFF);
			break;
		case 2:
			sprite1->Draw(scientistHelp_texture,NULL,NULL,&test1,0xFFFFFFFF);
			break;
		case 3:
			sprite1->Draw(shooterHelp_texture,NULL,NULL,&test1,0xFFFFFFFF);
			break;
	}

	/*direct3dText->DrawText(sprite1,        //pSprite
						  "LT: Camera Track\nRT: Power\nA: Jump\nLS: Move\nRS: Camera",	 //pString
                           -1,          //Count
                           &charge_rect,  //pRect
                           DT_LEFT|DT_NOCLIP,//Format,
                           0xFFFFFFFF);//0xFF000000); //Color
	*/
	sprite1->End();
}

void HeadsUpDisplay::displayHealthBars(int playerHealth, int monsterHealth, float charge)
{
	// if(playerHealth == 0) displayGameOver();
	// else if(monsterHealth == 0) displayVictory(); // todo Franklin fix so that we only Victory on last phase
	if(1) { // Work on how the infinite business is going to work.
		//display GUI
		HeadsUpDisplay::displayMonsterHealth(monsterHealth);

		D3DXVECTOR3 test1;
	
		test1.x= 1000; //CM::get()->find_config_as_float("TEST1_X");
		test1.y= 0; //CM::get()->find_config_as_float("TEST1_Y");
		test1.z= 0; //CM::get()->find_config_as_float("TEST1_Z");

		D3DXVECTOR2 trans=D3DXVECTOR2(0.0f,0.0f);
		D3DXVECTOR2 scaling(0.8f,0.75f);
		D3DXMATRIX mat;
		D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,NULL,0.f,&trans);

		playerHealthSprite->SetTransform(&mat);
		playerHealthSprite->Begin(D3DXSPRITE_ALPHABLEND);
		playerHealthSprite->Draw(playerHealth_texture,NULL,NULL,&test1,0xFFFFFFFF);
		playerHealthSprite->End();

		float healthBarPos[] = {860.f, test1.y+20.f};
		float healthBarSize = 120.f;
		//player health background
		D3DXVECTOR2 blines[] = {D3DXVECTOR2(healthBarPos[0], healthBarPos[1]), D3DXVECTOR2(healthBarPos[0]+healthBarSize, healthBarPos[1])};
		backgroundLine->SetWidth(15.0f);
		backgroundLine->Draw(blines, 2, D3DCOLOR_ARGB(255, 0, 0, 0));

		//player health
		float max_health = (float) CM::get()->find_config_as_int("INIT_HEALTH");
		float percentage = ((float)playerHealth)/max_health;
		float inverse = 1.f - ((float)playerHealth)/max_health;
		D3DXVECTOR2 hlines[] = {D3DXVECTOR2(healthBarPos[0], healthBarPos[1]), D3DXVECTOR2(percentage*healthBarSize + healthBarPos[0], healthBarPos[1])};
		healthLine->SetWidth(15.0f);
		healthLine->Draw(hlines, 2, D3DCOLOR_ARGB(255, (int)(255.0 * inverse), (int)(255.0 * percentage), 0));

		//background for the charge
		blines[0] = D3DXVECTOR2(healthBarPos[0], healthBarPos[1]+40); blines[1] = D3DXVECTOR2(healthBarPos[0]+healthBarSize, healthBarPos[1]+40);
		backgroundLine->SetWidth(15.0f);
		backgroundLine->Draw(blines, 2, D3DCOLOR_ARGB(255, 0, 0, 0));

		charge = charge * 8;
		if (charge > 100) charge = 100;

		//charge bar
		D3DXVECTOR2 clines[] = {D3DXVECTOR2(healthBarPos[0], healthBarPos[1]+40), D3DXVECTOR2(healthBarPos[0]+charge/100.0*healthBarSize , healthBarPos[1]+40)};
		chargeLine->SetWidth(15.0f);
		chargeLine->Draw(clines, 2, D3DCOLOR_ARGB(255, (int)(0), (int)(255.0 * (charge) / 100.0), (int)(255.0 * charge / 100.0)));

	}
	
	//D3DXVECTOR3 pos;
	//pos.x=0.0f;
	//pos.y=0.0f;
	//pos.z=0.0f;

	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
	//sprite->Draw(p,NULL,NULL,&pos,0xFFFFFFFF);
	//sprite->End();
}

void HeadsUpDisplay::displayMonsterHealth(int monsterHealth) {
	D3DXVECTOR3 test1;
	
	test1.x= -50; //CM::get()->find_config_as_float("TEST1_X");
	test1.y= 0; //CM::get()->find_config_as_float("TEST1_Y");
	test1.z= 0; //CM::get()->find_config_as_float("TEST1_Z");

	monsterHealthSprite->Begin(D3DXSPRITE_ALPHABLEND);
	monsterHealthSprite->Draw(monsterHealth_texture,NULL,NULL,&test1,0xFFFFFFFF);
	monsterHealthSprite->End();

	//The background for the monster
	float monsterHeight = 33.f;
	D3DXVECTOR2 blines[] = {D3DXVECTOR2(40.0f, monsterHeight), D3DXVECTOR2(351.0f, monsterHeight)};
	backgroundLine->SetWidth(15.0f);
	backgroundLine->Draw(blines, 2, D3DCOLOR_ARGB(255, 0, 0, 0));

	//The actual health for the monster
	//we want it to map so 100% health = 200 pixels in width, and the initial health is determined by the config file
	float max_health = (float) CM::get()->find_config_as_int("INIT_HEALTH");
	float percentage = ((float)monsterHealth)/max_health;
	float inverse = 1.f-((float)monsterHealth)/max_health;
	D3DXVECTOR2 mlines[] = {D3DXVECTOR2(40.0f, monsterHeight), D3DXVECTOR2(percentage*311.f + 40.f , monsterHeight)};
	monsterLine->SetWidth(15.0f);
	monsterLine->Draw(mlines, 2, D3DCOLOR_ARGB(255, (int)(255.0 * inverse), (int)(255.0 * percentage), 0));
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

	D3DXVECTOR2 spriteCentre=D3DXVECTOR2(1920.0f/2, 1920.0f/2);

	D3DXVECTOR3 test1;
	D3DXVECTOR3 blk(0,0,0.5);
	D3DXVECTOR2 trans=D3DXVECTOR2(0.0f,0.0f);
	D3DXVECTOR2 scaling(1.f,0.75f);
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&spriteCentre,0.f,&trans);

	sprite2->SetTransform(&mat);
	sprite2->Begin(D3DXSPRITE_ALPHABLEND);
	sprite2->Draw(test1_texture,NULL,NULL,&blk,0xFFFFFFFF);
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
		this->establishType();
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
		D3DXVECTOR3 pc(0,0,0.25);
		D3DXVECTOR3 pr(0,0,0);
		D3DXVECTOR2 spriteCentre=D3DXVECTOR2(1920.0f/2, 1920.0f/2);
			
		if(playercount >= 1) { //60, 60, 0.25
			D3DXVECTOR2 trans=D3DXVECTOR2(25,30);
			D3DXVECTOR2 scaling(1.9f,1.9f);
			D3DXMATRIX mat;
			D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&spriteCentre,0.f,&trans);

			if (ready[1]) {
				playerready->SetTransform(&mat);
				playerready->Begin(D3DXSPRITE_ALPHABLEND);
				playerready->Draw(playerreadytxt,NULL,NULL,&pc,0xFFFFFFFF);
				playerready->End(); 
			} else if (playernumber == 1) {
				youarep1->SetTransform(&mat);
				youarep1->Begin(D3DXSPRITE_ALPHABLEND);
				youarep1->Draw(youarep1txt,NULL,NULL,&pc,0xFFFFFFFF);
				youarep1->End(); 
			} else {
				p1connect->SetTransform(&mat);
				p1connect->Begin(D3DXSPRITE_ALPHABLEND);
				p1connect->Draw(p1connecttxt,NULL,NULL,&pc,0xFFFFFFFF);
				p1connect->End(); 
			}
		}
		if(playercount >=	2) { //740, 30
			D3DXVECTOR2 trans=D3DXVECTOR2(747,30);
			D3DXMATRIX mat;
			D3DXVECTOR2 scaling(1.9f,1.9f);
			D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&spriteCentre,0.f,&trans);

			if (ready[2]) {
				playerready->SetTransform(&mat);
				playerready->Begin(D3DXSPRITE_ALPHABLEND);
				playerready->Draw(playerreadytxt,NULL,NULL,&pc,0xFFFFFFFF);
				playerready->End(); 
			} else if (playernumber == 2) {
				youarep2->SetTransform(&mat);
				youarep2->Begin(D3DXSPRITE_ALPHABLEND);
				youarep2->Draw(youarep1txt,NULL,NULL,&pc,0xFFFFFFFF);
				youarep2->End(); 
			} else {
				p2connect->SetTransform(&mat);
				p2connect->Begin(D3DXSPRITE_ALPHABLEND);
				p2connect->Draw(p1connecttxt,NULL,NULL,&pc,0xFFFFFFFF);
				p2connect->End(); 
			}
		}
		if(playercount >= 3) {
			D3DXVECTOR2 trans=D3DXVECTOR2(25,490);
			D3DXVECTOR2 scaling(1.9f,1.9f);
			D3DXMATRIX mat;
			D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&spriteCentre,0.f,&trans);

			if (ready[3]) {
				playerready->SetTransform(&mat);
				playerready->Begin(D3DXSPRITE_ALPHABLEND);
				playerready->Draw(playerreadytxt,NULL,NULL,&pc,0xFFFFFFFF);
				playerready->End(); 
			} else if (playernumber == 3) {
				youarep1->SetTransform(&mat);
				youarep1->Begin(D3DXSPRITE_ALPHABLEND);
				youarep1->Draw(youarep3txt,NULL,NULL,&pc,0xFFFFFFFF);
				youarep1->End(); 
			} else {
				p3connect->SetTransform(&mat);
				p3connect->Begin(D3DXSPRITE_ALPHABLEND);
				p3connect->Draw(p3connecttxt,NULL,NULL,&pc,0xFFFFFFFF);
				p3connect->End(); 
			}
		}
		if(playercount >= 4) { //747,490
			D3DXVECTOR2 trans=D3DXVECTOR2(747,490);
			D3DXMATRIX mat;
			D3DXVECTOR2 scaling(1.9f,1.9f);
			D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&spriteCentre,0.f,&trans);

			if (ready[4]) {
				playerready->SetTransform(&mat);
				playerready->Begin(D3DXSPRITE_ALPHABLEND);
				playerready->Draw(playerreadytxt,NULL,NULL,&pc,0xFFFFFFFF);
				playerready->End(); 
			} else if (playernumber == 4) {
				youarep4->SetTransform(&mat);
				youarep4->Begin(D3DXSPRITE_ALPHABLEND);
				youarep4->Draw(youarep4txt,NULL,NULL,&pc,0xFFFFFFFF);
				youarep4->End(); 
			} else {
				p4connect->SetTransform(&mat);
				p4connect->Begin(D3DXSPRITE_ALPHABLEND);
				p4connect->Draw(p4connecttxt,NULL,NULL,&pc,0xFFFFFFFF);
				p4connect->End(); 
			}
		}
		/*
		if(!ready[playernumber]) {
			D3DXVECTOR3 rdy(300,200,0);
			displaytexture(&pressstart, &rdy, &pressstarttxt);
		}*/
		D3DXVECTOR3 blk(0,0,0.5);
		D3DXVECTOR2 trans=D3DXVECTOR2(0.0f,0.0f);
		D3DXVECTOR2 scaling(1.f,0.75f);
		D3DXMATRIX mat;
		D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&spriteCentre,0.f,&trans);

		blackbackground->SetTransform(&mat);
		blackbackground->Begin(D3DXSPRITE_ALPHABLEND);
		blackbackground->Draw(blackbackgroundtxt,NULL,NULL,&blk,0xFFFFFFFF);
		blackbackground->End();
		
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
