#include "Trigger.h"
#include "boom.h"

void Trigger::Reset()
{
	isTrigg = false;
	if(trap != NULL)
		trap->Reset();
	if (!enemies->empty()) {
		for (int i = 0; i < enemies->size(); i++)
		{
			int type = enemies->at(i)->getType();
			if (type == g_boom) {
				Boom* boom = dynamic_cast<Boom*>(enemies->at(i));
				boom->Reset();
				boom->Hide();
			}
		}
	}
}

void Trigger::TriggEnemies()
{
	if (!enemies->empty() && !isTrigg) {
		for (int i = 0; i < enemies->size(); i++) {
			int style = enemies->at(i)->getType();

			if (style == g_boom) {
				Boom* boom = dynamic_cast<Boom*>(enemies->at(i));
				if(boom->enable)
					boom->Reset();
			}
		}
		isTrigg = true;
	}
}

Trigger::Trigger()
{
	this->SetWidthHeight(8, 128);
	trap = NULL;
	penetrable = true;
	enemies = new vector<LPGAMEOBJECT>();
}
