#pragma once

#include "Entity.h"

class SanityCrystal : public RenderedEntity
{
public:
	SanityCrystal(Renderer* pRenderer, Engine* pEngine);

	virtual std::wstring GetTypeName() const;
};