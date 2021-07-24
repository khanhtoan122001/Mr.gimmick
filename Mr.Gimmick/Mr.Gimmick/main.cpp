#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>

#include "Trigger.h"
#include "Textures.h"
#include "Game.h"
#include "GameObject.h"
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include "Brick.h"
#include "Nakiri.h"
#include "Point.h"
#include <unordered_map>
#include "map.h"
#include "Quadtree.h"
#include "Rect.h"
#include "Trap.h"
#include "boom.h"
#include "cannons.h"
#include "Star.h"

#define ID_MAP_1 120
#define ID_MAP_7 180
#define ID_NAKIRI_RIGHT 15
#define ID_NAKIRI_LEFT 16
#define ID_TRAP 347
#define ID_ENEMIES_RIGHT 15058
#define ID_ENEMIES_LEFT 15235
#define ID_BOOM 16
#define ID_CHARGE_STAR 16544

#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"Gimmick"
#define STARTPOS_STAGE_1_X 32
#define STARTPOS_STAGE_1_Y 80


#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 200)


#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

#define MAX_FRAME_RATE 120

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20

#define TRAP_NORMAL 344
CGame* game;
Nakiri* nakiri;
Boom* boom;
Quadtree* quadtree;
Trap tp[2];
Trigger trigg;
Map* map;
Cannon* cannon;
Star* star;

class CSampleKeyHander : public CKeyEventHandler
{
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander* keyHandler;

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_SPACE:
		nakiri->SetState(NAKIRI_STATE_JUMP);
		break;
	case DIK_S:
		if (!star->isPress()) {
			if(!star->Active)
				star->SetState(STAR_CHARGE);
			star->Press();
		}
		break;
	}
}

Quadtree* CreateQuadTree(vector<LPGAMEOBJECT> list, Point p)
{
	// Init base game region for detecting collision
	Quadtree* quadtree = new Quadtree(1, new Rect(p - Point(5, 5) * 16, p + Point(5, 5) * 16));
	for (auto i = list.begin(); i != list.end(); i++)
		quadtree->Insert(*i);

	return quadtree;
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_S:
		if (star->isComplete)
			star->Shot();
		else
			star->SetState(STAR_HIDE);
		star->Press();
		break;
	}
}

void CSampleKeyHander::KeyState(BYTE* states)
{
	// disable control key when Mario die 
	if (nakiri->GetState() == NAKIRI_STATE_DIE) return;
	if (game->IsKeyDown(DIK_RIGHT))
		nakiri->SetState(NAKIRI_STATE_WALKING_RIGHT);

	/*else if (game->IsKeyDown(DIK_UP))
		nakiri->SetState(NAKIRI_STATE_UP);
	else if (game->IsKeyDown(DIK_DOWN))
		nakiri->SetState(NAKIRI_STATE_DOWN);*/

	else if (game->IsKeyDown(DIK_LEFT))
		nakiri->SetState(NAKIRI_STATE_WALKING_LEFT);
	else
		nakiri->SetState(NAKIRI_STATE_STAND);
}

vector<vector<int>> MapTile;
vector<vector<vector<int>>> MapObj;
vector<LPGAMEOBJECT> objects, screenObj, actObj, moveObj;
vector<LPGAMEOBJECT>* coObj = new vector<LPGAMEOBJECT>();

LPDIRECT3DTEXTURE9 texMap1;
int lx, ly;
int Stage;
Point tf, br;



using json = nlohmann::json;
using namespace std;

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void UpdateActObj(Point p) {
	float cx = p.x, cy = p.y;
	int stx = int(cx / BRICK_HEIGHT) - 5, sty = int(cy / BRICK_WIDTH) - 5;
	if (stx < 0) stx = 0;
	if (sty < 0) sty = 0;
	actObj.clear();
	for (int y = sty; y < sty + 5 + SCREEN_HEIGHT / BRICK_HEIGHT && y < MapTile.size(); y++) {
		for (int x = stx; x < stx + 5 + SCREEN_WIDTH / BRICK_WIDTH && x < MapTile[y].size(); x++) {
			for(int i = 0; i < MapObj[y][x].size(); i++)
				if (MapObj[y][x][i] != -1)
					actObj.push_back(objects.at(MapObj[y][x][i]));
		}
	}
	actObj.push_back(&trigg);
}

