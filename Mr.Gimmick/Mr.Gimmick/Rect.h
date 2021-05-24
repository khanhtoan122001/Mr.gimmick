#pragma once
#include "Point.h"


class Rect
{
public:
	Point tf, br;
	void operator= (Rect a) {
		tf = a.tf;
		br = a.br;
	}

	Rect operator+ (Point a) {
		return Rect(tf + a, br + a);
	}

	int height() {
		return (int)( (tf.y - br.y) > 0 ? (tf.y - br.y) : -(tf.y - br.y));
	}

	int width() {
		return (int)((tf.x - br.x) > 0 ? (tf.x - br.x) : -(tf.x - br.x));
	}

	bool isIn(Point p);

	bool IsCollide(Rect r);

	Rect() {
		tf = Point();
		br = Point();
	}

	Rect(Point _tf, Point _br);

	Rect(Point _tf, int width, int height);

};

