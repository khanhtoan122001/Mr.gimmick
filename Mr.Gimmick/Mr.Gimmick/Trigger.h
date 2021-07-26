#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "Trap.h"
class Trigger : public Brick
{
	Trap* trap;
	vector<LPGAMEOBJECT> enemies;
public: 
	bool isTrigg = false;
	void Reset();
	void setTrap(Trap* trap) { this->trap = trap; }
	Trap* getTrap() { return trap; }
	void setEnemies(LPGAMEOBJECT e) { enemies.push_back(e); }
	Trigger();
};

