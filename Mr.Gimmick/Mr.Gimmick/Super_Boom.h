#pragma once

#include "GameObject.h"
#include "Game.h"
#include "Nakiri.h"

#define SP_BOOM_ANI_WALK_RIGHT 986513
#define SP_BOOM_ANI_WALK_LEFT 986387

class Super_Boom : public GameObject
{
	int untouchable;
	DWORD untouchable_start;
	float start_x;			// initial position of Mario at scene
	float start_y;
	bool canChange = true;
	int time = 0;
public:
	Super_Boom();
	virtual void Render();
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void Hide();
	virtual void SetPosition(Point p);
	virtual void SetPosition(float x, float y);
	void Reset();
};

