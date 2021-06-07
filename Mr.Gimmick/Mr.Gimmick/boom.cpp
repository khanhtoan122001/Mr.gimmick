#include "boom.h"

Boom* Boom::__instance = NULL;

Boom* Boom::GetInstance()
{
	if (__instance == NULL)
		__instance = new Boom();
	return __instance;
}

void Boom::Render()
{
	int ani = BOOM_ANI_WALK_RIGHT;
	if (vx == 0)
	{
		if (nx > 0) ani = BOOM_ANI_WALK_RIGHT;
		else ani = BOOM_ANI_WALK_RIGHT;
	}
	else if (vx > 0)
		ani = BOOM_ANI_WALK_RIGHT;
	else ani = BOOM_ANI_WALK_RIGHT;

	animations[0]->Render((int)x, (int)y);
}

Rect Boom::GetBoundingBox()
{
	return Rect(Point(x, y), BOOM_WIDTH - 0.5, BOOM_HEIGHT - 0.5);
}

void Boom::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BOOM_WIDTH;
	b = y + BOOM_HEIGHT;
}

void Boom::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects)
{
	float _x, _y;
	Nakiri::GetInstance()->GetPosition(_x, _y);
	float dtx = _x - this->x;
	if (dtx < 0) {
		if (dtx < -GAME_PLAY_WIDTH * 16 / 4)
			vx = -BOOM_WALK_SPEED;
	}
	else {
		if (dtx > GAME_PLAY_WIDTH * 16 / 4)
			vx = BOOM_WALK_SPEED;
	}
	if (nx != 0) vx = -vx;

	GameObject::Update(dt);

	if (colliable_objects != NULL) {
		if (vy < NAKIRI_JUMP_SPEED * 1.5)
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

		/*if (state != NAKIRI_STATE_DIE)*/
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

			if (nx != 0) 
				vx = -vx;
			if (ny != 0) vy = 0;
		}



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
			case diagonal_left:
				if (dx == 0) {
					x -= NAKIRI_WALKING_SPEED * 0.1 * dt;
					y += NAKIRI_GRAVITY * dt;
				}
				else /*if (dx > 0) */ {
					y -= dx * (e->obj->height / e->obj->width);
				}
				break;
			default:
				break;
			}
		}
		x = x + 0.0001f;
		y = (int)y + 0.0001f;
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