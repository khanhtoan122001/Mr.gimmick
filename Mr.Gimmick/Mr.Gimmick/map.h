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

#define MAP_2_TF Point(0, 0)
#define MAP_2_BR Point(127, 59)

#define SWAP_POINT_STAGE_1 Point(96, 576 + 16)
#define SWAP_POINT_STAGE_2_3 Point(1984, 864 + 16)
#define SWAP_POINT_STAGE_3 Point(1088, 1312 - 16)
#define SWAP_POINT_STAGE_5_6 Point(3616, 1344 + 16)
#define SWAP_POINT_STAGE_6 Point(3872, 1024 - 16)
#define SWAP_POINT_STAGE_7 Point(3904, 256 + 16)

#define SWAP_POINT_MAP_2 Point(128, 1312)

#define ID_MAP_1 972324
#define ID_MAP_7 180
#define ID_MAP_2 8527152

#include "GameObject.h"
#include "Trigger.h"

class Map
{
	static Map* _instance;
	vector<LPGAMEOBJECT>* listObj;
	vector<LPGAMEOBJECT>* listMoveBrick;
	vector<Trigger*>* TriggEnemiesM1;
	vector<Trigger*>* TriggTrapM1 = new vector<Trigger*>();
	void updateObj();
	Point tf, br;

public:
	vector<LPGAMEOBJECT>* Stage1Enemies = new vector<LPGAMEOBJECT>();
	vector<LPGAMEOBJECT>* Stage7Enemies = new vector<LPGAMEOBJECT>();
	vector<LPGAMEOBJECT>* Stage3Enemies = new vector<LPGAMEOBJECT>();
	vector<LPGAMEOBJECT>* Stage6Enemies = new vector<LPGAMEOBJECT>();
	int idMap;
	vector<Trap*> tpMap_1;
	bool ChangeMap = false;

	Map();
	static Map* GetInstance();
	int Stage;
	void updateMapObject(vector<LPGAMEOBJECT>*);
	void AddTrigger(Trigger* t) { if(t->type == trigger_Enemies) TriggEnemiesM1->push_back(t); else TriggTrapM1->push_back(t); }
	void updateMap(float x, float y, Point& tf, Point& br);
	void MapReset();
	vector<LPGAMEOBJECT>* getList() { return listMoveBrick; };
};

