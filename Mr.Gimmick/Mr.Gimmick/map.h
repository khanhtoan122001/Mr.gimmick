#pragma once
#define STAGE_1_MAP_TF Point(0, 0)
#define STAGE_1_MAP_BR Point(63,24)

#define STAGE_2_MAP_TF Point(32,24)
#define STAGE_2_MAP_BR Point(63,36)

#define STAGE_3_MAP_TF Point(32,37)
#define STAGE_3_MAP_BR Point(80,48)

#define STAGE_4_MAP_TF Point(81,37)
#define STAGE_4_MAP_BR Point(110,48)

#define STAGE_5_MAP_TF Point(111,37)
#define STAGE_5_MAP_BR Point(127,48)

#define STAGE_6_MAP_TF Point(112,12)
#define STAGE_6_MAP_BR Point(127,36)

#define STAGE_7_MAP_TF Point(64,0)
#define STAGE_7_MAP_BR Point(127,11)

#define SWAP_POINT_STAGE_1 Point(96,576+16)
#define SWAP_POINT_STAGE_2_3 Point(1984,864 +16)
#define SWAP_POINT_STAGE_5_6 Point(3616,1344+16)
#define SWAP_POINT_STAGE_7 Point(3904,256+16)

#include "GameObject.h"
#include "Trigger.h"

class Map
{
	static Map* _instance;
	vector<LPGAMEOBJECT>* listObj;
	vector<LPGAMEOBJECT>* listMoveBrick;
	vector<Trigger*>* listTrigg;
	void updateObj();
	Point tf, br;

public:
	vector<LPGAMEOBJECT>* Stage1Enemies = new vector<LPGAMEOBJECT>();
	vector<LPGAMEOBJECT>* Stage7Enemies = new vector<LPGAMEOBJECT>();

	Map();
	static Map* GetInstance();
	int Stage;
	void updateMapObject(vector<LPGAMEOBJECT>*);
	void AddTrigger(Trigger* t) { listTrigg->push_back(t); }
	void updateMap(float x, float y, Point& tf, Point& br);
	void MapReset();
	vector<LPGAMEOBJECT>* getList() { return listMoveBrick; };
};

