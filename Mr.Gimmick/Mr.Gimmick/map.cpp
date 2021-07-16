#include "map.h"
#include "Rect.h"
#include "Brick.h"

Map* Map::_instance = nullptr;

void Map::updateObj()
{
	switch (Stage)
	{
	case 1:
	{
	}
	default:
		break;
	}
}

Map::Map()
{
	this->listMoveBrick = new vector<LPGAMEOBJECT>();
	Brick* mbrick = new Brick();
	mbrick->SetStyle(move_brick);

	mbrick->SetWidthHeight(32, 16);
	mbrick->SetPosition(80, 256);
	mbrick->SetSpeed(0, 0.05);
	mbrick->maxx = 128;
	mbrick->minx = 64;
	mbrick->maxy = 288;
	mbrick->miny = 224;
	mbrick->AddAnimation(0);

	this->listMoveBrick->push_back(mbrick);
	Stage = 0;
}

Map* Map::GetInstance()
{
    if (_instance == nullptr) {
        _instance = new Map();
    }
    return _instance;
}

void Map::updateMap(float x, float y, Point& tf, Point& br, vector<LPGAMEOBJECT>* objs)
{
	this->listObj = objs;
	Rect rect(STAGE_1_MAP_TF * 16, STAGE_1_MAP_BR * 16);
	Point p(x, y);
	if (rect.isIn(p))
		this->Stage = 1;
	rect = Rect(STAGE_2_MAP_TF * 16, STAGE_2_MAP_BR * 16);
	if (rect.isIn(p))
		this->Stage = 2;
	rect = Rect(STAGE_3_MAP_TF * 16, STAGE_3_MAP_BR * 16);
	if (rect.isIn(p))
		this->Stage = 3;
	switch (this->Stage)
	{
	case 1:
		tf = STAGE_1_MAP_TF;
		br = STAGE_1_MAP_BR;
		break;
	case 2:
		tf = STAGE_2_MAP_TF;
		br = STAGE_2_MAP_BR;
		break;
	case 3:
		tf = STAGE_3_MAP_TF;
		br = STAGE_3_MAP_BR;
	default:
		break;
	}
	tf *= BRICK_HEIGHT;
	br *= BRICK_WIDTH;
	objs->push_back(this->listMoveBrick->at(0));
}

