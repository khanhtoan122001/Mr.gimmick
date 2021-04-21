#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <vector>

#include "Sprite.h"
//#include "Animation.h"
#define ID_TEX_BBOX -100
#define BRICK_WIDTH 16
#define BRICK_HEIGHT 16

class GameObject;
typedef GameObject* LPGAMEOBJECT;

struct CCollisionEvent;
typedef CCollisionEvent* LPCOLLISIONEVENT;


class GameObject
{
public:
	float x;
	float y;

	float dx;	// dx = vx*dt
	float dy;	// dy = vy*dt

	float vx;
	float vy;

	int nx;

	int state;

	DWORD dt;

	LPDIRECT3DTEXTURE9 texture;

	vector<LPANIMATION> animations;

public:
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
	void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }

	int GetState() { return this->state; }

	virtual void Render() = 0;
	virtual void SetState(int state) { this->state = state; }
	void RenderBoundingBox();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) = 0;

	GameObject();
	~GameObject();

	void AddAnimation(int AniId);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
};

