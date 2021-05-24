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
#define NAKIRI_HEIGHT 19

#define NAKIRI_WALKING_SPEED 0.4
#define NAKIRI_JUMP_SPEED 0.8f

#define NAKIRI_ANI_STAND 1503
#define NAKIRI_ANI_WALKING_LEFT 1504
#define NAKIRI_ANI_WALKING_RIGHT 1505
#define NAKIRI_ANI_JUMP 1506

#define NAKIRI_GRAVITY 0.005
#define NAKIRI_UNTOUCHABLE_TIME 5000

class Nakiri : public GameObject
{
	int level;
	int untouchable;
	DWORD untouchable_start;

	float start_x;			// initial position of Mario at scene
	float start_y;
public:
	Nakiri(float x = 32, float y = 160);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	virtual string getType();

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }

	void Reset();

	virtual Rect GetBoundingBox();
};

bool check(Rect r, int dx, int dy, Rect obj, int &nx, int &ny, int dt);
