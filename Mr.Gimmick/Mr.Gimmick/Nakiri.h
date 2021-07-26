#pragma once
#include "GameObject.h"

#define NAKIRI_STATE_DIE 1000
#define NAKIRI_STATE_STAND 0
#define NAKIRI_STATE_WALKING_LEFT 1501
#define NAKIRI_STATE_WALKING_RIGHT 1502
#define NAKIRI_STATE_JUMP 1503
#define NAKIRI_STATE_UP 1504
#define NAKIRI_STATE_DOWN 1505
#define NAKIRI_STATE_STUN 651

#define NAKIRI_WIDTH 32
#define NAKIRI_HEIGHT 32

#define NAKIRI_WALKING_SPEED 0.30
#define NAKIRI_MAX_JUMP_SPEED 0.63f
#define NAKIRI_MEDIUM_JUMP_SPEED 0.33

#define NAKIRI_ANI_STAND_RIGHT 1503
#define NAKIRI_ANI_STAND_LEFT 1505
#define NAKIRI_ANI_WALKING_LEFT 1506
#define NAKIRI_ANI_WALKING_RIGHT 1504
#define NAKIRI_ANI_JUMP_RIGHT 1507
#define NAKIRI_ANI_JUMP_LEFT 1508
#define NAKIRI_ANI_STUN_LEFT 46514
#define NAKIRI_ANI_STUN_RIGHT 98435

#define NAKIRI_GRAVITY 0.002f
#define NAKIRI_UNTOUCHABLE_TIME 5000
#define STUN_TIME 1100

class Nakiri : public GameObject
{
	static Nakiri* __instance;
	int level;
	int untouchable;
	DWORD untouchable_start;
	int stun_time = 0;


	float start_x;			// initial position of Mario at scene
	float start_y;
public:
	bool tunning = false;
	bool tunning_rev = false;
	bool canJump = true;
	static Nakiri* GetInstance();
	Nakiri();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	bool isSlip;

	Point GetPos() { return Point(x, y); }

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }

	void Reset();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual Rect GetBoundingBox();
};

bool check(Rect r, int dx, int dy, Rect obj, int &nx, int &ny, int dt);
