#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH  32
#define BRICK_BBOX_HEIGHT 32
#define MOVE_BRICK_ANI 984652
class Brick : public GameObject
{
public:
	virtual void Render();
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};