#include "Brick.h"
#include "Textures.h"
#include "Game.h"

void Brick::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	animations[0]->Render(x + _x - int(_x), y + _y - int(_y));
	RenderBoundingBox();
}

Rect Brick::GetBoundingBox()
{
	return Rect(Point(x, y), BRICK_WIDTH + 0.05, BRICK_HEIGHT + 0.05);
}

string Brick::getType()
{
	return string("Brick");
}
void Brick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH + 0.05;
	b = y + BRICK_BBOX_HEIGHT + 0.05;
}