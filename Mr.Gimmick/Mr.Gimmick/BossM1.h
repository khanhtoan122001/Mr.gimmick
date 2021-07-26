#pragma once

#include "GameObject.h"
#include "Game.h"
#include "Nakiri.h"

#define BOSS_M1_ANI_STAND 4546131

class BossM1 : public GameObject
{
	int untouchable;
	DWORD untouchable_start;
	float start_x;			// initial position of Mario at scene
	float start_y;
	bool canChange = true;
	int time = 0;
	int hp = 3;
public:
	BossM1();
	virtual void Render();
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Hide();
	void Appear();
	virtual void SetPosition(Point p);
	virtual void SetPosition(float x, float y);
	void Reset();
};

