#include "Nakiri.h"
#include <algorithm>
#include <assert.h>
#include "Game.h"
#include "Quadtree.h"
#include "Brick.h"

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

	Quadtree* quadtree = CreateQuadTree(*colliable_objects);

	// simple fall down
	vy += NAKIRI_GRAVITY*dt;

	if (GetTickCount() - untouchable_start > NAKIRI_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	if (colliable_objects != NULL) {
		vector<LPGAMEOBJECT>* return_list = new vector<LPGAMEOBJECT>();
		quadtree->Retrieve(return_list, this);
		Rect r;
		for(int i = 0; i < return_list->size();i++){
			r = return_list->at(i)->GetBoundingBox();
			if (check(this->GetBoundingBox(), vx, vy, r)) {
				vx = vy = 0;
			}
		}
	}
}

bool check(Rect r, int vx, int vy, Rect obj) {
	if ((r + Point(vx, vy)).IsCollide(obj))
		return true;
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
}

string Nakiri::getType()
{
	return string("Nakiri");
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
		vy = -NAKIRI_JUMP_SPEED;
	/*case NAKIRI_STATE_DOWN:
		vy = NAKIRI_WALKING_SPEED;
		break;
	case NAKIRI_STATE_UP:
		vy = -NAKIRI_WALKING_SPEED;
		break;*/

	case NAKIRI_STATE_STAND:
		vx = 0;
		//vy = 0;
		break;
	}
}

void Nakiri::Reset()
{
}

Rect Nakiri::GetBoundingBox()
{
	return Rect(Point(x, y), NAKIRI_WIDTH, NAKIRI_HEIGHT);
}
