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
	return Rect(Point(x, y), BRICK_WIDTH, BRICK_HEIGHT);
}

string Brick::getType()
{
	return string("Brick");
}
