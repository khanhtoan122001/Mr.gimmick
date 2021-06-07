#pragma once

#include <unordered_map>

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <dinput.h>
#include "GameObject.h"
//#include "Nakiri.h"
//#include <Scence.h>

#define DIRECTINPUT_VERSION 0x0800
#define KEYBOARD_BUFFER_SIZE 1024

#define GAME_PLAY_WIDTH 16
#define GAME_PLAY_HEIGHT 12

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256

class CKeyEventHandler
{
public:
	virtual void KeyState(BYTE* state) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
};

typedef CKeyEventHandler* LPKEYEVENTHANDLER;



class CGame
{
	static CGame* __instance;
	HWND hWnd;									// Window handle

	LPDIRECT3D9 d3d = NULL;						// Direct3D handle
	LPDIRECT3DDEVICE9 d3ddv = NULL;				// Direct3D device object

	LPDIRECT3DSURFACE9 backBuffer = NULL;
	LPD3DXSPRITE spriteHandler = NULL;			// Sprite helper library to help us draw 2D image on the screen 

	LPDIRECTINPUT8       di;		// The DirectInput object         
	LPDIRECTINPUTDEVICE8 didv;		// The keyboard device 

	BYTE  keyStates[256];			// DirectInput keyboard state buffer 
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];		// Buffered keyboard data

	LPKEYEVENTHANDLER keyHandler;

	float cam_x = 0 * 16.0f;
	float cam_y = 0 * 16.0f;

	int screen_width;
	int screen_height;

	//unordered_map<int, LPSCENE> scenes;
	int current_scene;

	/*void _ParseSection_SETTINGS(string line);
	void _ParseSection_SCENES(string line);*/

public:
	void SetKeyHandler(LPKEYEVENTHANDLER handler) { keyHandler = handler; }
	void Init(HWND hWnd);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha = 255);

	LPDIRECT3DTEXTURE9 LoadTexture(LPCWSTR texturePath);
	Point getCamPos() { return Point(cam_x, cam_y); }
	int IsKeyDown(int KeyCode);
	void InitKeyboard(LPKEYEVENTHANDLER handler);
	void ProcessKeyboard();

	void Load(LPCWSTR gameFile);
	//LPSCENE GetCurrentScene() { return scenes[current_scene]; }
	void SwitchScene(int scene_id);


	int GetScreenWidth() { return screen_width; }
	int GetScreenHeight() { return screen_height; }

	static void SweptAABB(
		float ml,			// move left 
		float mt,			// move top
		float mr,			// move right 
		float mb,			// move bottom
		float dx,			// 
		float dy,			// 
		float sl,			// static left
		float st,
		float sr,
		float sb,
		float& tx,
		float& nx,
		float& ny,
		bool penetrable);

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddv; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }
	float GetCamPos_x() { return cam_x; }
	float GetCamPos_y() { return cam_y; }

	void SetScreenWidth(int i) { screen_width = i; }
	void SetScreenHeight(int i) { screen_height = i; }



	static CGame* GetInstance();

	~CGame();
};

void swapAB(float& a, float& b);
