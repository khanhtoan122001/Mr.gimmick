#include "Worm.h"
#include "Star.h"
#include "boom.h"

Worm::Worm()
{
	//vx = 0.8f;
	this->AddAnimation(WORM_ANI_WALK_RIGHT);
	this->AddAnimation(WORM_ANI_WALK_LEFT);
	this->AddAnimation(WORM_ANI_DIE);
	penetrable = true;
	type = g_worm;
	this->SetWidthHeight(48, 30);
	vx = 0.08f;
	state = BOOM_STATE_NONE;
}
void Worm::SetPosition(Point p)
{
	GameObject::SetPosition(p);
	start_x = p.x; start_y = p.y;
}
void Worm::Reset()
{
	x = start_x;
	y = start_y;
	vy = 0;
	vx = 0.08f;
	enable = true;
	state = BOOM_STATE_NONE;
	//Hide();
}
void Worm::SetPosition(float x, float y)
{
	GameObject::SetPosition(x, y);
	start_x = x;
	start_y = y;
}
void Worm::Render()
{
	float _x = CGame::GetInstance()->GetCamPos_x();
	float _y = CGame::GetInstance()->GetCamPos_y();

	if (_x < x + width && x + width < _x + GAME_PLAY_WIDTH * BRICK_WIDTH && _y < y && y < _y + GAME_PLAY_HEIGHT * BRICK_HEIGHT) {
		if (vx > 0)
			animations[0]->Render((int)x, (int)y);
		else animations[1]->Render((int)x, (int)y);
	}
	else if (state == WORM_ANI_DIE)
		animations[2]->Render((int)x, (int)y);
}

Rect Worm::GetBoundingBox()
{
	return Rect(Point(x, y), BOOM_WIDTH - 0.5, BOOM_HEIGHT - 0.5);
}

void Worm::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BOOM_WIDTH;
	b = y + BOOM_HEIGHT;
}

void Worm::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects)
{
	if (x + width >= maxx || x <= minx) {
		vx *= -1;
		//if (x > maxx) maxx = x;
	}
	if (state == BOOM_STATE_DIE) {
		time += dt;
		if (time >= 500) {
			time = 0;
			vx = vy = 0;
			Hide();
		}
		GameObject::Update(dt);
		x += dx;
		y += dy;
		return;
	}
	dx = dy = 0;

	GameObject::Update(dt);

	float x0 = x, y0 = y;
	float _vx = vx, _vy = vy;

	if (colliable_objects != NULL) {
		if (vy < NAKIRI_MAX_JUMP_SPEED * 1.5)
			vy += NAKIRI_GRAVITY * dt;
		else {
			dy -= vy * dt;
			vy = NAKIRI_MAX_JUMP_SPEED;
			dy += vy * dt;
		}

		vector<LPGAMEOBJECT>* return_list = new vector<LPGAMEOBJECT>();

		//Quadtree quadtree = Creat

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		if (GetTickCount() - untouchable_start > NAKIRI_UNTOUCHABLE_TIME)
		{
			untouchable_start = 0;
			untouchable = 0;
		}

		coEvents.clear();

		/*if (state != NAKIRI_STATE_DIE)*/
		CalcPotentialCollisions(colliable_objects, coEvents);

		if (coEvents.size() == 0)
		{
			x0 += dx;
			y0 += dy;
		}
		else {
			float min_tx, min_ty, nx = 0, ny;

			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

			// block 
			x0 += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
			y0 += min_ty * dy + ny * 0.4f;

			if (nx != 0)
				vx = -_vx;
			if (ny != 0) vy = 0;

			for (UINT i = 0; i < coEvents.size(); i++) {
				LPCOLLISIONEVENT e = coEvents[i];
				int style = e->obj->getType();
				if (style == g_star || style == thorns || style == tunnel1_1 || style == tunnel1_1_end || style == tunnel2 || style == tunnel2_end || style == tunnel3 || style == tunnel3_end || style == tunnel3_1 || style == tunnel3_1_end || style == tunnel4 || style == tunnel4_end || style == tunnel4_1 || style == tunnel4_1_end || style == tunnel5_end || style == tunnel5_1)
				{
					//this->Hide();
					state = BOOM_STATE_DIE;
					vx = 0;
					return;
				}
				switch (style)
				{
				case slide_left:
				{
					if (e->t > 0 && e->ny == -1)
						x0 -= 2.0f;
					break;
				}
				case slide_right:
				{
					if (e->t > 0 && e->ny == -1)
						x0 += 2.0f;
					break;
				}
				case diagonal_left:
					if (e->t > 0)
					{
						if (dx == 0) {
							x0 -= NAKIRI_GRAVITY * dt * 2 * ((float)e->obj->width / (float)e->obj->height);
							y0 += NAKIRI_GRAVITY * dt * 2;
						}
						if (dx > 0) {
							y0 -= 0.028 * dt;
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
							x0 += NAKIRI_GRAVITY * dt * 2 * ((float)e->obj->width / (float)e->obj->height);
							y0 += NAKIRI_GRAVITY * dt * 2;
						}
						if (dx > 0) {
							ny = 0;
						}
						if (dx < 0) {
							y0 -= 0.028 * dt;
							vx = -0.001f;
						}
					}
					break;
				case main_c:
					if (e->t > 0)
					{
						Nakiri* nakiri = dynamic_cast<Nakiri*>(e->obj);

						/*if (e->t > 0 && e->ny == 1) {
							e->obj->x += dx * 2;
							e->dy -= 0.8f;
						}

						if (e->t < 0 && e->t > -1.0f) {
							if (e->nx == 0)
								e->obj->y -= 1;
							e->obj->x += dx * 2;
						}*/
					}
					break;
				case move_brick:
					if (e->t > 0) {
						if (e->nx != 0 && dx != 0)
							vx = -_vx;
					}
					break;
				case normal_brick:
					if (e->t > 0 && e->nx != 0) {
						vy = -NAKIRI_MEDIUM_JUMP_SPEED;
					}
					break;
				default:
					break;
				}

			}

		}
		x = x0; y = y0;
		/*x = x + 0.0001f;
		y = (int)y + 0.0001f;*/
		/*Rect r;
		for(int i = 0; i < return_list->size();i++){
			r = return_list->at(i)->GetBoundingBox();
			int _x, _y;
			_x = _y = 0;
			if (check(this->GetBoundingBox(), vx, vy, r, _x, _y, dt)) {
				dy = _y;
				vy = 0;
			}
		}*/
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	}
}

void Worm::Hide()
{
	x = y = -9999;
}

void Worm::Appear()
{
	x = start_x;
	y = start_y;
}
