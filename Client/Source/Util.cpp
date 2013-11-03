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

D3DCOLOR MixColors(D3DCOLOR color1, D3DCOLOR color2, float tween)
{
	const float antiTween = 1.0f - tween;
	int a1 = ((color1 >> 24) & 0xFF) * tween;
	int r1 = ((color1 >> 16) & 0xFF) * tween;
	int g1 = ((color1 >> 8) & 0xFF) * tween;
	int b1 = ((color1 >> 0) & 0xFF) * tween;
	int a2 = ((color2 >> 24) & 0xFF) * antiTween;
	int r2 = ((color2 >> 16) & 0xFF) * antiTween;
	int g2 = ((color2 >> 8) & 0xFF) * antiTween;
	int b2 = ((color2 >> 0) & 0xFF) * antiTween;
	return D3DCOLOR_ARGB(a1 + a2, r1 + r2, g1 + g2, b1 + b2);
}