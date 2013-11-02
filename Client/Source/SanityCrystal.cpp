#include "SanityCrystal.h"

SanityCrystal::SanityCrystal(Renderer* pRenderer)
	: RenderedEntity(pRenderer, std::unique_ptr<IRenderObject>(new TextureRenderObject(pRenderer)))
{
	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	position = D3DXVECTOR3(400, 400, 1.0f);
	//	D3DXMatrixAffineTransformation2D(&rotation, 1.0f, nullptr, 0.0f, &D3DXVECTOR2(375, 225));
	velocity = D3DXVECTOR2(0.0f, 0.0f);

	textureObject->SetPosition(position);
	textureObject->SetTextureName(L"UI1");
	textureObject->SetTextureClip(Rect(0, 101, 32, 143));
}