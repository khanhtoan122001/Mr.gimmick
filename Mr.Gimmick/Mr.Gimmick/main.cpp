#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>

#include "Tunnel.h"
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
#define ID_TUNNEL 350

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

#define TUNNEL_1_0 3500
#define TUNNEL_1_1 3501
#define TUNNEL_1_2 3502

CGame* game;
Nakiri* nakiri;
Boom* boom;
Quadtree* quadtree;
vector<Trap*> tp;
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
	if (nakiri->tunning || nakiri->tunning_rev) return;
	switch (KeyCode)
	{
	case DIK_SPACE:
		nakiri->SetState(NAKIRI_STATE_JUMP);
		break;
	case DIK_R:
		tp[0]->Reset();
		tp[1]->Reset();
		break;
	case DIK_T:
		nakiri->doubleJump = !nakiri->doubleJump;
		break;
	case DIK_S:
		if (star->canPress) {
			if(!star->isComplete && star->canShot)
				star->SetState(STAR_CHARGE);
			star->canPress = false;
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
		if (star->isComplete) {
			if (star->canShot)
				star->Shot();
		}
		else
			star->Reset();
		star->canPress = true;
		break;
	}
}

void CSampleKeyHander::KeyState(BYTE* states)
{
	// disable control key when Mario die 
	if (nakiri->GetState() == NAKIRI_STATE_DIE) return;
	if (nakiri->tunning || nakiri->tunning_rev) return;
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
	textures->Add(ID_TEX_BBOX, L"Resource//Untitled1.png", D3DCOLOR_XRGB(255, 255, 255));
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
	/*LPDIRECT3DTEXTURE9 tunnel = textures->Get(ID_TUNNEL);
	sprites->Add(35000, 0, 0, 15, 15, tunnel);
	sprites->Add(35001, 0, 17, 15, 32, tunnel);
	sprites->Add(35002, 0, 34, 15, 49, tunnel);
	sprites->Add(35003, 0, 51, 15, 66, tunnel);
	sprites->Add(35004, 0, 68, 15, 83, tunnel);
	sprites->Add(35005, 0, 85, 15, 100, tunnel);
	sprites->Add(35006, 0, 102, 15, 117, tunnel);
	sprites->Add(35007, 0, 119, 15, 134, tunnel);
	sprites->Add(35008, 0, 136, 15, 151, tunnel);
	sprites->Add(35009, 0, 153, 15, 168, tunnel);
	sprites->Add(35010, 0, 170, 15, 185, tunnel);
	sprites->Add(35011, 0, 187, 15, 202, tunnel);
	sprites->Add(35012, 0, 204, 15, 219, tunnel);
	sprites->Add(35013, 0, 221, 15, 236, tunnel);
	sprites->Add(35014, 0, 238, 15, 253, tunnel);
	sprites->Add(35015, 0, 255, 15, 270, tunnel);*/

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

	sprites->Add(18000, 4, 143, 41, 194, texNakiriR);
	sprites->Add(18001, 41, 143, 86, 194, texNakiriR);
	sprites->Add(18002, 84, 143, 129, 194, texNakiriR);
	sprites->Add(18003, 129, 143, 174, 194, texNakiriR);
	sprites->Add(18004, 188, 143, 233, 194, texNakiriR);
	sprites->Add(18005, 240, 143, 285, 194, texNakiriR);
	sprites->Add(18006, 294, 143, 339, 194, texNakiriR);
	sprites->Add(18007, 340, 143, 385, 194, texNakiriR);
	sprites->Add(18008, 393, 143, 438, 194, texNakiriR);
	sprites->Add(18009, 435, 143, 480, 194, texNakiriR);
	sprites->Add(18010, 477, 143, 522, 194, texNakiriR);

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

	sprites->Add(18020, 834 - 4, 143, 834 - 41, 194, texNakiriL);
	sprites->Add(18021, 834 - 41, 143, 834 - 86, 194, texNakiriL);
	sprites->Add(18022, 834 - 84, 143, 834 - 129, 194, texNakiriL);
	sprites->Add(18023, 834 - 129, 143, 834 - 174, 194, texNakiriL);
	sprites->Add(18024, 834 - 188, 143, 834 - 233, 194, texNakiriL);
	sprites->Add(18025, 834 - 240, 143, 834 - 285, 194, texNakiriL);
	sprites->Add(18026, 834 - 294, 143, 834 - 339, 194, texNakiriL);
	sprites->Add(18027, 834 - 340, 143, 834 - 385, 194, texNakiriL);
	sprites->Add(18028, 834 - 393, 143, 834 - 438, 194, texNakiriL);
	sprites->Add(18029, 834 - 435, 143, 834 - 480, 194, texNakiriL);
	sprites->Add(18030, 834 - 477, 143, 834 - 522, 194, texNakiriL);

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

	ani = new CAnimation(100);
	for (int i = 0; i < 11; i++)
		ani->Add(18000 + i);
	animations->Add(NAKIRI_ANI_STUN_RIGHT, ani);

	ani = new CAnimation(100);
	for (int i = 0; i < 11; i++)
		ani->Add(18020 + i);
	animations->Add(NAKIRI_ANI_STUN_LEFT, ani);

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
	animations->Add(TRAP_NORMAL_ANI, ani);

	


	nakiri = Nakiri::GetInstance();
	nakiri->SetPosition(32 * 2, 2 * 16 * 12);
	//objects.push_back(nakiri);

	LPDIRECT3DTEXTURE9 enemiesR = textures->Get(ID_ENEMIES_RIGHT);
	sprites->Add(11000, 2*2, 2, 18 * 2, 34, enemiesR);
	sprites->Add(11001, 20 * 2, 2, 36 * 2, 34, enemiesR);
	sprites->Add(11002, 38 * 2, 2, 54 * 2, 34, enemiesR);

	sprites->Add(11006, 112, 2, 144, 34, enemiesR);



	LPDIRECT3DTEXTURE9 enemiesL = textures->Get(ID_ENEMIES_LEFT);
	sprites->Add(11005, 642 * 2, 2, 658 * 2, 34, enemiesL);
	sprites->Add(11004, 660 * 2, 2, 676 * 2, 34, enemiesL);
	sprites->Add(11003, 678 * 2, 2, 694 * 2, 34, enemiesL);

	sprites->Add(11007, 1248, 2, 1280, 34, enemiesL);

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

	ani = new CAnimation(100);
	ani->Add(11006);
	animations->Add(BOOM_ANI_DIE_RIGHT, ani);

	ani = new CAnimation(100);
	ani->Add(11007);
	animations->Add(BOOM_ANI_DIE_LEFT, ani);

	Trap* _trap = new Trap();
	tp.push_back(_trap);
	_trap = new Trap();
	tp.push_back(_trap);

	tp[0]->SetPosition(864 * 2, 416 * 2);
	tp[1]->SetPosition(816 * 2, 416 * 2);
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

	for (int i = 0; i < jsonfile["layers"][1]["objects"].size(); i++) 
	{
		Style style;
		int des = -1;
		string type = jsonfile["layers"][1]["objects"][i]["type"];
		int id = jsonfile["layers"][1]["objects"][i]["id"];
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
		else if (type == "4") {
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
			style = trigger_Trap;
		}
		else if (type == "9") {
			style = move_brick;
		}
		else if (type == "13") {
			style = thorns;
		}
		else if (type == "14") {
			style = up_y;
		}
		else if (type == "15") {
			style = trigger_Enemies;
		}
		else if (id == 1412)
		{
			style = (tunnel);
		}
		else if (id == 1628)
		{
			style = tunnel1;
		}
		else if (id == 1631)
		{
			style = tunnel1_end;
		}
		else if (id == 1629)
		{
			style = tunnel1_1;
		}
		else if (id == 1632)
		{
			style = tunnel1_1_end;
		}
		else if (id == 1452)
		{
			style = corner_1_2;
		}
		else if (id == 1455)
		{
			style = corner_1_1;
		}
		else
			style = normal_brick;
		
		Point p = Point(jsonfile["layers"][1]["objects"][i]["x"], jsonfile["layers"][1]["objects"][i]["y"]);
		int w = jsonfile["layers"][1]["objects"][i]["width"];
		int h = jsonfile["layers"][1]["objects"][i]["height"];

		if (style == trigger_Trap) 
		{
			Trigger* trigg = new Trigger();
			if (id == 1824)
				trigg->setTrap(tp[0]);
			if (id == 1823)
				trigg->setTrap(tp[1]);
			Obj(trigg, i, style, p, w, h);
		}
		else if (style == trigger_Enemies) {
			Trigger* trigg = new Trigger();
			if (id == 1826 || id == 1825) {
				trigg->setEnemies(Map::GetInstance()->Stage1Enemies);
			}
			Map::GetInstance()->AddTrigger(trigg);
			//Obj(trigg, i, style, p, w, h);
		}
		else if (style == tunnel1 || style == tunnel1_end || style == tunnel1_1 || style == tunnel1_1_end)
		{
			Tunnel* tunnel = new Tunnel();
			tunnel->SetPenetrable(true);
			Obj(tunnel, i, style, p, w, h);
		}
		else if(style == up_y) {
			Brick* brick = new Brick();
			brick->SetPenetrable(true);
			Obj(brick, i, style, p, w, h);
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

	float cx, cy;
	nakiri->GetPosition(cx, cy);

	UpdateActObj(obj->GetPos());

	quadtree = CreateQuadTree(actObj, obj->GetPos());

	quadtree->Retrieve(coObj, obj);

	if(obj->type != g_star)
		coObj->push_back(nakiri);
	coObj->push_back(cannon);
	coObj->push_back(boom);
	coObj->push_back(star);
	Map::GetInstance()->updateMap(cx, cy, tf, br);
	Map::GetInstance()->updateMapObject(coObj);

	obj->Update(dt, coObj);

	quadtree->~Quadtree();
}

void Update(DWORD dt) {
	float cx, cy;

	UpdateObj(cannon, dt);

	UpdateObj(star, dt);

	for (int i = 0; i < 2; i++)
	{
		UpdateObj(tp[i], dt);
	}

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

	coObj->push_back(nakiri);
	coObj->push_back(cannon);
	coObj->push_back(star);
	coObj->push_back(tp[0]);
	coObj->push_back(tp[1]);

	for (int i = 0; i < coObj->size(); i++)
	{
		if (coObj->at(i)->type == g_boom)
			continue;
		if (coObj->at(i)->type == g_cannon)
			continue;
		if (coObj->at(i)->type == g_star)
			continue;
		if (coObj->at(i)->type == main_c)
			continue;
		if (coObj->at(i)->type == trap)
			continue;
		if (coObj->at(i)->type == trigger_Enemies) {
			Trigger* _trigg = dynamic_cast<Trigger*>(coObj->at(i));
		}
		coObj->at(i)->Update(dt, coObj);
	}

	nakiri->Update(dt, coObj);

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
	/*for (int i = 0; i < coObj->size(); i++) {
		coObj->at(i)->RenderBoundingBox();
		//coObj->at(i)->Render();
	}*/
	for (int i = 0; i < 2; i++)
	{
		tp[i]->Render();
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