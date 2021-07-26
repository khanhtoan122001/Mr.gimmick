#include "Trap.h"
#include "Textures.h"
#include "Game.h"
#include "Star.h"

Trap::Trap()
{
	this->AddAnimation(TRAP_NORMAL_ANI);
	this->AddAnimation(EXPLOSIVE_ANI);
	this->SetWidthHeight(28, 36);
	penetrable = true;
	type = trap;
	state = TRAP_MOVE;
}

void Trap::SetPosition(Point p)
{
	GameObject::SetPosition(p);
	start_x = p.x; start_y = p.y;
}

void Trap::SetPosition(float x, float y)
{
	GameObject::SetPosition(x, y);
	start_x = x;
	start_y = y;
}

void Trap::Reset()
{
	x = start_x;
	y = start_y;
	vx = vy = 0;
	enable = true;
}

void Trap::Fall()
{
	vy = 0.0001f;
}


void Trap::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == TRAP_EXPLOSIVE) {
		time_ex += dt;
		if (time_ex >= EXPLOSIVE_TIME) {
			x = y = -9999;
			state = TRAP_MOVE;
			vx = vy = 0;
			time_ex = 0;
		}
		return;
	}
	if (vy != 0)
		vy += GRAVITY * dt;
	dx = dy = 0;
	GameObject::Update(dt);

	if (coObjects != NULL) {
		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		CalcPotentialCollisions(coObjects, coEvents);

		if (coEvents.size() != 0) {
			float min_tx, min_ty, nx = 0, ny;
			this->ny = 0;
			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

			// block 
			x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
			y += min_ty * dy + ny * 0.4f;

			//if (nx != 0) vx = 0;
			//if (ny != 0) vy = 0;

			for (int i = 0; i < coEvents.size(); i++) {
				LPCOLLISIONEVENT e = coEvents[i];
				switch (e->obj->getType())
				{
				case main_c:
					
					break;
				case normal_brick:
					if (e->t >= 0) {
						state = TRAP_EXPLOSIVE;
					}
					break;
				case diagonal_left:
					if (e->t >= 0) {
						state = TRAP_EXPLOSIVE;
					}
					break;
				case diagonal_right:
					if (e->t >= 0) {
						state = TRAP_EXPLOSIVE;
					}
					break;
				default:
					break;
				}

			}
		}
		else {
			x += dx; y += dy;
		}
	}
}

void Trap::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	if (animations.empty()) return;
	if (_x < x + width && x + width < _x + GAME_PLAY_WIDTH * BRICK_WIDTH && _y < y && y < _y + GAME_PLAY_HEIGHT * BRICK_HEIGHT && state == TRAP_MOVE)
		animations[0]->Render(x, y);
	if (state == TRAP_EXPLOSIVE)
		animations[1]->Render(x, y);
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