#pragma once
#include "GameObject.h"
#include "Game.h"

#define BULLET_ANI 615165

class Bullet : public GameObject
{
public:
	virtual void Render();
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};

