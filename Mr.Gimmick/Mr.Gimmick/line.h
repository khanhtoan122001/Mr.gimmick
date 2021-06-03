#pragma once
#include "Point.h"
class Line // ax + by = c
{
	Point x, y;
	float calcX(float y);
	float calcY(float x);
public:
	float a, b, c;
	Line(Point x, Point y);
	Line() {};
	Point intersectingWithLine(Line l);
	bool isIntersecting(Line l);
};

