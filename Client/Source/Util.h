#pragma once

#include <Windows.h>
#include <d3d9.h>

template <class T>
void SafeRelease(T*& p)
{
	if (p != 0)
		p->Release();
	p = 0;
}

struct Rect : public RECT
{
	Rect();
	Rect(const RECT& other);
	Rect(long left, long top, long right, long bottom);
};

D3DCOLOR MixColors(D3DCOLOR color1, D3DCOLOR color2, float tween);