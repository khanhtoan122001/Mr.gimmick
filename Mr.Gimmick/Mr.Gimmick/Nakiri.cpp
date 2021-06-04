#include "Nakiri.h"
#include <algorithm>
#include <assert.h>
#include "Game.h"
#include "Quadtree.h"
#include "Brick.h"
Nakiri* Nakiri::__instance = NULL;

Nakiri* Nakiri::GetInstance()
{
	if (__instance == NULL)
		__instance = new Nakiri();
	return __instance;
}

Nakiri::Nakiri(float x, float y)
{
	style = main_c;
	untouchable = 0;

	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
}

void Nakiri::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects)
{
	GameObject::Update(dt);

	if (colliable_objects != NULL) {
		if(vy < NAKIRI_JUMP_SPEED * 1.5)
			vy += NAKIRI_GRAVITY * dt;
		else {
			dy -= vy * dt;
			vy = NAKIRI_JUMP_SPEED;
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

		if (state != NAKIRI_STATE_DIE)
			CalcPotentialCollisions(colliable_objects, coEvents);

		if (coEvents.size() == 0)
		{
			x += dx;
			y += dy;
		}
		else {
			float min_tx, min_ty, nx = 0, ny;

			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

			// block 
			x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
			y += min_ty * dy + ny * 0.4f;

			if (nx != 0) vx = 0;
			if (ny != 0) vy = 0;
		}

		x = (int)x + 0.0001f;
		y = (int)y + 0.0001f;
		for (UINT i = 0; i < coEventsResult.size(); i++) {
			LPCOLLISIONEVENT e = coEventsResult[i];
			switch (e->obj->getType())
			{
				case slide_left:
				{
					x -= 2.0f;
					break;
				}
				case slide_right:
				{
					x += 2.0f;
					break;
				}
				case trap:
				{
					StartUntouchable();
					break;
				}
				case normal_brick:
				{
					break;
				}
				default:
				{
					break;
				}
			}
		}

 		dx = dy = 0;
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

bool check(Rect r, int vx, int vy, Rect obj, int &nx, int &ny, int dt) {
	Rect lr = r + Point(vx, vy) * dt;
	if (lr.IsCollide(obj)) {
		if(vx > 0)
			nx = obj.tf.x - lr.br.x;
		if(vy >= 0)
			ny = obj.tf.y - lr.br.y;
		return true;
	}
	return false;
}

void Nakiri::Render()
{
	int ani = NAKIRI_ANI_STAND;
	if (vx == 0)
	{
		if (nx > 0) ani = NAKIRI_ANI_STAND;
		else ani = NAKIRI_ANI_STAND;
	}
	else if (vx > 0)
		ani = NAKIRI_ANI_STAND;
	else ani = NAKIRI_ANI_STAND;

	animations[0]->Render((int)x, (int)y);
	RenderBoundingBox();
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
		vx = -NAKIRI_WALKING_SPEED / 2;
		nx = -1;
		break;
	case NAKIRI_STATE_JUMP:
		vy = -NAKIRI_JUMP_SPEED;
	/*case NAKIRI_STATE_DOWN:
		vy = NAKIRI_WALKING_SPEED;
		break;
	case NAKIRI_STATE_UP:
		vy = -NAKIRI_WALKING_SPEED;
		break;*/

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
	return Rect(Point(x, y + 3), NAKIRI_WIDTH - 0.5, NAKIRI_HEIGHT - 0.5);
}
