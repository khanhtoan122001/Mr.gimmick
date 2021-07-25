#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Nakiri.h"

#define BOOM_WIDTH 32
#define BOOM_HEIGHT 32


#define BOOM_ANI_DIE 894512
#define BOOM_ANI_WALK_RIGHT 156505
#define BOOM_ANI_WALK_LEFT 156506

#define BOOM_WALK_SPEED 0

class Boom : public GameObject
{
	int untouchable;
	DWORD untouchable_start;
	float start_x;			// initial position of Mario at scene
	float start_y;
	bool canChange = true;

public:
	Boom();
	virtual void Render();
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
};

