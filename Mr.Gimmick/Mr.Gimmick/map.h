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



#include "GameObject.h"

class Map
{
	static Map* _instance;
	int Stage;
	vector<LPGAMEOBJECT>* listObj;
	vector<LPGAMEOBJECT>* listMoveBrick;

	void updateObj();

public:
	Map();
	static Map* GetInstance();
	void updateMapObject(vector<LPGAMEOBJECT>*);
	void updateMap(float x, float y, Point& tf, Point& br);
	vector<LPGAMEOBJECT>* getList() { return listMoveBrick; };
};

