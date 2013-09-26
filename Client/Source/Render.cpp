#include "Render.h"

#include <cassert>

void Render::Init()
{
	D3DXFONT_DESC desc;
	desc.Height = 32;
	desc.Width = 0;
	desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	desc.CharSet = DEFAULT_CHARSET;
	desc.Italic = false;
	desc.Weight = FW_BOLD;
	desc.PitchAndFamily = DEFAULT_PITCH || FF_DONTCARE;
	desc.Quality = DEFAULT_QUALITY;
	wcscpy_s(desc.FaceName, L"Tahoma");

	HRESULT result = D3DXCreateFontIndirect(pDirectX->GetDevice(), &desc, &pSanityFont);

	assert(SUCCEEDED(result));

	desc.Height = 20;

	result = D3DXCreateFontIndirect(pDirectX->GetDevice(), &desc, &pDayTimeFont);

	assert(SUCCEEDED(result));
}

void Render::UnInit()
{
	pSanityFont->Release();
	pDayTimeFont->Release();
}

void Render::RenderUI()
{

}

void Render::RenderWorld()
{

}