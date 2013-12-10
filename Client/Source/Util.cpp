#include "Util.h"

Rect::Rect()
{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
}

Rect::Rect(const RECT& other)
{
	left = other.left;
	top = other.top;
	right = other.right;
	bottom = other.bottom;
}

Rect::Rect(long _1, long _2, long _3, long _4)
{
	left = _1;
	top = _2;
	right = _3;
	bottom = _4;
}

bool Rect::operator ==(const RECT& other) const
{
	return left == other.left && top == other.top && right == other.right && bottom == other.bottom;
}

bool Rect::operator !=(const RECT& other) const
{
	return left != other.left || top != other.top || right != other.right || bottom != other.bottom;
}

Point::Point()
{
	x = 0;
	y = 0;
}

Point::Point(const POINT& other)
{
	x = other.x;
	y = other.y;
}

Point::Point(long x, long y)
{
	this->x = x;
	this->y = y;
}

bool Point::operator ==(const POINT& other) const
{
	return x == other.x && y == other.y;
}

bool Point::operator !=(const POINT& other) const
{
	return x != other.x || y != other.y;
}

D3DCOLOR MixColors(D3DCOLOR color1, D3DCOLOR color2, float tween)
{
	const float antiTween = 1.0f - tween;
	int a1 = (int) (((color1 >> 24) & 0xFF));
	int r1 = (int) (((color1 >> 16) & 0xFF) * tween);
	int g1 = (int) (((color1 >> 8) & 0xFF) * tween);
	int b1 = (int) (((color1 >> 0) & 0xFF) * tween);
	int a2 = (int) (((color2 >> 24) & 0xFF));
	int r2 = (int) (((color2 >> 16) & 0xFF) * antiTween);
	int g2 = (int) (((color2 >> 8) & 0xFF) * antiTween);
	int b2 = (int) (((color2 >> 0) & 0xFF) * antiTween);
	return D3DCOLOR_ARGB((a1 * a2) / 256, r1 + r2, g1 + g2, b1 + b2);
}