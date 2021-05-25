#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <vector>

#include "Sprite.h"
#include "Rect.h"
//#include "Animation.h"
#define ID_TEX_BBOX 1303
#define BRICK_WIDTH 16
#define BRICK_HEIGHT 16

class GameObject;
typedef GameObject* LPGAMEOBJECT;

enum Style {normal_brick};

struct CCollisionEvent;
typedef CCollisionEvent* LPCOLLISIONEVENT;
struct CCollisionEvent
{
	LPGAMEOBJECT obj;
	float t, nx, ny;

	float dx, dy;		// *RELATIVE* movement distance between this object and obj
	CCollisionEvent(float t, float nx, float ny, LPGAMEOBJECT obj = NULL) { this->t = t; this->nx = nx; this->ny = ny; this->obj = obj; }
	CCollisionEvent(float t, float nx, float ny, float dx = 0, float dy = 0, LPGAMEOBJECT obj = NULL)
	{
		this->t = t;
		this->nx = nx;
		this->ny = ny;
		this->dx = dx;
		this->dy = dy;
		this->obj = obj;
	}

	static bool compare(const LPCOLLISIONEVENT& a, LPCOLLISIONEVENT& b)
	{
		return a->t < b->t;
	}
};


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
	int ny;

	int state;

	Style style;

	DWORD dt;

	LPDIRECT3DTEXTURE9 texture;

	vector<LPANIMATION> animations;

public:
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
	void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }

	int GetState() { return this->state; }

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	LPCOLLISIONEVENT SweptAABBEx(LPGAMEOBJECT coO);
	void FilterCollision(
		vector<LPCOLLISIONEVENT>& coEvents,
		vector<LPCOLLISIONEVENT>& coEventsResult,
		float& min_tx,
		float& min_ty,
		float& nx,
		float& ny);

	virtual void Render() = 0;
	virtual void SetState(int state) { this->state = state; }
	void RenderBoundingBox();
	virtual Rect GetBoundingBox() = 0;
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b) = 0;

	virtual string getType() = 0;

	bool IsCollide(GameObject *i) {

	}

	GameObject();
	~GameObject();

	void AddAnimation(int AniId);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
};

