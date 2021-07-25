#include "Trap.h"
#include "Textures.h"
#include "Game.h"

/*Trap::Trap()
{
	this->AddAnimation(TRAP_NORMAL);
}*/

void Trap::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt);
	x += dx;
	y += dy;
}

void Trap::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	if (animations.empty()) return;
	if (_x < x + width && x + width < _x + GAME_PLAY_WIDTH * BRICK_WIDTH && _y < y && y < _y + GAME_PLAY_HEIGHT * BRICK_HEIGHT)
		animations[0]->Render(x, y);
}

Rect Trap::GetBoundingBox()
{
	return Rect(Point((int)x, (int)y), this->width, this->height);
}

void Trap::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + this->width;
	b = y + this->height;
}