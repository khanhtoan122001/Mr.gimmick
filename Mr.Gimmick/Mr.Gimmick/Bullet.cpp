#include "Bullet.h"

void Bullet::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	if (animations.empty()) return;
	if (_x < x + width && x + width < _x + GAME_PLAY_WIDTH * BRICK_WIDTH && _y < y && y < _y + GAME_PLAY_HEIGHT * BRICK_HEIGHT)
	animations[0]->Render(x, y);
	//RenderBoundingBox();
}

Rect Bullet::GetBoundingBox()
{
	return Rect(Point((int)x, (int)y), this->width, this->height);
}

void Bullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + this->width;
	b = y + this->height;
}

void Bullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	dx = dy = 0;
	GameObject::Update(dt);

	x += dx;
	y += dy;
}
