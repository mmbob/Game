#include "GameState.h"

#include "Renderer.h"
#include "RenderObject.h"
#include "Engine.h"
#include "GameWorld.h"
#include "WorldGenerator.h"
#include "Player.h"
#include "PhysicsDebug.h"

GameState::GameState(GameState* parent, Renderer* renderer)
: parent(parent), child(nullptr), renderer(renderer)
{ }

GameState::~GameState()
{ }

void GameState::EnterState(GameState* newState)
{
	child = newState;
}

void GameState::ExitState()
{
	child = nullptr;
}

void GameState::Input(float timeElapsed)
{
	if (child != nullptr)
		child->Input(timeElapsed);
}

void GameState::Update(float timeElapsed)
{
	if (child != nullptr)
		child->Update(timeElapsed);
}

void GameState::Render(float timeElapsed)
{
	if (child != nullptr)
		child->Render(timeElapsed);
}

GameState* GameState::GetParent() const
{
	return parent;
}

GameState* GameState::GetChild() const
{
	return child;
}

InGameState::InGameState(GameState* parent, Renderer* renderer)
: GameState(parent, renderer), engine(new Engine), world(new GameWorld), player(new Player(renderer, engine.get())), gameTime(0.0f)
{
	DirectXManager* directX = renderer->GetDirectX();

	world->Init(new WorldGenerator(this, renderer, engine.get()));
	renderer->SetGameWorld(world.get());

	D3DVIEWPORT9 viewport;
	directX->GetDevice()->GetViewport(&viewport);

	Rect sanityRect;
	Rect sanityUpdateRect;
	Rect dayRect;
	Rect timeRect;
	Rect dayLabelRect;
	Rect timeLabelRect;
	Rect spellCostRect;

	auto uiLargeFont = renderer->GetFont(L"UILarge");
	auto uiMediumFont = renderer->GetFont(L"UIMedium");
	auto uiSmallFont = renderer->GetFont(L"UISmall");

	uiLargeFont->DrawText(nullptr, L"0099", -1, &sanityRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiLargeFont->DrawText(nullptr, L"+99", -1, &sanityUpdateRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiMediumFont->DrawText(nullptr, L"12:00 am", -1, &timeRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiMediumFont->DrawText(nullptr, L"999", -1, &dayRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiSmallFont->DrawText(nullptr, L"Time", -1, &timeLabelRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiSmallFont->DrawText(nullptr, L"Day", -1, &dayLabelRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiSmallFont->DrawText(nullptr, L"8", -1, &spellCostRect, DT_CALCRECT | DT_SINGLELINE, 0);

	sanityRect = Rect(60, viewport.Height - sanityRect.bottom - 10, 60 + sanityRect.right, viewport.Height - 10);
	sanityUpdateRect = Rect(sanityRect.right, sanityRect.top, sanityRect.right + sanityUpdateRect.right, sanityRect.bottom);
	timeRect = Rect(viewport.Width - timeRect.right - 10, viewport.Height - timeRect.bottom - 10, viewport.Width - 10, viewport.Height - 10);
	dayRect = Rect(timeRect.left - dayRect.right - 10, timeRect.top, timeRect.left - 10, timeRect.bottom);
	timeLabelRect = Rect(timeRect.left, timeRect.top - timeLabelRect.bottom, timeRect.right, timeRect.top);
	dayLabelRect = Rect(dayRect.left, dayRect.top - dayLabelRect.bottom, dayRect.right, dayRect.top);
	spellCostRect = Rect(viewport.Width / 2 + 30 - spellCostRect.right, viewport.Height - 20 - spellCostRect.bottom, viewport.Width / 2 + 32, viewport.Height - 20);

	struct UITextElement
	{
		UITextElement(const std::wstring& name, LPD3DXFONT font, const std::wstring& text, Rect& textRect, D3DCOLOR textColor, DWORD textFormat)
		: name(name), font(font), text(text), textRect(textRect), textColor(textColor), textFormat(textFormat)
		{ }

		std::wstring name;
		LPD3DXFONT font;
		std::wstring text;
		Rect textRect;
		D3DCOLOR textColor;
		DWORD textFormat;
	};

	UITextElement elements[] =
	{
		UITextElement(L"SanityCount", uiLargeFont, L"", sanityRect, 0xFFFFFFFF, DT_SINGLELINE),
		UITextElement(L"SanityCountUpdate", uiLargeFont, L"", sanityUpdateRect, 0xFFFFFFFF, DT_SINGLELINE),
		UITextElement(L"Day", uiMediumFont, L"", dayRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"Time", uiMediumFont, L"", timeRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"DayLabel", uiSmallFont, L"Day", dayLabelRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"TimeLabel", uiSmallFont, L"Time", timeLabelRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"SpellCost", uiSmallFont, L"1", spellCostRect, 0xFFFFFFFF, DT_SINGLELINE),
	};

	for (UITextElement& element : elements)
	{
		auto renderObject = new TextRenderObject(renderer);
		renderObject->SetFont(element.font);
		renderObject->SetColor(element.textColor);
		renderObject->SetText(element.text);
		renderObject->SetRect(element.textRect);
		renderObject->SetFormat(element.textFormat);

		uiElements[element.name] = renderObject;
		renderer->AddUIObject(renderObject);
	}

	auto uiBackgroundTexture = new TextureRenderObject(renderer);
	uiBackgroundTexture->SetPosition(D3DXVECTOR3(0.0f, 500.0f, 0.25f));
	uiBackgroundTexture->SetTextureName(L"UI1");
	uiBackgroundTexture->SetTextureClip(Rect(0, 0, 800, 100));

	auto sanityTexture = new TextureRenderObject(renderer);
	sanityTexture->SetPosition(D3DXVECTOR3(10.0f, float(viewport.Height) - 10.0f - 46.0f, 0.2f));
	sanityTexture->SetTextureName(L"UI1");
	sanityTexture->SetTextureClip(Rect(0, 101, 32, 143));

	auto spellBorderTexture = new TextureRenderObject(renderer);
	spellBorderTexture->SetPosition(D3DXVECTOR3(float(viewport.Width - 96) / 2.0f, float(viewport.Height) - 98.0f, 0.2f));
	spellBorderTexture->SetTextureName(L"UI1");
	spellBorderTexture->SetTextureClip(Rect(832, 0, 832 + 96, 96));

	auto darknessTexture = new TextureRenderObject(renderer);
	darknessTexture->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	darknessTexture->SetTextureName(L"UI1");
	darknessTexture->SetTextureClip(Rect(0, 1023 - 500, 800, 1024));

	renderer->AddUIObject(uiBackgroundTexture);
	renderer->AddUIObject(sanityTexture);
	renderer->AddUIObject(spellBorderTexture);
	renderer->AddUIObject(darknessTexture);

	uiElements[L"Darkness"] = darknessTexture;

	for (int i = 0; i < 10; ++i)
	{
		auto spellTexture = new TextureRenderObject(renderer);
		spellTexture->SetPosition(D3DXVECTOR3(float(viewport.Width - 64) / 2.0f, float(viewport.Height) - 82.0f, 0.2f));
		spellTexture->SetTextureName(L"UI1");
		spellTexture->SetTextureClip(Rect(i * 64, 192, i * 64 + 64, 256));

		std::wstring spellTextureName = L"Spell";
		spellTextureName += '0' + (i + 1) % 10;

		uiElements[spellTextureName] = spellTexture;
	}

	renderer->AddUIObject(uiElements[L"Spell1"]);

	D3DXVECTOR3 playerPosition = D3DXVECTOR3(20.0f * WorldChunk::ChunkSize, 20.0f * WorldChunk::ChunkSize, 0.5f);

	engine->Init(world.get());
	engine->AddEntity(player.get(), EntityType::Dynamic);

	player->SetPosition(playerPosition);

	engine->SetPlayer(player.get());

	auto debugDraw = new PhysicsDebugDraw(renderer);
	engine->GetPhysics()->SetDebugDraw(debugDraw);
	renderer->SetPhysicsDraw(engine->GetPhysics(), debugDraw);
}

InGameState::~InGameState()
{ }

void InGameState::EnterState(GameState* newState)
{
	child = newState;
}

void InGameState::ExitState()
{
	delete child;
	child = nullptr;
}

void InGameState::Input(float timeElapsed)
{
	if (child != nullptr)
	{
		child->Input(timeElapsed);
		return;
	}

	DirectXManager* directX = renderer->GetDirectX();

	static int escapeCheck = 0;
	if (escapeCheck > 0)
		escapeCheck--;
	if (escapeCheck == 0 && directX->IsKeyPressed(DIK_ESCAPE))
	{
		EnterState(new InGamePausedState(this, renderer));
		escapeCheck = 30;
	}

	int playerWeapon = -1;
	if (directX->IsKeyPressed(DIK_1))
		playerWeapon = 0;
	else if (directX->IsKeyPressed(DIK_2))
		playerWeapon = 1;
	else if (directX->IsKeyPressed(DIK_3))
		playerWeapon = 2;
	else if (directX->IsKeyPressed(DIK_4))
		playerWeapon = 3;
	else if (directX->IsKeyPressed(DIK_5))
		playerWeapon = 4;

	if (playerWeapon != -1)
	{
		player->ChangeWeapon(playerWeapon);

		std::wstring spellTextureName = L"Spell";
		spellTextureName += '0' + (playerWeapon + 1) % 10;

		for (int i = 0; i < 10; ++i)
		{
			std::wstring spellTextureName = L"Spell";
			spellTextureName += '0' + (i + 1) % 10;

			renderer->RemoveUIObject(uiElements[spellTextureName]);
		}

		renderer->AddUIObject(uiElements[spellTextureName]);

		std::wstring costText;
		costText += L'0' + wchar_t(player->GetCurrentWeaponCost());

		reinterpret_cast<TextRenderObject*>(uiElements[L"SpellCost"])->SetText(costText);
	}
}

void InGameState::Update(float timeElapsed)
{
	if (child != nullptr)
	{
		child->Update(timeElapsed);
		return;
	}

	gameTime += 1.0f / 60.0f;

	engine->Update(timeElapsed);

	if (player->GetSanity() == 0)
	{
		parent->ExitState();
		parent->EnterState(new GameOverState(parent, renderer));
	}

	D3DXVECTOR3 playerPosition;
	player->GetPosition(&playerPosition);

	D3DXVECTOR2 cameraPosition(playerPosition.x, playerPosition.y);
	renderer->SetCameraPosition(cameraPosition);

	int minutesSince730am = int(GetGameTime() * 10.0f);

	int hoursPassed = int(float(minutesSince730am + 30) / 60.0f);
	int day = (hoursPassed + 7) / 24 + 1;
	int hour = (hoursPassed + 7) % 24;
	int minute = (minutesSince730am + 30) % 60;

	std::wstring suffix = (hour >= 12 ? L"pm" : L"am");

	float colorFactor;
	if (hour > 7 && hour <= 17)
		colorFactor = 0.0f;
	else if (hour > 6 && hour <= 7)
		colorFactor = 1.0f - (float(minute) / 60.0f);
	else if (hour > 17 && hour <= 18)
		colorFactor = float(minute) / 60.0f;
	else
		colorFactor = 1.0f;

	if (colorFactor > 0.5f)
		isDayTime = false;
	else
		isDayTime = true;

	const D3DCOLOR MaxColor = D3DCOLOR_XRGB(43, 69, 138);

	D3DCOLOR ambientColor = MixColors(MaxColor, 0xFFFFFFFF, colorFactor);
	renderer->SetAmbientColor(ambientColor);

	reinterpret_cast<TextureRenderObject*>(uiElements[L"Darkness"])->SetColor(D3DCOLOR_ARGB(int((0.25f + colorFactor / 2.0f) * 255.0f), 255, 255, 255));

	wchar_t timeBuff[256];
	swprintf_s(timeBuff, L"%d:%02d %s", hour != 0 ? ((hour - 1) % 12) + 1 : 12, minute, suffix.c_str());

	reinterpret_cast<TextRenderObject*>(uiElements[L"Day"])->SetText(to_wstring(day));
	reinterpret_cast<TextRenderObject*>(uiElements[L"Time"])->SetText(timeBuff);

	reinterpret_cast<TextRenderObject*>(uiElements[L"SanityCount"])->SetText(to_wstring(player->GetSanity()));
}

void InGameState::Render(float timeElapsed)
{
	if (child != nullptr)
	{
		child->Render(timeElapsed);
		return;
	}
}

Player* InGameState::GetPlayer() const
{
	return player.get();
}

float InGameState::GetGameTime() const
{
	return gameTime;
}

bool InGameState::IsDayTime() const
{
	return isDayTime;
}

InGamePausedState::InGamePausedState(GameState* parent, Renderer* renderer)
: GameState(parent, renderer), totalTimeElapsed(0.0f)
{ }

InGamePausedState::~InGamePausedState()
{ }

void InGamePausedState::Input(float timeElapsed)
{
	totalTimeElapsed += timeElapsed;

	if (totalTimeElapsed > 0.5f && renderer->GetDirectX()->IsKeyPressed(DIK_ESCAPE))
	{
		parent->ExitState();
	}
}

void InGamePausedState::Update(float timeElapsed)
{ }

GameOverState::GameOverState(GameState* parent, Renderer* renderer, int highestSanity, int hoursSurvived)
: GameState(parent, renderer), highestSanity(highestSanity), hoursSurvived(hoursSurvived)
{
	TextRenderObject*

	renderer->AddUIObject(
}

GameOverState::~GameOverState()
{ }

void GameOverState::Input(float timeElapsed)
{
	if (renderer->GetDirectX()->IsKeyPressed(DIK_SPACE))
	{
		parent->ExitState();
	}
}

void GameOverState::Update(float timeElapsed)
{ }