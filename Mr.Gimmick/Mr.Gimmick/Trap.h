#pragma once
#include "GameObject.h"
#define TRAP_WIDTH 16
#define TRAP_HEIGH 32

#define TRAP_NORMAL_ANI 344

#define TRAP_MOVE 75122
#define TRAP_EXPLOSIVE 75235

class Trap : public GameObject
{
	int time_ex = 0;
	float start_x, start_y;
public:
	Trap();
	virtual void SetPosition(Point p);
	virtual void SetPosition(float x, float y);
	void Reset();
	void Fall();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

