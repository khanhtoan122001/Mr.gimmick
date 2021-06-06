#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "Trap.h"
class Trigger : public Brick
{
	Trap* trap;
public: 
	void setTrap(Trap* trap) { this->trap = trap; }
	Trap* getTrap() { return trap; }
	Trigger() { Brick::Brick(); trap = NULL; }
};

