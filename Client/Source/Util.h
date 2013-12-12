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

	bool operator ==(const RECT& other) const;
	bool operator !=(const RECT& other) const;
};

struct Point : public POINT
{
	Point();
	Point(const POINT& other);
	Point(long x, long y);

	bool operator ==(const POINT& other) const;
	bool operator !=(const POINT& other) const;
};

D3DCOLOR MixColors(D3DCOLOR color1, D3DCOLOR color2, float tween);

template <typename T>
float MeasureTime(const T& function)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER begin;
	QueryPerformanceCounter(&begin);

	function();

	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);

	return float(end.QuadPart - begin.QuadPart) / float(frequency.QuadPart);
}