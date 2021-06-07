#pragma once
#include "GameObject.h"

#define NAKIRI_STATE_DIE 1000
#define NAKIRI_STATE_STAND 0
#define NAKIRI_STATE_WALKING_LEFT 1501
#define NAKIRI_STATE_WALKING_RIGHT 1502
#define NAKIRI_STATE_JUMP 1503
#define NAKIRI_STATE_UP 1504
#define NAKIRI_STATE_DOWN 1505

#define NAKIRI_WIDTH 16
#define NAKIRI_HEIGHT 16

#define NAKIRI_WALKING_SPEED 0.13
#define NAKIRI_JUMP_SPEED 0.43f

#define NAKIRI_ANI_STAND_RIGHT 1503
#define NAKIRI_ANI_STAND_LEFT 1505
#define NAKIRI_ANI_WALKING_LEFT 1506
#define NAKIRI_ANI_WALKING_RIGHT 1504
#define NAKIRI_ANI_JUMP_RIGHT 1507
#define NAKIRI_ANI_JUMP_LEFT 1508

#define NAKIRI_GRAVITY 0.002f
#define NAKIRI_UNTOUCHABLE_TIME 5000

class Nakiri : public GameObject
{
	static Nakiri* __instance;
	int level;
	int untouchable;
	DWORD untouchable_start;

	float start_x;			// initial position of Mario at scene
	float start_y;
public:
	static Nakiri* GetInstance();
	Nakiri(float x = 32, float y = 160);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	Point GetPos() { return Point(x, y); }

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }

	void Reset();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual Rect GetBoundingBox();
};

bool check(Rect r, int dx, int dy, Rect obj, int &nx, int &ny, int dt);
