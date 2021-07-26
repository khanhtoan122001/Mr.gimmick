#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Nakiri.h"

#define ACTIVE_TIME 1500
#define	CHARGE_TIME 1060
#define EXPLOSIVE_TIME 500

#define STAR_CHARGE 152
#define STAR_MOVE 852
#define STAR_EXPLOSIVE 511
#define STAR_HIDE 125
#define STAR_INDIE 857

#define STAR_CHARGE_WIDTH 238
#define STAR_CHARGE_HEIGHT 230

#define STAR_MAX_SPEED_FALL 0.3f

#define CHARGE_ANI 616506
#define ACTIVE_ANI 684124
#define MOVE_ANI 641321
#define EXPLOSIVE_ANI 985434

#define VX_MIN 0.01f
#define VY_MIN 0.25f

class Star : public GameObject
{
	int time = 0;
	int time_ex = 0;
	int count = 0;
public:
	bool isComplete = false;
	void Reset();
	bool canPress = true;
	bool canShot = true;
	Star();
	virtual void Render();
	bool isActive();
	virtual Rect GetBoundingBox();
	void Shot();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
};

