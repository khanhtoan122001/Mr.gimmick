#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <vector>

#include "Sprite.h"
#include "Rect.h"
#include "line.h"
//#include "Animation.h"
#define ID_TEX_BBOX 303
#define BRICK_WIDTH 32
#define BRICK_HEIGHT 32

#define GRAVITY 0.002f

class GameObject;
typedef GameObject* LPGAMEOBJECT;

enum Style {normal_brick, diagonal_left, diagonal_right, main_c, slide_left, g_boom,
	slide_right, spike, trap, trigger_Trap, move_brick, g_cannon, g_star, thorns, up_y, trigger_Enemies, sp_boom, g_worm, teleport_map2,
	tunnel, tunnel1, tunnel1_end, tunnel1_1, tunnel1_1_end , corner_1_1, corner_1_2, tunnel2, tunnel2_end,
	tunnel3, tunnel3_end, tunnel3_1, tunnel3_1_end, tunnel4, tunnel4_end, tunnel4_1, tunnel4_1_end, tunnel5, tunnel5_end, tunnel5_1, tunnel5_1_end,
	corner_3_1, corner_3_2, corner_3_3, corner_3_4, corner_4_1, corner_4_2, corner_5_1, corner_5_2};

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

	bool enable = true;

	int width;
	int height;

	bool penetrable;

	int nx;
	int ny;

	int maxx, maxy, minx, miny;

	int state;

	DWORD dt;

	Style type;

	LPDIRECT3DTEXTURE9 texture;

	vector<LPANIMATION> animations;

public:
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetPosition(Point p) { this->x = p.x, this->y = p.y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	void SetWidthHeight(int w, int h) { this->width = w; this->height = h; }
	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
	void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }
	void SetPenetrable(bool b) { this->penetrable = b; }
	bool GetPenetrable() { return this->penetrable; }

	Line getLine() {
		if (type == diagonal_left)
			return Line(Point(x + width, y), Point(x, y + height));
		if (type == diagonal_right)
			return Line(Point(x, y), Point(x + width, y + height));
		return Line();
	}

	int GetState() { return this->state; }

	void CalcPotentialCollisions(vector<LPGAMEOBJECT>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	vector<LPCOLLISIONEVENT> SweptAABBEx(LPGAMEOBJECT coO);
	void FilterCollision(
		vector<LPCOLLISIONEVENT>& coEvents,
		vector<LPCOLLISIONEVENT>& coEventsResult,
		float& min_tx,
		float& min_ty,
		float& nx,
		float& ny);

	virtual void Render() = 0;
	virtual void SetState(int state) { this->state = state; }
	void SetStyle(Style style) { this->type = style; }
	void RenderBoundingBox();
	virtual Rect GetBoundingBox() = 0;
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b) = 0;
	Point GetPos() { return Point(x, y); }


	Style getType() { return this->type; }

	GameObject();
	~GameObject();

	void AddAnimation(int AniId);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
};

