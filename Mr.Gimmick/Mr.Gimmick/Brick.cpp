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