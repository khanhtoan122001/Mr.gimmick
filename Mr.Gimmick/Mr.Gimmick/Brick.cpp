#include "Brick.h"
#include "Textures.h"
#include "Game.h"

void Brick::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	animations[0]->Render(x, y);
	//RenderBoundingBox();
}

Rect Brick::GetBoundingBox()
{
	return Rect(Point((int)x, (int)y), this->width, this->height);
}

void Brick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + this->width;
	b = y + this->height;
}

void Brick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL)
{
	dx = dy = 0;

	GameObject::Update(dt, coObjects);
	x += dx;
	y += dy;
	if (this->style == move_brick) {
		if (x >= maxx || x <= minx) {
			vx *= -1;
			//if (x > maxx) maxx = x;
		}
		if (y >= maxy || y <= miny)
			vy *= -1;
	}
}
