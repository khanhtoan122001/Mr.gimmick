#pragma once
#include "GameObject.h"
#define TRAP_WIDTH 16
#define TRAP_HEIGH 16
class Trap : public GameObject
{
public:
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

