#include "SanityCrystal.h"

SanityCrystal::SanityCrystal(Renderer* pRenderer)
	: RenderedEntity(pRenderer)
{
	position = D3DXVECTOR3(400, 400, 1.0f);
	//	D3DXMatrixAffineTransformation2D(&rotation, 1.0f, nullptr, 0.0f, &D3DXVECTOR2(375, 225));
	velocity = D3DXVECTOR2(0.0f, 0.0f);

	pRenderObject->SetPosition(position);
	RECT clip = { 0, 0, -1, -1 };
	pRenderObject->SetTextureName(L"Sanity");
	pRenderObject->SetTextureClip(clip);
}