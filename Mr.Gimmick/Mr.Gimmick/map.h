#pragma once
#define STAGE_1_MAP_TF Point(0, 0)
#define STAGE_1_MAP_BR Point(63,24)

#define STAGE_2_MAP_TF Point(32,24)
#define STAGE_2_MAP_BR Point(63,36)

#define STAGE_3_MAP_TF Point(32,37)
#define STAGE_3_MAP_BR Point(80,48)

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

