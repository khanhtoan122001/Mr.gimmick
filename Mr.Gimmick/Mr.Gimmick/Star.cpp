#include "Star.h"

void Star::Reset()
{
	state = STAR_HIDE;
	x = y = -99999;
	time_ex = 0;
	time = 0;
}

Star::Star()
{
	this->AddAnimation(CHARGE_ANI);
	this->AddAnimation(ACTIVE_ANI);
	this->AddAnimation(EXPLOSIVE_ANI);
	width = height = 32;
	type = g_star;
	Reset();
}

void Star::Render()
{
	float _x;
	float _y;
	Nakiri::GetInstance()->GetPosition(_x, _y);
	if (animations.empty()) return;

	if (state == STAR_CHARGE) {
		animations[0]->Render(_x - STAR_CHARGE_WIDTH / 2 + 16, _y - 16 - STAR_CHARGE_HEIGHT / 2, 255, -1);
	}
	if (state == STAR_INDIE) {
		animations[1]->Render(_x, _y - 32);
		//animations[0]->Render(_x - STAR_CHARGE_WIDTH / 2 + 16, _y - 16 - STAR_CHARGE_HEIGHT / 2, 0);
	}
	if (state == STAR_MOVE) {
		animations[1]->Render(x, y);
	}
	if (state == STAR_EXPLOSIVE) {
		animations[2]->Render(x, y);
	}

	//if (_x < x + width && x + width < _x + GAME_PLAY_WIDTH * BRICK_WIDTH && _y < y && y < _y + GAME_PLAY_HEIGHT * BRICK_HEIGHT)
		
	//RenderBoundingBox();
}

bool Star::isActive()
{
	Point p = CGame::GetInstance()->getCamPos();
	if (p.x <= x && x <= p.x + GAME_PLAY_WIDTH * BRICK_WIDTH
		&& p.y < y && y < p.y + GAME_PLAY_HEIGHT * BRICK_HEIGHT)
		return true;
	return false;
}

Rect Star::GetBoundingBox()
{
	return Rect(Point((int)x, (int)y), this->width, this->height);
}

void Star::Shot()
{
	state = STAR_MOVE;
	float _nx = Nakiri::GetInstance()->nx;
	float _vx = Nakiri::GetInstance()->vx;
	float _vy = Nakiri::GetInstance()->vy;
	if (_nx > 0)
		this->SetSpeed(0.25 + _vx, -0.03 + _vy);
	if (_nx < 0)
		this->SetSpeed(-0.25 + _vx, -0.03 + _vy);
}

void Star::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + this->width;
	b = y + this->height;
}

void Star::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!isActive() && state != STAR_CHARGE && state != STAR_INDIE) {
		Active = false;
		Reset();
	}
	else Active = true;
	Point p = Nakiri::GetInstance()->GetPos();
	if (state == STAR_EXPLOSIVE) {
		time_ex += dt;
		if (time_ex >= EXPLOSIVE_TIME) {
			Reset();
		}
	}
	if (state == STAR_INDIE) {
		x = p.x;
		y = p.y - 32;
	}
	
	if (state == STAR_CHARGE) {
		time += dt;
		if (time >= CHARGE_TIME) {
			state = STAR_INDIE;
			isComplete = true;
			animations[0]->Reset();
			this->SetSpeed(0.07, 0);
			time = 0;
		}
		else {
			isComplete = false;
		}
	}
	else {
		animations[0]->Reset();
		time = 0;
	}
	if (state == STAR_MOVE) {
		if (-VX_MIN < vx && vx < VX_MIN)
			time_ex += dt;
		else if (-VY_MIN < vy && vy < VY_MIN)
			time_ex += dt;
		else time_ex = 0;
		if (time_ex >= ACTIVE_TIME) {
			state = STAR_EXPLOSIVE;
			time_ex = 0;
			return;
		}
		dx = dy = 0;
		GameObject::Update(dt, coObjects);

		bool isXChange = false;
		bool isYChange = false;

		vy += GRAVITY * dt;
		float _vx = vx, _vy = vy;

		if (coObjects != NULL) {
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			CalcPotentialCollisions(coObjects, coEvents);

			if (coEvents.size() != 0) {
				float min_tx, min_ty, nx = 0, ny;
				this->ny = 0;
				FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

				// block 

				for (int i = 0; i < coEventsResult.size(); i++) {
					LPCOLLISIONEVENT e = coEventsResult[i];
					if (e->obj->getType() == main_c)
						continue;
					else if (e->obj->getType() == diagonal_left) {
						if (vx > 0)
							vx = -_vx * 0.8;
						vy = -_vy * 0.7;
					}
					else if (e->obj->getType() == diagonal_right) {
						if (vx < 0)
							vx = -_vx * 0.8;
						vy = -_vy * 0.7;
					}
					else {
						x += min_tx * dx + nx * 0.8f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
						y += min_ty * dy + ny * 0.8f;


						if (nx != 0) {
							vx = -_vx * 0.75;
						}
							
						if (ny != 0) {
							vy = -_vy * 0.75;
						}
					}
				}
			}
			else {
				x += dx;
				y += dy;
			}
		}
	}


}