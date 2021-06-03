#include "line.h"

Line::Line(Point x, Point y) { // ax + by = c 
	this->x = x;
	this->y = y;
	if (x.x == y.x) {
		a = 1; b = 0; c = x.x;
	}
	else if (x.y == y.y) {
		a = 0; b = 1; c = x.y;
	}
	else {
		b = -1; a = (this->x.y - this->y.y) / (this->x.x - this->y.x);
		c = this->x.y - this->x.x * (this->x.y - this->y.y) / (this->x.x - this->y.x);
		c = -c;
	}
}

Point Line::intersectingWithLine(Line l)
{
	if (l.a == 0)
		return Point(this->calcX(l.c),l.c);
	if (this->a == 0)
		return Point(l.calcX(this->c), this->c);
	if (l.b == 0)
		return Point(l.c, this->calcY(l.c));
	if (this->b == 0)
		return Point(this->c, l.calcY(this->c));
	// ax - c = y
	return Point((-l.c + this->c)/(this->a - l.a),((this->a * (-l.c) + l.a * this->c) / (this->a - l.a)));
}

float Line::calcX(float y)
{
	return this->b == 0 ? c : (c - b * y) / a;
}

float Line::calcY(float x)
{
	return this->a == 0 ? c : (c - a * x) / b;
}

bool Line::isIntersecting(Line l)
{
	if ((l.a == 0 && this->a == 0) || (l.b == 0 && this->b == 0))
		return false;
	else {
		return l.a != this->a;
	}
}
