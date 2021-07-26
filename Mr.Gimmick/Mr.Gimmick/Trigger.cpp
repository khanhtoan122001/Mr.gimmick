#include "Trigger.h"

void Trigger::Reset()
{
	trap->Reset();
}

Trigger::Trigger()
{
	trap = NULL;
	penetrable = true;
	enemies = vector<LPGAMEOBJECT>();
}
