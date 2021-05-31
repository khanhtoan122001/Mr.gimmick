#include "Brick.h"
#include "Textures.h"
#include "Game.h"

void Brick::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	animations[0]->Render(x + _x - int(_x), y + _y - int(_y));
	//RenderBoundingBox();
}

Rect Brick::GetBoundingBox()
{
	return Rect(Point((int)x, (int)y), this->width + 0.5, this->height + 0.5);
}

void Brick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + this->width + 0.5;
	b = y + this->height + 0.5;
}