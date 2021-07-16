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

#define ID_MAP_1 120
#define ID_MAP_7 180
#define ID_NAKIRI_RIGHT 15
#define ID_NAKIRI_LEFT 16
#define ID_TRAP 347
#define ID_ENEMIES_RIGHT 15058
#define ID_ENEMIES_LEFT 15235
#define ID_BOOM 16

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

void updateLimit(int stage);
void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
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

vector<vector<int>> MapTile, MapObj;
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
			if (MapObj[y][x] != -1)
				actObj.push_back(objects.at(MapObj[y][x]));
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
	textures->Add(ID_ENEMIES_LEFT, L"Resource//NES - Gimmick Mr Gimmick - Enemies(1).png", D3DCOLOR_XRGB(57,189,255));
	
	CSprites* sprites = CSprites::GetInstance();
	LPDIRECT3DTEXTURE9 texMap1 = textures->Get(ID_MAP_1);

	for (int j = 0; j < 23; j++) {
		for (int i = 0; i < 14; i++) {
			sprites->Add(j * 14 + i + 1000, 1 + i * 17, 1 + j * 17, (i + 1) * 17, (j + 1) * 17, texMap1);
		}
	}

	

	LPDIRECT3DTEXTURE9 trap = textures->Get(ID_TRAP);
	sprites->Add(12345, 2, 1, 15, 18, trap);
	sprites->Add(12346, 19, 1, 32, 18, trap);
	sprites->Add(12347, 36, 1, 49, 18, trap);
	sprites->Add(12348, 53, 1, 66, 18, trap);

	LPDIRECT3DTEXTURE9 texNakiriR = textures->Get(ID_NAKIRI_RIGHT);
	//stand right
	sprites->Add(10000, 2, 2, 18, 21, texNakiriR);
	sprites->Add(10001, 22, 2, 38, 21, texNakiriR);

	//walk right
	sprites->Add(10002, 2, 24, 18, 43, texNakiriR);
	sprites->Add(10003, 21, 24, 37, 43, texNakiriR);
	sprites->Add(10004, 39, 24, 55, 43, texNakiriR);
	sprites->Add(10005, 58, 24, 74, 43, texNakiriR);
	sprites->Add(10006, 79, 24, 95, 43, texNakiriR);
	sprites->Add(10007, 99, 24, 115, 43, texNakiriR);

	sprites->Add(10016, 2, 46, 18, 70, texNakiriR);


	LPDIRECT3DTEXTURE9 texNakiriL = textures->Get(ID_NAKIRI_LEFT);
	//stand left
	sprites->Add(10008, 399, 2, 415, 21, texNakiriL);
	sprites->Add(10009, 379, 2, 395, 21, texNakiriL);

	//walk left
	sprites->Add(10010, 302, 24, 318, 43, texNakiriL);
	sprites->Add(10011, 322, 24, 338, 43, texNakiriL);
	sprites->Add(10012, 343, 24, 359, 43, texNakiriL);
	sprites->Add(10013, 362, 24, 378, 43, texNakiriL);
	sprites->Add(10014, 380, 24, 396, 43, texNakiriL);
	sprites->Add(10015, 399, 24, 415, 43, texNakiriL);

	sprites->Add(10017, 399, 46, 415, 70, texNakiriL);


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
	
	ani = new CAnimation(10);
	ani->Add(12345);
	ani->Add(12346);
	ani->Add(12347);
	ani->Add(12348);
	animations->Add(TRAP_NORMAL, ani);

	tp[0].SetPosition(864, 416);
	tp[1].SetPosition(816, 416);
	for (int i = 0; i < 2; i++)
	{
		tp[i].AddAnimation(TRAP_NORMAL);
	}
	

	nakiri = Nakiri::GetInstance();
	nakiri->SetPosition(32, 16 * 12);
	nakiri->AddAnimation(NAKIRI_ANI_STAND_RIGHT);
	nakiri->AddAnimation(NAKIRI_ANI_WALKING_RIGHT);
	nakiri->AddAnimation(NAKIRI_ANI_STAND_LEFT);
	nakiri->AddAnimation(NAKIRI_ANI_WALKING_LEFT);
	nakiri->AddAnimation(NAKIRI_ANI_JUMP_RIGHT);
	nakiri->AddAnimation(NAKIRI_ANI_JUMP_LEFT);
	objects.push_back(nakiri);

	LPDIRECT3DTEXTURE9 enemiesR = textures->Get(ID_ENEMIES_RIGHT);
	sprites->Add(11000, 2, 1, 18, 17, enemiesR);
	sprites->Add(11001, 20, 1, 36, 17, enemiesR);
	sprites->Add(11002, 38, 1, 54, 17, enemiesR);

	LPDIRECT3DTEXTURE9 enemiesL = textures->Get(ID_ENEMIES_LEFT);
	sprites->Add(11005, 642, 1, 658, 17, enemiesL);
	sprites->Add(11004, 660, 1, 676, 17, enemiesL);
	sprites->Add(11003, 678, 1, 694, 17, enemiesL);

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
	boom->SetPosition(32 * 6, 16 * 21);
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
	if (MapObj[y][x] != NULL)
		MapObj[y][x] = objects.size() - 1;
}
void LoadMap(string MapFile) {
	ifstream ifs{ MapFile };
	json jsonfile = json::parse(ifs);


	vector<vector<int>> r_map;
	vector<int> lineMapTile, lineMapObj;
	r_map.push_back(jsonfile["layers"][0]["data"]);

	int w = jsonfile["layers"][0]["width"], h = jsonfile["layers"][0]["height"];
	for (int i = 0; i < r_map[0].size(); i++) {
		 {
			lineMapTile.push_back(r_map[0][i] - 1);
			lineMapObj.push_back(-1);
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
		int id = jsonfile["layers"][1]["objects"][i]["id"];
		if (id == 1301)
		{
			style = (diagonal_left);
		}
		else if (id == 1289)
		{
			style = (slide_right);
		}
		else if (id == 1290) {
			style = slide_left;
		}
		else if (id == 1405 || id == 1406){
			style = trigger;
			des = 0;
		}
		else if (id == 1408) {
			style = trigger;
			//des = 0;
		}
		else if (id == 1403 || id == 1404)
		{
			style = trigger;
			des = 1;
		}
		else if (id == 1303) {
			style = (diagonal_left);
		}
		else
			style = normal_brick;
		
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

	if (x - (GAME_PLAY_WIDTH / 2 - 1) * BRICK_WIDTH < tf.x)
		cx = tf.x;
	else if (x + (GAME_PLAY_WIDTH / 2 + 1) * BRICK_WIDTH > br.x)
		cx = br.x - GAME_PLAY_WIDTH * BRICK_WIDTH;
	else {

		cx = x - (GAME_PLAY_WIDTH / 2 - 1) * BRICK_WIDTH;
	}

	cy = (int)(y / BRICK_HEIGHT / GAME_PLAY_HEIGHT)* BRICK_HEIGHT* GAME_PLAY_HEIGHT;

	CGame::GetInstance()->SetCamPos(/*(int)*/cx,cy);
	
}

void Update(DWORD dt) {
	float cx, cy;

	coObj->clear();

	UpdateActObj(boom->GetPos());

	quadtree = CreateQuadTree(actObj, boom->GetPos());

	quadtree->Retrieve(coObj, boom);

	boom->Update(dt, coObj);

	quadtree->~Quadtree();

	coObj->clear();

	UpdateActObj(nakiri->GetPos());

	//coObj = &actObj;
	quadtree = CreateQuadTree(actObj, nakiri->GetPos());

	quadtree->Retrieve(coObj, nakiri);

	nakiri->GetPosition(cx, cy);

	Map::GetInstance()->updateMap(cx, cy, tf, br, coObj);

	for (int i = 0; i < coObj->size(); i++)
	{
		coObj->at(i)->Update(dt);
	}
	for (int i = 0; i < 2; i++)
	{
		tp[i].Update(dt);
	}

	nakiri->Update(dt, coObj);

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
	for (int y = sty; y < int(cy / BRICK_HEIGHT) + GAME_PLAY_HEIGHT && y < MapTile.size(); y++) {
		for (int x = stx; x < int(cx / BRICK_WIDTH) + GAME_PLAY_WIDTH && x < MapTile[y].size(); x++) {
			LPANIMATION ani;
			if (MapTile[y][x] != -1) {
				ani = CAnimations::GetInstance()->Get(MapTile[y][x]);
				if(ani != NULL)
					ani->Render(BRICK_HEIGHT * (x)+ cx - (int)(cx), BRICK_WIDTH * (y)+ cy - (int)(cy));
			}
			if (MapObj[y][x] != -1)
				screenObj.push_back(objects.at(MapObj[y][x]));
		}
	}
	/*for (int i = 0; i < screenObj.size(); i++)
		screenObj.at(i)->Render();*/

	for (int i = 0; i < coObj->size(); i++)
		coObj->at(i)->RenderBoundingBox();;
	for (int i = 0; i < 2; i++)
	{
		tp[i].Render();
	}
	
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
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);

	lx = ly = 0;

	LoadResource();
	LoadMap("Maps\\map1.json");

	Run();
	return 0;
}