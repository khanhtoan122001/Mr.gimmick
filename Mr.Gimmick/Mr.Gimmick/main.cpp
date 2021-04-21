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
#include <unordered_map>

CGame* game;
Nakiri* nakiri;

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
		//nakiri->SetState(NAKIRI_STATE_JUMP);
		break;
	}
}

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

	else if (game->IsKeyDown(DIK_UP))
		nakiri->SetState(NAKIRI_STATE_UP);
	else if (game->IsKeyDown(DIK_DOWN))
		nakiri->SetState(NAKIRI_STATE_DOWN);

	else if (game->IsKeyDown(DIK_LEFT))
		nakiri->SetState(NAKIRI_STATE_WALKING_LEFT);
	else
		nakiri->SetState(NAKIRI_STATE_STAND);
}

vector<vector<int>> Map;
vector<LPGAMEOBJECT> objects;
LPDIRECT3DTEXTURE9 texMap1;
int lx, ly;

#define ID_MAP_1 120
#define ID_MAP_7 180
#define ID_NAKIRI 15
#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"Gimmick"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 200)
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256

#define BRICK_WIDTH 16
#define BRICK_HEIGHT 16

#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

#define MAX_FRAME_RATE 120

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20

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
void LoadResource() {

	CTextures* textures = CTextures::GetInstance();
	textures->Add(ID_MAP_1, L"Resource//NES - Gimmick Mr Gimmick - Stage 1.png", D3DCOLOR_XRGB(215, 121, 214));
	textures->Add(ID_MAP_7, L"Resource//NES - Gimmick Mr Gimmick - Stage 7.png", D3DCOLOR_XRGB(99, 30, 100));
	textures->Add(ID_NAKIRI, L"Resource//NES - Gimmick Mr Gimmick - Yumetaro.png", D3DCOLOR_XRGB(0, 0, 255));

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
}

void LoadMap(string MapFile) {
	ifstream ifs{ MapFile };
	json jsonfile = json::parse(ifs);


	vector<vector<int>> r_map;
	vector<int> lineMap;
	r_map.push_back(jsonfile["layers"][0]["data"]);

	int w = jsonfile["layers"][0]["width"], h = jsonfile["layers"][0]["height"];
	for (int i = 0; i < r_map[0].size(); i++) {
		 {
			if (r_map[0][i] - 1 > 321)
				r_map[0][i] = 0;
			lineMap.push_back(r_map[0][i] - 1);
			if (lineMap.size() == w) {
				Map.push_back(lineMap);
				lineMap.clear();
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
		brick->AddAnimation(jsonfile["layers"][1]["objects"][i]["gid"] - 1);
		brick->SetPosition(jsonfile["layers"][1]["objects"][i]["x"], jsonfile["layers"][1]["objects"][i]["y"] - 16);
		objects.push_back(brick);
	}

}

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

void Update(DWORD dt) {
	float cx, cy;

	nakiri->Update(dt);

	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Update(dt);
	}

	nakiri->GetPosition(cx, cy);

	CGame::GetInstance()->SetCamPos(cx - 32, cy - 16 * 5);

}

void Render_Map() {

	float cx = CGame::GetInstance()->GetCamPos_x(), cy = CGame::GetInstance()->GetCamPos_y();

	int stx = int(cx / 16), sty = int(cy / 16);
	if (stx < 0) stx = 0;
	if (sty < 0) sty = 0;
	for (int y = sty; y < int(cy / 16) + 12 && y < Map.size(); y++) {
		for (int x = stx; x < int(cx /16) + 16 && x < Map[y].size(); x++) {
			LPANIMATION ani;
			if (Map[y][x] != -1) {
				ani = CAnimations::GetInstance()->Get(Map[y][x]);
				ani->Render(BRICK_HEIGHT * (x)+ cx - (int)(cx), BRICK_WIDTH * (y)+ cy - (int)(cy));
			}
		}
	}
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->Render();
	}
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