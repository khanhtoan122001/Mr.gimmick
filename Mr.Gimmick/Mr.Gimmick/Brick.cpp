#include "Brick.h"
#include "Textures.h"
#include "Game.h"
#include "Nakiri.h"

void Brick::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();
	if (animations.empty()) return;
	if (_x < x + width && x + width < _x + GAME_PLAY_WIDTH * BRICK_WIDTH && _y < y && y < _y + GAME_PLAY_HEIGHT * BRICK_HEIGHT)
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

	if (x >= maxx || x <= minx) {
		vx *= -1;
		//if (x > maxx) maxx = x;
	}
	if (y >= maxy || y <= miny)
		vy *= -1;

	GameObject::Update(dt, coObjects);

	if (this->type == move_brick) {
		x += dx;
		y += dy;
		if (coObjects != NULL) {
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			CalcPotentialCollisions(coObjects, coEvents);

			if (coEvents.size() != 0) {
				float min_tx, min_ty, nx = 0, ny;
				this->ny = 0;
				FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

				// block 
				//x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
				//y += min_ty * dy + ny * 0.4f;

				//if (nx != 0) vx = 0;
				//if (ny != 0) vy = 0;

				for (int i = 0; i < coEvents.size(); i++) {
					LPCOLLISIONEVENT e = coEvents[i];
					switch (e->obj->getType())
					{
					case main_c:
						if (e->t > 0 && e->ny != 0)
							e->obj->x += dx * 2;

						if (e->t < 0 && e->t > -1.0f) {
							if (e->nx == 0)
								e->obj->y -= 1;
							e->obj->x += dx * 2;
						}
						break;
					case g_boom:

						if (e->t > 0 && e->ny != 0)
							e->obj->x += dx * 2;

						if (e->t < 0 && e->t > -1.0f) {
							if (e->nx == 0)
								e->obj->y -= 1;
							e->obj->x += dx * 2;
						}

						break;
					default:
						break;
					}

				}
			}
		}

	}
}
