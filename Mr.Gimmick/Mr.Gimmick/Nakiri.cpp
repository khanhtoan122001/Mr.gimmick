#include "Nakiri.h"
#include <algorithm>
#include <assert.h>
#include "Game.h"
#include "Quadtree.h"
#include "Brick.h"
#include "Trigger.h"
#include "Star.h"
#include "boom.h"
#include <winuser.h>
#include "Tunnel.h"
#include "map.h"
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
	this->AddAnimation(NAKIRI_ANI_STUN_RIGHT);
	this->AddAnimation(NAKIRI_ANI_STUN_LEFT);
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

	if (state == NAKIRI_STATE_STUN) {
		stun_time += dt;
		if (stun_time >= STUN_TIME) {
			state = NAKIRI_STATE_STAND;
			stun_time = 0;
		}
		vx = 0;
		//return;
	}

	GameObject::Update(dt);

	if (colliable_objects != NULL) {
		if (!tunning && !tunning_rev)
		{
			if (vy < NAKIRI_MAX_JUMP_SPEED * 1.5)
				vy += NAKIRI_GRAVITY * dt;
			else {
				dy -= vy * dt;
				vy = NAKIRI_MAX_JUMP_SPEED;
				dy += vy * dt;
			}
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
					canJump = false;
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

			if (ny == -1) {
				canJump = true;
				this->count = 0;
			}

			if (tunning && nx != 0) {
				vx = 0; //clgt sao no lai la normal kìa wtf cai gi normal
			}

			if (nx != 0) // ok buoc 1 :v
				vx = 0;
			//tunning với tunning_rev khac gi nhau di len va di xuong
			if (ny != 0) vy = 0;
		}

		if (tunning && vx == 0) {
			//v cho nhanh
			// van de la tai sao no k tinh va cham kia
			//no va cham voi cai nao do roi bi vx = 0 trước khi vấp cục đấy
			// co cai nao lam vx=0 dc a`
			//bí ẩn của vũ trụ (;
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
						if (ny == -1) {
							canJump = true;
							this->count = 0;
						}
						if (nx != 0) vx = 0;
						if (ny != 0) vy = 0;
						star->penetrable = true;

					}
				}
			}

			break;
			case up_y:
				if (e->t > 0 && e->ny == 1) {
					y0 -= 32;
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
				if (e->t > 0) {
					//StartUntouchable();
					state = NAKIRI_STATE_STUN;
					//stun_time = 0;
					return;
				}
				break;
			}
			case diagonal_left:
				if (e->t > 0)
				{
					slip = true;
					if (dx == 0) {
						x0 -= NAKIRI_GRAVITY * dt * 8 * ((float)e->obj->width / (float)e->obj->height);
						y0 += NAKIRI_GRAVITY * dt * 8;
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
						x0 += NAKIRI_GRAVITY * dt * 8 * ((float)e->obj->width / (float)e->obj->height);
						y0 += NAKIRI_GRAVITY * dt * 8;
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
					slip = true;
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
			case trigger_Trap:
				if (e->t > 0)
				{
					Trigger* trigg = dynamic_cast<Trigger*>(e->obj);
					if (trigg->getTrap() != NULL)
						trigg->getTrap()->Fall();
				}
				break;
			case g_boom:
				if (e->t > 0) {
					//StartUntouchable();
					if (e->obj->state != BOOM_STATE_DIE)
						state = NAKIRI_STATE_STUN;
					//stun_time = 0;
					return;
				}
				break;
			case sp_boom:
				if (e->t > 0) {
					//StartUntouchable();
					if (e->obj->state != BOOM_STATE_DIE)
						state = NAKIRI_STATE_STUN;
					//stun_time = 0;
					return;
				}
				break;
			case g_worm:
				if (e->t > 0) {
					//StartUntouchable();
					if(e->obj->state != BOOM_STATE_DIE)
						state = NAKIRI_STATE_STUN;
					//stun_time = 0;
					return;
				}
				break;
			case thorns:
				if (e->t > 0) {
					int stage = Map::GetInstance()->Stage;
					if (stage == 1)
						this->SetPosition(SWAP_POINT_STAGE_1);
					if (stage == 2 || stage == 3)
						this->SetPosition(SWAP_POINT_STAGE_2_3);
					if (stage == 5 || stage == 6)
						this->SetPosition(SWAP_POINT_STAGE_5_6);
					if (stage == 7)
						this->SetPosition(SWAP_POINT_STAGE_7);
					this->Reset();
					Map::GetInstance()->MapReset();
					return;
				}
				break;
			case trigger_Enemies:
				if (e->t > 0) {
					Trigger* trigg = dynamic_cast<Trigger*>(e->obj);
					trigg->TriggEnemies();
				}
				break;
			/*case tunnel:
			{
				Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
				x += 4.0f;
			}
			break;*/
			case tunnel1:
				if (e->t != -1.0 && !tunning_rev)
				{
					tunning = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vx = 0.06f;
					//return; ok gg
				}
				//else 
					//tunning = false;
				break;
			case corner_1_1:
				if (e->t != -1.0)
					if (tunning)
					{
						vx = 0;
						vy = 0.3f;
					}
				break;
			case corner_1_2:
				if (e->t != -1.0)
					if (tunning_rev)
					{
						vy = 0;
						vx = -0.1f;
					}
				break;
			case tunnel1_end:
				if (e->t != -1.0 && tunning)
				{
					tunning = false;
				}
				break;
			case tunnel1_1:
				if (e->t != -1.0 && !tunning)
				{
					tunning_rev = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vy = -0.3f;
				}
				break;
			case tunnel1_1_end:
				if (e->t != -1.0 && tunning_rev)
				{
					tunning_rev = false;
				}
				break;
			case tunnel2:
				if (e->t != -1.0 && !tunning_rev)
				{
					tunning = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vy = 0.3f;
				}
				break;
			case tunnel2_end:
				if (e->t != -1.0 && tunning)
				{
					tunning = false;
				}
				break;
			case tunnel3:
				if (e->t != -1.0 && !tunning_rev)
				{
					tunning = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vx = 0.06f;
				}
				break;
			case tunnel3_1:
				if (e->t != -1.0 && !tunning)
				{
					tunning_rev = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vx = 0.06f;
				}
				break;
			case tunnel3_end:
				if (e->t != -1.0 && tunning)
				{
					tunning = false;
				}
				break;
			case tunnel3_1_end:
				if (e->t != -1.0 && tunning_rev)
				{
					tunning_rev = false;
				}
				break;
			case corner_3_1:
				if (e->t != -1.0)
					if (tunning)
					{
						vx = 0;
						vy = -0.3f;
					}
				break;
			case corner_3_2:
				if (e->t != -1.0)
					if (tunning)
					{
						vx = -0.06f;
						vy = 0;
					}
				break;
			case corner_3_3:
				if (e->t != -1.0)
					if (tunning_rev)
					{
						vx = 0;
						vy = 0.3f;
					}
				break;
			case corner_3_4:
				if (e->t != -1.0)
					if (tunning_rev)
					{
						vy = 0;
						vx = -0.06f;
					}
				break;
			case tunnel4:
				if (e->t != -1.0 && !tunning_rev)
				{
					tunning = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vy = -0.3f;
				}
				break;
			case tunnel4_end:
				if (e->t != -1.0 && tunning)
				{
					tunning = false;
				}
				break;
			case tunnel4_1:
				if (e->t != -1.0 && !tunning)
				{
					tunning_rev = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vx = -0.06f;
				}
				break;
			case tunnel4_1_end:
				if (e->t != -1.0 && tunning_rev)
				{
					tunning_rev = false;
				}
				break;
			case corner_4_1:
				if (e->t != -1.0)
					if (tunning)
					{
						vy = 0;
						vx = 0.06f;
					}
				break;
			case corner_4_2:
				if (e->t != -1.0)
					if (tunning_rev)
					{
						vx = 0;
						vy = 0.3f;
					}
				break;
			case tunnel5:
				if (e->t != -1.0 && !tunning_rev)
				{
					tunning = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vy = -0.3f;
				}
				break;
			case tunnel5_1:
				if (e->t != -1.0 && !tunning)
				{
					tunning_rev = true;
					Tunnel* tunnel = dynamic_cast<Tunnel*>(e->obj);
					vx = 0.06f;
				}
				break;
			case tunnel5_end:
				if (e->t != -1.0 && tunning)
				{
					tunning = false;
				}
				break;
			case tunnel5_1_end:
				if (e->t != -1.0 && tunning_rev)
				{
					tunning_rev = false;
				}
				break;
			case corner_5_1:
				if (e->t != -1.0)
					if (tunning)
					{
						vy = 0;
						vx = -0.06f;
					}
				break;
			case corner_5_2:
				if (e->t != -1.0)
					if (tunning_rev)
					{
						vx = 0;
						vy = 0.3f;
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
	if (state == NAKIRI_STATE_STUN) {
		
		if (ny == 0)
			if (vx == 0)
			{
				if (nx >= 0) animations[6]->Render(x - 4, y - 19);
				else animations[7]->Render(x - 3 + 32, y - 15);
			}
			else if (vx > 0)
				animations[6]->Render(x - 4, y - 19);
			else animations[7]->Render(x - 4 + 32, y - 19);
		else {
			if (ny > 0)
				animations[6]->Render(x - 4, y - 19);
			else
				animations[7]->Render(x - 4 + 32, y - 19);
		}
		return;
	}
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
	if (this->state == NAKIRI_STATE_STUN)
		return;
	int _state = this->state;
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
		if (canJump) {
			vy = -NAKIRI_MAX_JUMP_SPEED;
			this->count = 1;
		}
		else if (doubleJump) {
			if (this->count == 1) {
				vy = -NAKIRI_MAX_JUMP_SPEED;
				this->count = 2;
			}
		}
		break;
	case NAKIRI_STATE_STAND:
		{
			 if(!tunning && !tunning_rev)
				vx = 0; 
		} 
		break;
	}
}

void Nakiri::Reset()
{
	vx = vy = 0;
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
