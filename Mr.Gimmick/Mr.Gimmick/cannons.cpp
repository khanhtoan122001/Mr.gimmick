#include "cannons.h"

Cannon::Cannon()
{
	bullets = vector<LPGAMEOBJECT>();
	type = g_cannon;
	loop = 0;
}
void Cannon::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	if (animations.empty()) return;
	if (_x < x + width && x + width < _x + GAME_PLAY_WIDTH * BRICK_WIDTH && _y < y && y < _y + GAME_PLAY_HEIGHT * BRICK_HEIGHT)
		animations[0]->Render(x, y);

	//RenderBoundingBox();
}

Rect Cannon::GetBoundingBox()
{
	return Rect(Point((int)x, (int)y), this->width, this->height);
}

void Cannon::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + this->width;
	b = y + this->height;
}

void Cannon::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	dx = dy = 0;

	GameObject::Update(dt, coObjects);

	vy += GRAVITY * dt;

	if (coObjects != NULL) {
		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		CalcPotentialCollisions(coObjects, coEvents);
		
		float min_tx, min_ty, nx = 0, ny;
		this->ny = 0;
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

		// block 
		x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * 0.2f;

		if (nx != 0)
			vx = 0;
		if (ny != 0)
			vy = 0;

		for (int i = 0; i < coEvents.size(); i++) {
			LPCOLLISIONEVENT e = coEvents[i];
			switch (e->obj->type)
			{
			case diagonal_left:
				if (e->t > 0)
				{
					if (dx == 0) {
						x -= GRAVITY * dt * ((float)e->obj->width / (float)e->obj->height);
						y += GRAVITY * dt;
					}
					if (dx > 0) {
						y -= 0.028 * dt;
						vx = 0.001f;
					}
					if (dx < 0) {
						ny = 0;
					}
				}
				break;
			case diagonal_right:
				if (e->t > 0) {
					if (dx == 0) {
						x += GRAVITY * dt * ((float)e->obj->width / (float)e->obj->height);
						y += GRAVITY * dt;
					}
					if (dx > 0) {
						ny = 0;
					}
					if (dx < 0) {
						y -= 0.028 * dt;
						vx = -0.001f;
					}
				}
				break;
			case main_c:
				y -= dy * min_ty;
				break;
			default:
				break;
			}
			/*if (e->obj->style == main_c) {
				//e->obj->y -= 1;
				
			}
			if (e->t < 0) {
				if (e->ny < 0) {
					dy = vy = 0;
				}
			}*/
		}
	}
	loop += dt;
	if (loop >= TIME_FIRE) {
		if (bullets.empty()) {
			Bullet* bullet = new Bullet();
			bullet->AddAnimation(BULLET_ANI);
			bullet->SetWidthHeight(32, 32);
			bullet->SetPosition(x, y);
			bullet->SetSpeed(0.05, 0);
			bullets.push_back(bullet);
		}
		loop = 0;
	}
}
