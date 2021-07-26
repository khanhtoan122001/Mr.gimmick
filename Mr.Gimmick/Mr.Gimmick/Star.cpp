#include "Star.h"
#include "boom.h"

void Star::Reset()
{
	state = STAR_HIDE;
	x = y = -99999;
	canPress = true;
	canShot = true;
	isComplete = false;
	time_ex = 0;
	time = 0;
}

Star::Star()
{
	this->AddAnimation(CHARGE_ANI);
	this->AddAnimation(ACTIVE_ANI);
	this->AddAnimation(EXPLOSIVE_ANI);
	width = height = 32;
	penetrable = true;
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
	if (_nx == 0)
		this->SetSpeed(0.4, -0.2);
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
		Reset();
	}
	//else isComplete = false;

	Point p = Nakiri::GetInstance()->GetPos();
	if (state == STAR_EXPLOSIVE) {
		time_ex += dt;
		if (time_ex >= EXPLOSIVE_TIME) {
			Reset();
		}
	}
	if (state == STAR_INDIE) {
		x = p.x;
		y = p.y - 20;
	}
	
	if (state == STAR_CHARGE) {
		time += dt;
		if (time >= CHARGE_TIME) {
			state = STAR_INDIE;
			isComplete = true;
			animations[0]->Reset();
			//this->SetSpeed(0.07, 0);
			canShot = true;
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
		canShot = false;
		if (-VX_MIN < vx && vx < VX_MIN)
			time_ex += dt;
		else if (-VY_MIN < vy && vy < VY_MIN)
			time_ex += dt;
		else time_ex = 0;
		if (time_ex >= ACTIVE_TIME) {
			state = STAR_EXPLOSIVE;
			time_ex = 0;
		}
		dx = dy = 0;
		GameObject::Update(dt, coObjects);

		bool isXChange = false;
		bool isYChange = false;

		if (vy < STAR_MAX_SPEED_FALL * 1.3)
			vy += GRAVITY * dt;
		else {
			dy -= vy * dt;
			vy = STAR_MAX_SPEED_FALL;
			dy += vy * dt;
		}

		float _vx = vx, _vy = vy;
		float x0 = x, y0 = y;

		if (coObjects != NULL) {
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			CalcPotentialCollisions(coObjects, coEvents);

			if (coEvents.size() != 0) {
				float min_tx, min_ty, nx = 0, ny;
				this->ny = 0;
				FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

				// block 

				for (int i = 0; i < coEvents.size(); i++) {
					LPCOLLISIONEVENT e = coEvents[i];
					//if (e->t <= 0) continue;
					int style = e->obj->getType();

					if (style == main_c) {

						Nakiri* nakiri = dynamic_cast<Nakiri*>(e->obj);

						if (e->ny == 1) {
							nakiri->canJump = true;
							nakiri->x += min_tx * dx;
							nakiri->y += min_ty * dy;
						}

					}
					else if (style == diagonal_left) {
						if (vx > 0)
							vx = -_vx * 0.8;
						vy = -_vy * 0.7;
					}
					else if (style == diagonal_right) {
						if (vx < 0)
							vx = -_vx * 0.8;
						vy = -_vy * 0.7;
					}
					else if (style == g_boom) {

						Boom* boom = dynamic_cast<Boom*>(e->obj);
						
					}
					else if (style == sp_boom || style == thorns || style == tunnel1 || style == tunnel1_end || style == tunnel1_1 || style == tunnel1_1_end || style == tunnel2 || style == tunnel2_end || style == tunnel3 || style == tunnel3_end || style == tunnel3_1 || style == tunnel3_1_end || style == tunnel4 || style == tunnel4_end || style == tunnel4_1 || style == tunnel4_1_end || style == tunnel5 || style == tunnel5_end || style == tunnel5_1) {
						state = STAR_EXPLOSIVE;
						time_ex = 0;
						return;
					}
					else {
						x0 += min_tx * dx + nx * 0.8f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
						y0 += min_ty * dy + ny * 0.8f;


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
				x0 += dx;
				y0 += dy;
			}

			for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

		}

		x = x0;
		y = y0;
	}


}