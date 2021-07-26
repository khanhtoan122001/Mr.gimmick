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
		listObj = listMoveBrick;
	}
	default:
		break;
	}
}

Map::Map()
{
	this->listMoveBrick = new vector<LPGAMEOBJECT>();
	Brick* mbrick;
	mbrick = new Brick();
	mbrick->SetStyle(move_brick);

	mbrick->SetWidthHeight(64, 32);
	mbrick->AddAnimation(MOVE_BRICK_ANI);
	mbrick->SetPosition(960, 192);
	mbrick->SetSpeed(0.0, 0.1);
	mbrick->maxx = 960;
	mbrick->minx = 960;
	mbrick->maxy = 320;
	mbrick->miny = 192 - 16;
	this->listMoveBrick->push_back(mbrick);

	mbrick = new Brick();
	mbrick->SetStyle(move_brick);

	mbrick->SetWidthHeight(64, 32);
	mbrick->AddAnimation(MOVE_BRICK_ANI);
	mbrick->SetPosition(800 - 64, 192);
	mbrick->SetSpeed(0.1, 0.0);
	mbrick->maxx = 960 - 32 - 64;
	mbrick->minx = 768 - 64;
	mbrick->maxy = 192;
	mbrick->miny = 192;
	this->listMoveBrick->push_back(mbrick);

	mbrick = new Brick();
	mbrick->SetStyle(move_brick);

	mbrick->SetWidthHeight(64, 32);
	mbrick->AddAnimation(MOVE_BRICK_ANI);
	mbrick->SetPosition(704 - 64, 320);
	mbrick->SetSpeed(0.0, 0.1);
	mbrick->maxx = 704 - 64;
	mbrick->minx = 704 - 64;
	mbrick->maxy = 320;
	mbrick->miny = 192 - 16;
	this->listMoveBrick->push_back(mbrick);

	mbrick = new Brick();
	mbrick->SetStyle(move_brick);

	mbrick->SetWidthHeight(64, 32);
	mbrick->AddAnimation(MOVE_BRICK_ANI);
	mbrick->SetPosition(500 - 64, 192 + 32);
	mbrick->SetSpeed(0.1, 0.0);
	mbrick->maxx = 576 - 16 - 64;
	mbrick->minx = 448 + 16 - 64;
	mbrick->maxy = 192 + 32;
	mbrick->miny = 192 + 32;

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

void Map::updateMapObject(vector<LPGAMEOBJECT>* objs)
{
	updateObj();
	//this->listObj = objs;
	for(int i = 0; i < listObj->size(); i++)
		objs->push_back(this->listObj->at(i));
}

void Map::updateMap(float x, float y, Point& tf, Point& br)
{
	Rect rect(STAGE_1_MAP_TF * BRICK_WIDTH, STAGE_1_MAP_BR * BRICK_HEIGHT);
	Point p(x, y);
	if (rect.isIn(p))
		this->Stage = 1;
	rect = Rect(STAGE_2_MAP_TF * BRICK_WIDTH, STAGE_2_MAP_BR * BRICK_HEIGHT);
	if (rect.isIn(p))
		this->Stage = 2;
	rect = Rect(STAGE_3_MAP_TF * BRICK_WIDTH, STAGE_3_MAP_BR * BRICK_HEIGHT);
	if (rect.isIn(p))
		this->Stage = 3;
	rect = Rect(STAGE_4_MAP_TF * BRICK_WIDTH, STAGE_4_MAP_BR * BRICK_HEIGHT);
	if (rect.isIn(p))
		this->Stage = 4;
	rect = Rect(STAGE_5_MAP_TF * BRICK_WIDTH, STAGE_5_MAP_BR * BRICK_HEIGHT);
	if (rect.isIn(p))
		this->Stage = 5;
	rect = Rect(STAGE_6_MAP_TF * BRICK_WIDTH, STAGE_6_MAP_BR * BRICK_HEIGHT);
	if (rect.isIn(p))
		this->Stage = 6;
	rect = Rect(STAGE_7_MAP_TF * BRICK_WIDTH, STAGE_7_MAP_BR * BRICK_HEIGHT);
	if (rect.isIn(p))
		this->Stage = 7;
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
		break;
	case 4:
		tf = STAGE_4_MAP_TF;
		br = STAGE_4_MAP_BR;
		break;
	case 5:
		tf = STAGE_5_MAP_TF;
		br = STAGE_5_MAP_BR;
		break;
	case 6:
		tf = STAGE_6_MAP_TF;
		br = STAGE_6_MAP_BR;
		break;
	case 7:
		tf = STAGE_7_MAP_TF;
		br = STAGE_7_MAP_BR;
		break;
	default:
		break;
	}
	tf *= BRICK_HEIGHT;
	br *= BRICK_WIDTH;
}

