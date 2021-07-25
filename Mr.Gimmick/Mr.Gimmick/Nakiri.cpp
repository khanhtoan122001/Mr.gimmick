#include "Nakiri.h"
#include <algorithm>
#include <assert.h>
#include "Game.h"
#include "Quadtree.h"
#include "Brick.h"
#include "Trigger.h"
#include "Star.h"
#include "boom.h"
Nakiri* Nakiri::__instance = NULL;

Nakiri::Nakiri()
{
	type = main_c;
	untouchable = 0;
	isSlip = false;
	penetrable = true;
	width = NAKIRI_WIDTH;
	height = NAKIRI_HEIGHT;

	this->AddAnimation(NAKIRI_ANI_STAND_RIGHT);
	this->AddAnimation(NAKIRI_ANI_WALKING_RIGHT);
	this->AddAnimation(NAKIRI_ANI_STAND_LEFT);
	this->AddAnimation(NAKIRI_ANI_WALKING_LEFT);
	this->AddAnimation(NAKIRI_ANI_JUMP_RIGHT);
	this->AddAnimation(NAKIRI_ANI_JUMP_LEFT);
}

Nakiri* Nakiri::GetInstance()
{
	if (__instance == NULL)
		__instance = new Nakiri();
	return __instance;
}

void Nakiri::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects)
{
	dx = dy = 0;

	float x0 = x, y0 = y;

	GameObject::Update(dt);

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

		if (this->state == NAKIRI_STATE_JUMP) isSlip = false;

		coEvents.clear();

		if (state != NAKIRI_STATE_DIE)
			CalcPotentialCollisions(colliable_objects, coEvents);

		int count = 0;

		for (int i = 0; i < coEvents.size(); i++)
			if (coEvents[i]->t > 0)
				count++;

		if (count == 0)
		{
			x0 += dx;
			y0 += dy;
			if (!isSlip) {
				if (dy != 0) {
					if (dx > 0) ny = 1;
					else if (dx < 0) ny = -1;
					else ny = nx == 0 ? 1 : nx;
				}
			}
			else ny = 0;
		}
		else {
			float min_tx, min_ty, nx = 0, ny;
			this->ny = 0;
			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

			// block 
			x0 += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
			y0 += min_ty * dy + ny * 0.4f;

			if (nx != 0) vx = 0;
			if (ny != 0) vy = 0;
		}

		/*x = (int)x + 0.0001f;
		y = (int)y + 0.0001f;*/
		bool slip = false;
		for (UINT i = 0; i < coEvents.size(); i++) {
			LPCOLLISIONEVENT e = coEvents[i];

			switch (e->obj->getType())
			{
			case g_star:
			{
				Star* star = dynamic_cast<Star*>(e->obj);
				if (e->t > 0)
				{
					
					float min_tx, min_ty, nx = 0, ny;
					this->ny = 0;

					if (e->ny == -1)
					{
						
						star->penetrable = false;
						x0 = x;
						y0 = y;
						FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

						// block 
						x0 += min_tx * dx + nx * 0.6f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
						y0 += min_ty * dy + ny * 0.6f;

						if (nx != 0) vx = 0;
						if (ny != 0) vy = 0;
						star->penetrable = true;

					}
					else {
				
						star->penetrable = true;

						x0 = x;
						y0 = y;

						FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

						// block 
						x0 += min_tx * dx + nx * 0.6f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
						y0 += min_ty * dy + ny * 0.6f;

						if (nx != 0) vx = 0;
						if (ny != 0) vy = 0;
						star->penetrable = false;
					}
				}
			}

			break;
			case slide_left:
			{
				if (e->t > 0 && e->ny == -1)
					x0 -= 3.0f;
				break;
			}
			case slide_right:
			{
				if (e->t > 0 && e->ny == -1)
					x0 += 3.0f;
				break;
			}
			case trap:
			{
				if (e->t > 0)
					StartUntouchable();
				break;
			}
			case diagonal_left:
				if (e->t > 0)
				{
					slip = true;
					if (dx == 0) {
						x0 -= NAKIRI_GRAVITY * dt * ((float)e->obj->width / (float)e->obj->height);
						y0 += NAKIRI_GRAVITY * dt;
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
					slip = true;
					if (dx == 0) {
						x0 += NAKIRI_GRAVITY * dt * ((float)e->obj->width / (float)e->obj->height);
						y0 += NAKIRI_GRAVITY * dt;
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
			case move_brick:
				if (e->t > 0) {
					if (e->nx != 0 && dx != 0)
						vx = -e->nx;
				}
				break;
			case g_cannon:
				if (e->t > 0) {
					if (e->nx < 0) {
						vx = -e->nx;
						e->obj->x += 0.2;
					}
					if (e->nx > 0) {
						vx = -e->nx;
						e->obj->x -= 0.2;
					}
					if (e->ny != 0) {
						y0 += 0.5f * e->ny;
						slip = true;
					}
				}
				break;
			case trigger:
				if (e->t > 0)
				{
					Trigger* trigg = dynamic_cast<Trigger*>(e->obj);
					if (trigg->getTrap() != NULL)
						trigg->getTrap()->SetSpeed(0, 0.02);
				}
				break;
			case g_boom:
				if (e->t > 0) {
					Boom* boom = dynamic_cast<Boom*>(e->obj);
					boom->penetrable = true;
					float min_tx, min_ty, nx = 0, ny;
					this->ny = 0;

					x0 = x;
					y0 = y;

					FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

					// block 
					x0 += min_tx * dx + nx * 0.6f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
					y0 += min_ty * dy + ny * 0.6f;

					if (nx != 0) vx = 0;
					if (ny != 0) vy = 0;
					boom->penetrable = false;
				}
				break;
			default:
				break;
			}
		}
		if (coEvents.size() != 0)
			if (slip) isSlip = true;
			else isSlip = false;
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
		x = x0;
		y = y0;
	}
}

bool check(Rect r, int vx, int vy, Rect obj, int& nx, int& ny, int dt) {
	Rect lr = r + Point(vx, vy) * dt;
	if (lr.IsCollide(obj)) {
		if (vx > 0)
			nx = obj.tf.x - lr.br.x;
		if (vy >= 0)
			ny = obj.tf.y - lr.br.y;
		return true;
	}
	return false;
}

void Nakiri::Render()
{
	int ani = NAKIRI_ANI_STAND_RIGHT;
	if (ny == 0)
		if (vx == 0)
		{
			if (nx >= 0) ani = NAKIRI_ANI_STAND_RIGHT;
			else ani = NAKIRI_ANI_STAND_LEFT;
		}
		else if (vx > 0)
			ani = NAKIRI_ANI_WALKING_RIGHT;
		else ani = NAKIRI_ANI_WALKING_LEFT;
	else {
		if (ny > 0)
			ani = NAKIRI_ANI_JUMP_RIGHT;
		else
			ani = NAKIRI_ANI_JUMP_LEFT;
	}
	animations[ani - NAKIRI_ANI_STAND_RIGHT]->Render((int)x, (int)y);
	//RenderBoundingBox();
}

void Nakiri::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case NAKIRI_STATE_WALKING_RIGHT:
		vx = NAKIRI_WALKING_SPEED;
		nx = 1;
		break;
	case NAKIRI_STATE_WALKING_LEFT:
		vx = -NAKIRI_WALKING_SPEED;// / 2;
		nx = -1;
		break;
	case NAKIRI_STATE_JUMP:
		vy = -NAKIRI_MAX_JUMP_SPEED;
		break;
	case NAKIRI_STATE_STAND:
		vx = 0;
		break;
	}
}

void Nakiri::Reset()
{
}

void Nakiri::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH;
	b = y + BRICK_BBOX_HEIGHT;
}

Rect Nakiri::GetBoundingBox()
{
	return Rect(Point(x, y + 6), NAKIRI_WIDTH - 1, NAKIRI_HEIGHT - 1);
}
