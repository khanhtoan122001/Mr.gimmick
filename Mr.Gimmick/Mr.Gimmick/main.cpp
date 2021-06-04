#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>

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
#include "Quadtree.h"
#include "Rect.h"

#define ID_MAP_1 120
#define ID_MAP_7 180
#define ID_NAKIRI 15
#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"Gimmick"
#define STARTPOS_STAGE_1_X 32
#define STARTPOS_STAGE_1_Y 80

#define STAGE_1_MAP_TF Point(0,0)
#define STAGE_1_MAP_BR Point(65,24)

#define STAGE_2_MAP_TF Point(32,24)
#define STAGE_2_MAP_BR Point(65,36)

#define STAGE_3_MAP_TF Point(32,37)
#define STAGE_3_MAP_BR Point(82,48)

#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 200)
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256

#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

#define MAX_FRAME_RATE 120

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20

CGame* game;
Nakiri* nakiri;
Quadtree* quadtree;

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

Quadtree* CreateQuadTree(vector<LPGAMEOBJECT> list)
{
	// Init base game region for detecting collision
	Quadtree* quadtree = new Quadtree(1, new Rect(Point(CGame::GetInstance()->getCamPos()/* - Point(GAME_PLAY_WIDTH * 16, GAME_PLAY_HEIGHT * 16)*/),
		(GAME_PLAY_WIDTH * 1.5) * 16, (GAME_PLAY_HEIGHT * 1.5) * 16));
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
vector<LPGAMEOBJECT> objects, screenObj, actObj;
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

void UpdateActObj() {
	float cx = CGame::GetInstance()->GetCamPos_x(), cy = CGame::GetInstance()->GetCamPos_y();
	cx = nakiri->GetPos().x;
	cy = nakiri->GetPos().y;
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
}

void LoadResource() {

	CTextures* textures = CTextures::GetInstance();
	textures->Add(ID_MAP_1, L"Resource//NES - Gimmick Mr Gimmick - Stage 1.png", D3DCOLOR_XRGB(255,255,255));
	textures->Add(ID_MAP_7, L"Resource//NES - Gimmick Mr Gimmick - Stage 7.png", D3DCOLOR_XRGB(99, 30, 100));
	textures->Add(ID_NAKIRI, L"Resource//NES - Gimmick Mr Gimmick - Yumetaro.png", D3DCOLOR_XRGB(0, 0, 255));
	textures->Add(ID_TEX_BBOX, L"Resource//Untitled.png", D3DCOLOR_XRGB(255, 255, 255));
	
	CSprites* sprites = CSprites::GetInstance();
	LPDIRECT3DTEXTURE9 texMap1 = textures->Get(ID_MAP_1);

	for (int j = 0; j < 23; j++) {
		for (int i = 0; i < 14; i++) {
			sprites->Add(j * 14 + i + 1000, 1 + i * 17, 1 + j * 17, (i + 1) * 17, (j + 1) * 17, texMap1);
		}
	}

	LPDIRECT3DTEXTURE9 texNakiri = textures->Get(ID_NAKIRI);
	//stand
	sprites->Add(10000, 2, 2, 18, 21, texNakiri);
	sprites->Add(10001, 22, 2, 38, 21, texNakiri);

	//walk
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
	animations->Add(NAKIRI_ANI_STAND, ani);

	nakiri = new Nakiri(32, 288);
	nakiri->AddAnimation(NAKIRI_ANI_STAND);
	objects.push_back(nakiri);
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
		Brick* brick = new Brick();
		int id = jsonfile["layers"][1]["objects"][i]["id"];
		if (id == 1285)
		{
			brick->SetStyle(diagonal_left);
		}
		else if (id == 1289)
		{
			brick->SetStyle(slide_right);
		}
		else
			brick->style = normal_brick;
		brick->SetPosition(jsonfile["layers"][1]["objects"][i]["x"], jsonfile["layers"][1]["objects"][i]["y"]);
		brick->SetWidthHeight(jsonfile["layers"][1]["objects"][i]["width"], jsonfile["layers"][1]["objects"][i]["height"]);
		int x, y;

		x = (int)(brick->x / BRICK_HEIGHT);
		y = (int)(brick->y / BRICK_WIDTH);

		objects.push_back(brick);
		if (MapObj[y][x] != NULL)
			MapObj[y][x] = objects.size() - 1;
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

		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}

void updateStage(float x, float y) {
	Rect rect(STAGE_1_MAP_TF * 16, STAGE_1_MAP_BR * 16);
	Point p(x, y);
	if (rect.isIn(p))
		Stage = 1;
	rect = Rect(STAGE_2_MAP_TF * 16, STAGE_2_MAP_BR * 16);
	if (rect.isIn(p))
		Stage = 2;
	rect = Rect(STAGE_3_MAP_TF * 16, STAGE_3_MAP_BR * 16);
	if (rect.isIn(p))
		Stage = 3;
}

void setCam(float x, float y) {

	int cx, cy;

	if (x - (GAME_PLAY_WIDTH / 2 - 1) * BRICK_WIDTH < tf.x)
		cx = tf.x;
	else if (x + (GAME_PLAY_WIDTH / 2 + 1) * BRICK_WIDTH > br.x)
		cx = br.x - GAME_PLAY_WIDTH * BRICK_WIDTH;
	else
		cx = x - (GAME_PLAY_WIDTH / 2 - 1) * BRICK_WIDTH;

	cy = (int)(y / BRICK_HEIGHT / GAME_PLAY_HEIGHT)* BRICK_HEIGHT* GAME_PLAY_HEIGHT;

	CGame::GetInstance()->SetCamPos((int)cx,cy);
	
}

void Update(DWORD dt) {
	float cx, cy;

	coObj->clear();

	UpdateActObj();

	//coObj = &actObj;
	quadtree = CreateQuadTree(actObj);

	quadtree->Retrieve(coObj, nakiri);

	nakiri->Update(dt, coObj);

	for (int i = 0; i < screenObj.size(); i++)
	{
		screenObj.at(i)->Update(dt);
	}


	nakiri->GetPosition(cx, cy);

	updateStage(cx,cy);
	updateLimit(Stage);

	setCam(cx, cy);
}

void Render_Map() {

	float cx = CGame::GetInstance()->GetCamPos_x(), cy = CGame::GetInstance()->GetCamPos_y();
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
void updateLimit(int stage) {
	switch (stage)
	{
	case 1:
		tf = STAGE_1_MAP_TF;
		br = STAGE_1_MAP_BR;
		break;
	case 2:
		tf = STAGE_2_MAP_TF;
		br = STAGE_2_MAP_BR;
		break;
	case 3:
		tf = STAGE_3_MAP_TF;
		br = STAGE_3_MAP_BR;
	default:
		break;
	}
	tf *= BRICK_HEIGHT;
	br *= BRICK_WIDTH;
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