#include "SanityCrystal.h"

#include "Engine.h"

SanityCrystal::SanityCrystal(Renderer* pRenderer, Engine* pEngine)
	: RenderedEntity(pRenderer, pEngine, new TextureRenderObject(pRenderer))
{
	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(128, 1, 160, 43));

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.type = b2_staticBody;
	bodyDef.angle = 0.0f;

	b2PolygonShape box;
	box.SetAsBox(0.25f, 0.32f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.isSensor = true;

	SetBody(parent->GetPhysics()->CreateBody(&bodyDef));
	body->CreateFixture(&fixtureDef);
}

std::wstring SanityCrystal::GetTypeName() const
{
	return L"SanityCrystal";
}