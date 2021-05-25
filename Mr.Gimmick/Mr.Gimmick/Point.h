#pragma once
class Point
{
public:
	float x;
	float y;
	Point();
	Point(float _x, float _y);
	void operator= (Point a) {
		x = a.x;
		y = a.y;
	}
	void operator*= (int a) {
		x *= a;
		y *= a;
	}
	Point operator+ (Point a) {
		return Point(x + a.x, y + a.y);
	}
	Point operator- (Point a) {
		return Point(x - a.x, y - a.y);
	}
	Point operator* (int a) {
		return Point(x * a, y * a);
	}
};

