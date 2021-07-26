#include "map.h"
#include "Rect.h"
#include "Brick.h"
#include "boom.h"
#include "Super_Boom.h"
#include "Worm.h"

Map* Map::_instance = nullptr;

void Map::updateObj()
{
	//listObj->clear();
	if (idMap == ID_MAP_1) {
		
		for (int i = 0; i < 8; i++)
		{
			if (tpMap_1[i]->enable) {
				listObj->push_back(tpMap_1[i]);
				tpMap_1[i]->enable = false;
			}
		}
		for (int i = 0; i < TriggTrapM1->size(); i++) {
			if (TriggTrapM1->at(i)->enable) {
				listObj->push_back(TriggTrapM1->at(i));
				TriggTrapM1->at(i)->enable = false;
			}
		}
		switch (Stage)
		{
		case 1:
		{
			for (int i = 0; i < listMoveBrick->size(); i++) {
				if (listMoveBrick->at(i)->enable) {
					listObj->push_back(listMoveBrick->at(i));
					listMoveBrick->at(i)->enable = false;
				}
			}

			Rect rect(this->tf, this->br);
			for (int i = 0; i < TriggEnemiesM1->size(); i++) {
				if (rect.isIn(TriggEnemiesM1->at(i)->GetPos())) {
					if (TriggEnemiesM1->at(i)->enable) {
						listObj->push_back(TriggEnemiesM1->at(i));
						TriggEnemiesM1->at(i)->enable = false;
					}


					if (TriggEnemiesM1->at(i)->isTrigg) {
						Trigger* trigger = TriggEnemiesM1->at(i);
						for (int j = 0; j < trigger->getEnemies()->size(); j++) {
							if (trigger->getEnemies()->at(j)->enable) {
								listObj->push_back(trigger->getEnemies()->at(j));
								trigger->getEnemies()->at(j)->enable = false;
							}
						}
					}
				}
			}
		}
		break;
		case 3:
			if (Stage3Enemies->at(0)->enable) {
				listObj->push_back(Stage3Enemies->at(0));
				Worm* worm = dynamic_cast<Worm*>(Stage3Enemies->at(0));
				worm->Appear();
				//worm->Reset();
				Stage3Enemies->at(0)->enable = false;
			}
			break;
		case 6:
			if (Stage6Enemies->at(0)->enable) {
				listObj->push_back(Stage6Enemies->at(0));
				Worm* worm = dynamic_cast<Worm*>(Stage6Enemies->at(0));
				worm->Appear();
				//worm->Reset();
				Stage6Enemies->at(0)->enable = false;
			}
			break;
		case 7:
		{
			if (boss1->enable) {
				listObj->push_back(boss1);
				boss1->enable = false;
			}

			Rect rect(this->tf, this->br);
			for (int i = 0; i < TriggEnemiesM1->size(); i++) {
				if (rect.isIn(TriggEnemiesM1->at(i)->GetPos())) {
					if (TriggEnemiesM1->at(i)->enable) {
						listObj->push_back(TriggEnemiesM1->at(i));
						TriggEnemiesM1->at(i)->enable = false;
					}


					if (TriggEnemiesM1->at(i)->isTrigg) {
						Trigger* trigger = TriggEnemiesM1->at(i);
						for (int j = 0; j < trigger->getEnemies()->size(); j++) {
							if (trigger->getEnemies()->at(j)->enable) {
								listObj->push_back(trigger->getEnemies()->at(j));
								trigger->getEnemies()->at(j)->enable = false;
							}
						}
					}
				}
			}
		}
		default:
			break;
		}
	}
}


Map::Map()
{
	this->listMoveBrick = new vector<LPGAMEOBJECT>();
	this->TriggEnemiesM1 = new vector<Trigger*>();
	this->listObj = new vector<LPGAMEOBJECT>();

	boss1 = new BossM1();
	boss1->SetPosition(2176, 160);

	Trap* _trap;
	//tp.push_back(_trap);
	//_trap = new Trap();
	//tp.push_back(_trap);
	for (int i = 0; i < 8; i++)
	{
		_trap = new Trap();
		tpMap_1.push_back(_trap);
	}
	tpMap_1[0]->SetPosition(864 * 2, 416 * 2);
	tpMap_1[1]->SetPosition(816 * 2, 416 * 2);
	tpMap_1[2]->SetPosition(1696, 1216);
	tpMap_1[3]->SetPosition(1760, 1216);
	tpMap_1[4]->SetPosition(1824, 1216);
	tpMap_1[5]->SetPosition(1888, 1216);
	tpMap_1[6]->SetPosition(2016, 1216);
	tpMap_1[7]->SetPosition(2080, 1216);

	Worm* worm;
	worm = new Worm();
	worm->SetPosition(1472 + 32, 1280 - 32);
	worm->Hide();
	worm->minx = 1472;
	worm->maxx = 1472 + 96;
	Stage3Enemies->push_back(worm);

	worm = new Worm();
	worm->SetPosition(3712, 992 - 32);
	worm->Hide();
	worm->minx = 3712;
	worm->maxx = 3712 + 64;
	Stage6Enemies->push_back(worm);

	Super_Boom* spBoom;
	spBoom = new Super_Boom();
	spBoom->SetPosition(3808, 192);
	spBoom->Hide();
	Stage7Enemies->push_back(spBoom);

	spBoom = new Super_Boom();
	spBoom->SetPosition(3616, 192);
	spBoom->Hide();
	Stage7Enemies->push_back(spBoom);

	Boom* boom;
	boom = new Boom();
	boom->SetPosition(1120, 192);
	boom->Hide();
	Stage1Enemies->push_back(boom);

	boom = new Boom();
	boom->SetPosition(1216, 288);
	boom->Hide();
	Stage1Enemies->push_back(boom);

	boom = new Boom();
	boom->SetPosition(1367, 288);
	boom->Hide();
	Stage1Enemies->push_back(boom);

	boom = new Boom();
	boom->SetPosition(1376 + 32, 288);
	boom->Hide();
	Stage1Enemies->push_back(boom);

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

void Map::updateMap(float x, float y, Point& tf, Point& br) {
	if (idMap == ID_MAP_1) 
	{
		int stage = this->Stage;
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
		this->tf = tf;
		this->br = br;
		if (stage != this->Stage) {
			MapReset();
		}
	}
	else {
		tf = MAP_2_TF;
		br = MAP_2_BR;
		tf *= BRICK_HEIGHT;
		br *= BRICK_WIDTH;
		this->tf = tf;
		this->br = br;
	}
}



void Map::MapReset()
{
	listObj->clear();
	for (int i = 0; i < listMoveBrick->size(); i++)
		listMoveBrick->at(i)->enable = true;
	for (int i = 0; i < TriggEnemiesM1->size(); i++) {
		TriggEnemiesM1->at(i)->enable = true;
		TriggEnemiesM1->at(i)->Reset();
	}
	Worm* worm = dynamic_cast<Worm*>(Stage3Enemies->at(0));
	worm->Reset();
	worm = dynamic_cast<Worm*>(Stage6Enemies->at(0));
	worm->Reset();
	for (int i = 0; i < 8; i++)
	{
		tpMap_1[i]->Reset();
	}
	for (int i = 0; i < TriggTrapM1->size(); i++) {
		TriggTrapM1->at(i)->Reset();
	}
	boss1->Reset();
}

