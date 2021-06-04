#include "Rect.h"
#include "GameObject.h"
bool Rect::isIn(Point p) {
	if (p.x < tf.x || p.x > br.x)
		return false;
	if (p.y < tf.y || p.y > br.y)
		return false;
	return true;
}

bool Rect::IsCollide(Rect r) {
	if (br.x < r.tf.x || br.y < r.tf.y || tf.x > r.br.x || tf.y > r.br.y)
		return false;
	return true;
}

Rect::Rect(Point _tf, Point _br) {
	tf = _tf; br = _br;
}

Rect::Rect(Point _tf, float width, float height)
{
	tf = _tf;
	br.x = tf.x + width;
	br.y = tf.y + height;
}