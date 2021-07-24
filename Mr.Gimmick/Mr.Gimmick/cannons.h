#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Bullet.h"

#define CANNON_ANI 165165
#define TIME_FIRE 500
class Cannon : public GameObject
{
	int loop;
public:
	vector<LPGAMEOBJECT> bullets;
	Cannon();
	virtual void Render();
	virtual Rect GetBoundingBox();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};