void LoadResource() {

	CTextures* textures = CTextures::GetInstance();
	textures->Add(ID_MAP_1, L"Resource//NES - Gimmick Mr Gimmick - Stage 1.png", D3DCOLOR_XRGB(255,0,255));
	textures->Add(ID_MAP_7, L"Resource//NES - Gimmick Mr Gimmick - Stage 7.png", D3DCOLOR_XRGB(99, 30, 100));
	textures->Add(ID_NAKIRI_RIGHT, L"Resource//NES - Gimmick Mr Gimmick - Yumetaro.png", D3DCOLOR_XRGB(0, 0, 255));
	textures->Add(ID_NAKIRI_LEFT, L"Resource//NES - Gimmick Mr Gimmick - Yumetaro(1).png", D3DCOLOR_XRGB(0, 0, 255));
	textures->Add(ID_TRAP, L"Resource//NES - Gimmick Mr Gimmick - Hazards and Interactables.png", D3DCOLOR_XRGB(203, 102, 185));
	textures->Add(ID_TEX_BBOX, L"Resource//Untitled.png", D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_ENEMIES_RIGHT, L"Resource//NES - Gimmick Mr Gimmick - Enemies.png", D3DCOLOR_XRGB(57, 189, 255));
	textures->Add(ID_ENEMIES_LEFT, L"Resource//NES - Gimmick Mr Gimmick - Enemies(1).png", D3DCOLOR_XRGB(57, 189, 255));
	textures->Add(ID_CHARGE_STAR, L"Resource//star-charge.png", D3DCOLOR_XRGB(255, 174, 201));
	
	CSprites* sprites = CSprites::GetInstance();
	LPDIRECT3DTEXTURE9 texMap1 = textures->Get(ID_MAP_1);
	LPDIRECT3DTEXTURE9 charge = textures->Get(ID_CHARGE_STAR);

	for (int j = 0; j < 23; j++) {
		for (int i = 0; i < 14; i++) {
			sprites->Add(j * 14 + i + 1000, 2 + i * 34, 2 + j * 34, (i + 1) * 34, (j + 1) * 34, texMap1);
		}
	}

	sprites->Add(20000, 5, 5, 243, 235, charge);
	sprites->Add(20001, 251, 5, 489, 235, charge);
	sprites->Add(20002, 493, 5, 493 + STAR_CHARGE_WIDTH, 5 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20003, 7, 244, 7 + STAR_CHARGE_WIDTH, 244 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20004, 253, 244, 253 + STAR_CHARGE_WIDTH, 244 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20005, 495, 244, 495 + STAR_CHARGE_WIDTH, 244 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20006, 7, 481, 7 + STAR_CHARGE_WIDTH, 481 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20007, 253, 481, 253 + STAR_CHARGE_WIDTH, 481 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20008, 495, 481, 495 + STAR_CHARGE_WIDTH, 481 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20009, 6, 717, 6 + STAR_CHARGE_WIDTH, 717 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20010, 252, 717, 252 + STAR_CHARGE_WIDTH, 717 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20011, 494, 717, 494 + STAR_CHARGE_WIDTH, 717 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20012, 6, 953, 6 + STAR_CHARGE_WIDTH, 953 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20013, 252, 953, 252 + STAR_CHARGE_WIDTH, 953 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20014, 494, 953, 494 + STAR_CHARGE_WIDTH, 953 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20015, 735, 953, 735 + STAR_CHARGE_WIDTH, 953 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20016, 981, 953, 981 + STAR_CHARGE_WIDTH, 953 + STAR_CHARGE_HEIGHT, charge);
	sprites->Add(20017, 739, 716, 739 + STAR_CHARGE_WIDTH, 716 + STAR_CHARGE_HEIGHT, charge);

	LPDIRECT3DTEXTURE9 trap = textures->Get(ID_TRAP);
	sprites->Add(12345, 2 * 2, 2, 15 * 2, 36, trap);
	sprites->Add(12346, 19 * 2, 2, 32 * 2, 36, trap);
	sprites->Add(12347, 36 * 2, 2, 49 * 2, 36, trap);
	sprites->Add(12348, 53 * 2, 2, 66 * 2, 36, trap);

	sprites->Add(12500, 170, 44, 202, 76, trap);
	sprites->Add(12501, 204, 44, 236, 76, trap);
	sprites->Add(12502, 306, 383, 338, 415, trap);
	sprites->Add(12503, 340, 383, 372, 415, trap);
	sprites->Add(12504, 374, 383, 406, 415, trap);
	sprites->Add(12505, 374 + 34, 383, 406 + 34, 415, trap);
	sprites->Add(12506, 374 + 34 * 2, 383, 406 + 34 * 2, 415, trap);
	sprites->Add(12507, 374 + 34 * 3, 383, 406 + 34 * 3, 415, trap);

	//cannon
	sprites->Add(12400, 170, 4, 202, 36, trap);
	sprites->Add(12401, 204, 4, 236, 36, trap);

	LPDIRECT3DTEXTURE9 texNakiriR = textures->Get(ID_NAKIRI_RIGHT);
	//stand right
	sprites->Add(10000, 4, 4, 36, 42, texNakiriR);
	sprites->Add(10001, 44, 4, 76, 42, texNakiriR);

	//Star
	sprites->Add(17000, 568, 282, 600, 314, texNakiriR);
	sprites->Add(17001, 604, 282, 636, 314, texNakiriR);

	//Star explosive
	sprites->Add(17100, 640, 282, 672, 314, texNakiriR);
	sprites->Add(17101, 680, 282, 712, 314, texNakiriR);
	sprites->Add(17102, 718, 282, 750, 314, texNakiriR);
	sprites->Add(17103, 754, 282, 786, 314, texNakiriR);
	sprites->Add(17104, 790, 282, 822, 314, texNakiriR);

	//walk right
	sprites->Add(10002, 4, 48, 36, 86, texNakiriR);
	sprites->Add(10003, 42, 48, 37 * 2, 86, texNakiriR);
	sprites->Add(10004, 78, 48, 55*2, 86, texNakiriR);
	sprites->Add(10005, 116, 48, 74*2, 86, texNakiriR);
	sprites->Add(10006, 158, 48, 95*2, 86, texNakiriR);
	sprites->Add(10007, 198, 48, 115*2, 86, texNakiriR);

	sprites->Add(10016, 2*2, 46*2, 18*2, 70*2, texNakiriR);


	LPDIRECT3DTEXTURE9 texNakiriL = textures->Get(ID_NAKIRI_LEFT);
	//stand left
	sprites->Add(10008, 399*2, 2*2, 415*2, 21*2, texNakiriL);
	sprites->Add(10009, 379*2, 2*2, 395*2, 21*2, texNakiriL);

	//walk left
	sprites->Add(10010, 302*2, 24*2, 318*2, 43*2, texNakiriL);
	sprites->Add(10011, 322*2, 24*2, 338*2, 43*2, texNakiriL);
	sprites->Add(10012, 343*2, 24*2, 359*2, 43*2, texNakiriL);
	sprites->Add(10013, 362*2, 24*2, 378*2, 43*2, texNakiriL);
	sprites->Add(10014, 380*2, 24*2, 396*2, 43*2, texNakiriL);
	sprites->Add(10015, 399*2, 24*2, 415*2, 43*2, texNakiriL);

	sprites->Add(10017, 399*2, 46*2, 415*2, 70*2, texNakiriL);

	//move_brick
	sprites->Add(15000, 306, 425, 370, 457, trap);


	CAnimations* animations = CAnimations::GetInstance();
	LPANIMATION ani;

	for (int j = 0; j < 23; j++) {
		for (int i = 0; i < 14; i++) {
			ani = new CAnimation(100);
			ani->Add(j * 14 + i + 1000);
			animations->Add(j * 14 + i, ani);
		}
	}
	

	ani = new CAnimation(100);
	ani->Add(1132);
	ani->Add(1133);
	ani->Add(1134);
	ani->Add(1135);
	animations->Add(135, ani);

	ani = new CAnimation(100);
	ani->Add(1156);
	ani->Add(1155);
	ani->Add(1154);
	ani->Add(1157);
	animations->Add(156, ani);
	animations->Add(155, ani);

	ani = new CAnimation(100);
	ani->Add(1140);
	ani->Add(1141);
	ani->Add(1142);
	ani->Add(1143);
	animations->Add(141, ani);

	ani = new CAnimation(100);
	ani->Add(1168);
	ani->Add(1170);
	ani->Add(1196);
	ani->Add(1198);
	animations->Add(168, ani);

	ani = new CAnimation(100);
	ani->Add(1169);
	ani->Add(1171);
	ani->Add(1197);
	ani->Add(1199);
	animations->Add(169, ani);

	ani = new CAnimation(100);
	ani->Add(1182);
	ani->Add(1184);
	ani->Add(1210);
	ani->Add(1212);
	animations->Add(182, ani);

	ani = new CAnimation(60);
	for (int i = 0; i < 18; i++)
		ani->Add(20000 + i);
	animations->Add(CHARGE_ANI, ani);


	ani = new CAnimation(100);
	ani->Add(17000);
	ani->Add(17001);
	animations->Add(ACTIVE_ANI, ani);

	ani = new CAnimation(100);
	ani->Add(17100);
	ani->Add(17101);
	ani->Add(17102);
	ani->Add(17103);
	ani->Add(17104);
	animations->Add(EXPLOSIVE_ANI, ani);

	ani = new CAnimation(100);
	ani->Add(1183);
	ani->Add(1185);
	ani->Add(1211);
	ani->Add(1213);
	animations->Add(183, ani);


	//	thac nuoc


	//	banh rang nho
	ani = new CAnimation(100);
	ani->Add(1172);
	ani->Add(1186);
	ani->Add(1200);
	ani->Add(1214);
	animations->Add(172, ani);

	ani = new CAnimation(500);
	ani->Add(10000);
	ani->Add(10001);
	animations->Add(NAKIRI_ANI_STAND_RIGHT, ani);

	ani = new CAnimation(50);
	ani->Add(10002);
	ani->Add(10003);
	ani->Add(10004);
	ani->Add(10005);
	ani->Add(10006);
	ani->Add(10007);
	animations->Add(NAKIRI_ANI_WALKING_RIGHT, ani);

	ani = new CAnimation(50);
	ani->Add(10010);
	ani->Add(10011);
	ani->Add(10012);
	ani->Add(10013);
	ani->Add(10014);
	ani->Add(10015);
	animations->Add(NAKIRI_ANI_WALKING_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(15000);
	animations->Add(MOVE_BRICK_ANI, ani);

	ani = new CAnimation(100);
	ani->Add(12500);
	ani->Add(12501);
	ani->Add(12502);
	ani->Add(12503);
	ani->Add(12504);
	ani->Add(12505);
	ani->Add(12506);
	ani->Add(12507);
	animations->Add(BULLET_ANI, ani);

	ani = new CAnimation(500);
	ani->Add(10008);
	ani->Add(10009);
	animations->Add(NAKIRI_ANI_STAND_LEFT, ani);

	ani = new CAnimation(100);
	ani->Add(10016);
	animations->Add(NAKIRI_ANI_JUMP_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(10017);
	animations->Add(NAKIRI_ANI_JUMP_LEFT, ani);

	ani = new CAnimation(500);	//cannon
	ani->Add(12400);
	ani->Add(12401);
	animations->Add(CANNON_ANI, ani);

	cannon = new Cannon();
	cannon->SetWidthHeight(32, 32);
	cannon->AddAnimation(CANNON_ANI);
	cannon->SetPosition(2336, 1280);
	
	ani = new CAnimation(10);
	ani->Add(12345);
	ani->Add(12346);
	ani->Add(12347);
	ani->Add(12348);
	animations->Add(TRAP_NORMAL, ani);

	tp[0].SetPosition(864 * 2, 416 * 2);
	tp[1].SetPosition(816 * 2, 416 * 2);
	for (int i = 0; i < 2; i++)
	{
		tp[i].AddAnimation(TRAP_NORMAL);
	}
	

	nakiri = Nakiri::GetInstance();
	nakiri->SetPosition(32 * 2, 2 * 16 * 12);
	nakiri->AddAnimation(NAKIRI_ANI_STAND_RIGHT);
	nakiri->AddAnimation(NAKIRI_ANI_WALKING_RIGHT);
	nakiri->AddAnimation(NAKIRI_ANI_STAND_LEFT);
	nakiri->AddAnimation(NAKIRI_ANI_WALKING_LEFT);
	nakiri->AddAnimation(NAKIRI_ANI_JUMP_RIGHT);
	nakiri->AddAnimation(NAKIRI_ANI_JUMP_LEFT);
	//objects.push_back(nakiri);

	LPDIRECT3DTEXTURE9 enemiesR = textures->Get(ID_ENEMIES_RIGHT);
	sprites->Add(11000, 2*2, 2, 18 * 2, 34, enemiesR);
	sprites->Add(11001, 20 * 2, 2, 36 * 2, 34, enemiesR);
	sprites->Add(11002, 38 * 2, 2, 54 * 2, 34, enemiesR);

	LPDIRECT3DTEXTURE9 enemiesL = textures->Get(ID_ENEMIES_LEFT);
	sprites->Add(11005, 642 * 2, 2, 658 * 2, 34, enemiesL);
	sprites->Add(11004, 660 * 2, 2, 676 * 2, 34, enemiesL);
	sprites->Add(11003, 678 * 2, 2, 694 * 2, 34, enemiesL);

	ani = new CAnimation(100);
	ani->Add(11000);
	ani->Add(11001);
	ani->Add(11002);
	animations->Add(BOOM_ANI_WALK_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(11003);
	ani->Add(11004);
	ani->Add(11005);
	animations->Add(BOOM_ANI_WALK_LEFT, ani);

	boom = Boom::GetInstance();
	boom->SetPosition(32 * 6 * 2, 16 * 21 * 2);
	boom->AddAnimation(BOOM_ANI_WALK_RIGHT);
	boom->AddAnimation(BOOM_ANI_WALK_LEFT);
	objects.push_back(boom);
}
void Obj(GameObject* brick, int i, Style style, Point p, int w, int h) {

	brick->SetStyle(style);

	brick->SetPosition(p.x, p.y);
	brick->SetWidthHeight(w, h);
	int x, y;

	x = (int)(brick->x / BRICK_HEIGHT);
	y = (int)(brick->y / BRICK_WIDTH);

	objects.push_back(brick);
	MapObj[y][x].push_back(objects.size() - 1);
}
void LoadMap(string MapFile) {
	ifstream ifs{ MapFile };
	json jsonfile = json::parse(ifs);


	vector<vector<int>> r_map;
	vector<int> lineMapTile;
	vector<vector<int>> lineMapObj;
	r_map.push_back(jsonfile["layers"][0]["data"]);

	int w = jsonfile["layers"][0]["width"], h = jsonfile["layers"][0]["height"];
	for (int i = 0; i < r_map[0].size(); i++) {
		 {
			lineMapTile.push_back(r_map[0][i] - 1);
			vector<int> a = vector<int>();
			lineMapObj.push_back(a);
			if (lineMapTile.size() == w) {
				MapTile.push_back(lineMapTile);
				lineMapTile.clear();
				MapObj.push_back(lineMapObj);
				lineMapObj.clear();
			}
		}
	}
	
	//for (int i = 0; i < Map.size(); i++) {
	//	for (int j = 0; j < Map[i].size(); j++) {
	//		Brick* brick = new Brick();
	//		if (Map[i][j] != -1) {
	//			brick->SetPosition(BRICK_HEIGHT * (j), BRICK_WIDTH * (i));
	//			brick->AddAnimation(Map[i][j]);
	//			objects.push_back(brick);
	//		}
	//	}
	//}

	for (int i = 0; i < jsonfile["layers"][1]["objects"].size(); i++) {
		Style style;
		int des = -1;
		string type = jsonfile["layers"][1]["objects"][i]["type"];
		/*int id = jsonfile["layers"][1]["objects"][i]["id"];*/
		if (type == "0")
		{
			style = normal_brick;
		}
		else if (type == "1") {
			style = diagonal_left;
		}
		else if (type == "2") {
			style = diagonal_right;
		}
		else if (type == "3") {
			style = main_c;
		}
		else if (type == "4"){
			style = slide_left;
		}
		else if (type == "5") {
			style = slide_right;
		}
		else if (type == "6") {
			style = spike;
		}
		else if (type == "7") {
			style = trap;
		}
		else if (type == "8") {
			style = trigger;
		}
		else if (type == "9") {
			style = move_brick;
		}
		Point p = Point(jsonfile["layers"][1]["objects"][i]["x"], jsonfile["layers"][1]["objects"][i]["y"]);
		int w = jsonfile["layers"][1]["objects"][i]["width"];
		int h = jsonfile["layers"][1]["objects"][i]["height"];

		if (style == trigger) {
			Trigger* trigg = new Trigger();
			trigg->SetPenetrable(true);
			if(des >= 0)
				trigg->setTrap(&tp[des]);
			Obj(trigg, i, style, p, w, h);
		}
		else {
			Brick* brick = new Brick();
			Obj(brick, i, style, p, w, h);
		}
	}
	
}


void Update(DWORD dt);

void Render_Map();
void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();
	

	if (d3ddv->BeginScene())
	{
		// Clear back buffer with a color
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

		//CGame::GetInstance()->GetCurrentScene()->Render();
		D3DXVECTOR3 p(16, 16, 0);
		spriteHandler->Draw(texMap1, NULL, NULL, &p, BACKGROUND_COLOR);

		Render_Map();
		nakiri->Render();
		boom->Render();

		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}



void setCam(float x, float y) {
	int ox = (int)CGame::GetInstance()->GetCamPos_x();
	int cx, cy;

	if (x - (GAME_PLAY_WIDTH / 2 - 1) * BRICK_WIDTH <= tf.x)
		cx = tf.x;
	else if (x + (GAME_PLAY_WIDTH / 2 + 1) * BRICK_WIDTH > br.x + 32 + 16)
		cx = br.x - GAME_PLAY_WIDTH * BRICK_WIDTH + 32 + 16;
	else {
		cx = x - (GAME_PLAY_WIDTH / 2 - 1) * BRICK_WIDTH;
	}

	cy = (int)(y / BRICK_HEIGHT / GAME_PLAY_HEIGHT)* BRICK_HEIGHT* GAME_PLAY_HEIGHT;

	CGame::GetInstance()->SetCamPos(/*(int)*/cx,cy);
	
}

void UpdateObj(GameObject* obj, DWORD dt) {
	coObj->clear();

	UpdateActObj(obj->GetPos());

	quadtree = CreateQuadTree(actObj, obj->GetPos());

	quadtree->Retrieve(coObj, obj);

	if(obj->type != g_star)
		coObj->push_back(nakiri);

	obj->Update(dt, coObj);

	quadtree->~Quadtree();
}

void Update(DWORD dt) {
	float cx, cy;

	UpdateObj(boom, dt);

	UpdateObj(cannon, dt);


	coObj->clear();

	UpdateActObj(nakiri->GetPos());

	//coObj = &actObj;
	quadtree = CreateQuadTree(actObj, nakiri->GetPos());

	quadtree->Retrieve(coObj, nakiri);

	nakiri->GetPosition(cx, cy);

	Map::GetInstance()->updateMap(cx, cy, tf, br);
	Map::GetInstance()->updateMapObject(coObj);

	for (int i = 0; i < cannon->bullets.size(); i++)
		cannon->bullets[i]->Update(dt);

	vector<LPGAMEOBJECT>* coObj2 = new vector<LPGAMEOBJECT>();

	for (int i = 0; i < coObj->size(); i++) {
		coObj2->push_back(coObj->at(i));
	}
	coObj2->push_back(cannon);
	coObj2->push_back(boom);

	coObj->push_back(nakiri);

	for (int i = 0; i < coObj->size() - 1; i++)
	{
		coObj->at(i)->Update(dt, coObj);
	}
	for (int i = 0; i < 2; i++)
	{
		tp[i].Update(dt, coObj);
	}

	nakiri->Update(dt, coObj2);

	UpdateObj(star, dt);

	nakiri->GetPosition(cx, cy);

	/*for (int i = 0; i < actObj.size(); i++)
		actObj.at(i)->Update(dt);*/

	setCam(cx, cy);
}

void Render_Map() {
	float cx = CGame::GetInstance()->GetCamPos_x(), cy = CGame::GetInstance()->GetCamPos_y();

	//Map::GetInstance()->updateMap(nakiri->x, nakiri->y, tf, br, coObj);

	screenObj.clear();
	int stx = int(cx / BRICK_HEIGHT), sty = int(cy / BRICK_WIDTH);
	if (stx < 0) stx = 0;
	if (sty < 0) sty = 0;
	for (int y = sty; y < sty + GAME_PLAY_HEIGHT && y < MapTile.size(); y++) {
		for (int x = stx; x <= stx + GAME_PLAY_WIDTH && x < MapTile[y].size(); x++) {
			LPANIMATION ani;
			if (MapTile[y][x] != -1) {
				ani = CAnimations::GetInstance()->Get(MapTile[y][x]);
				if(ani != NULL)
					ani->Render(BRICK_HEIGHT * (x)+ cx - (int)(cx), BRICK_WIDTH * (y)+ cy - (int)(cy));
			}
			for (int i = 0; i < MapObj[y][x].size(); i++) {
				screenObj.push_back(objects.at(MapObj[y][x][i]));
			}
		}
 	}
	/*for (int i = 0; i < screenObj.size(); i++)
		screenObj.at(i)->Render();*/
	vector<LPGAMEOBJECT>* list = Map::GetInstance()->getList();
	for (int i = 0; i < list->size(); i++)
		list->at(i)->Render();



	for (int i = 0; i < cannon->bullets.size(); i++)
		cannon->bullets[i]->Render();
	for (int i = 0; i < coObj->size(); i++) {
		coObj->at(i)->RenderBoundingBox();
		//coObj->at(i)->Render();
	}
	for (int i = 0; i < 2; i++)
	{
		tp[i].Render();
	}
	star->Render();
	cannon->Render();
	/*for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render();
	}*/
}

int Run()
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

 	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();

			Update(dt);

			Render();
		}
		else
			Sleep(tickPerFrame - dt);
	}

	return 1;
}
HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd)
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH * 1.0, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);

	

	lx = ly = 0;

	LoadResource();
	LoadMap("Maps\\map1.json");

	star = new Star();

	
	Run();
	return 0;
}