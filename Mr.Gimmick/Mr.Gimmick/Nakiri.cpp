#include "Nakiri.h"
#include <algorithm>
#include <assert.h>
#include "Game.h"

Nakiri::Nakiri(float x, float y)
{

	untouchable = 0;

	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
}

void Nakiri::Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects)
{
	GameObject::Update(dt);

	// simple fall down
	//vy += NAKIRI_GRAVITY;
	/*if (y > 288)
	{
		vy = 0; y = 288;
	}*/

	// simple screen edge collision!!!
	/*if (vx > 0 && x > 16 * 65) x = 32;
	if (vx < 0 && x < 0) x = 32;*/
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

	animations[0]->Render(x, y);
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
		vx = -NAKIRI_WALKING_SPEED;
		nx = -1;
		break;
	case NAKIRI_STATE_JUMP:
		if (y == 288.f)
			vy = -NAKIRI_JUMP_SPEED;
		break;

	case NAKIRI_STATE_DOWN:
		vy = NAKIRI_WALKING_SPEED;
		break;
	case NAKIRI_STATE_UP:
		vy = -NAKIRI_WALKING_SPEED;
		break;

	case NAKIRI_STATE_STAND:
		vx = 0;
		vy = 0;
		break;
	}
}

void Nakiri::Reset()
{
}

void Nakiri::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{

}
