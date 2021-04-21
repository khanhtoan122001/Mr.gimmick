#include "Textures.h"
#include "Game.h"
#include "GameObject.h"
#include <d3dx9.h>
#include <algorithm>
#include "Brick.h"
#include "Sprite.h"



GameObject::GameObject()
{
	x = y = 0;
	vx = vy = 0;
	nx = 1;
}

GameObject::~GameObject()
{

}
void GameObject::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPDIRECT3DTEXTURE9 bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	CGame::GetInstance()->Draw(x, y, bbox, rect.left, rect.top, rect.right, rect.bottom, 32);
}
void GameObject::AddAnimation(int AniId)
{
	LPANIMATION ani = CAnimations::GetInstance()->Get(AniId);
	animations.push_back(ani);
}
void GameObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	this->dt = dt;
	x += vx * dt;
	y += vy * dt;
}