#pragma once

#include "DirectX.h"

class Render
{
	DirectXManager* pDirectX;

	LPD3DXFONT pSanityFont;
	LPD3DXFONT pDayTimeFont;
public:
	void Init();
	void UnInit();

	void RenderUI();
	void RenderWorld();
};