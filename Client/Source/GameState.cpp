#include "GameState.h"

#include "Game.h"
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
	delete child;
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

InGameState::InGameState(GameState* parent, Renderer* renderer, int startDay)
: GameState(parent, renderer), engine(new Engine), world(new GameWorld), player(new Player(renderer, engine.get())), gameTime(0.0f), startDay(startDay), showFPS(false)
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
	Rect sanityGoalRect;
	Rect fpsRect(0, 0, 1000, 600);

	auto uiLargeFont = renderer->GetFont(L"UILarge");
	auto uiMediumFont = renderer->GetFont(L"UIMedium");
	auto uiSmallFont = renderer->GetFont(L"UISmall");

	uiLargeFont->DrawText(nullptr, L"99", -1, &sanityRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiLargeFont->DrawText(nullptr, L"+99", -1, &sanityUpdateRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiMediumFont->DrawText(nullptr, L"12:00 am", -1, &timeRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiMediumFont->DrawText(nullptr, L"999", -1, &dayRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiSmallFont->DrawText(nullptr, L"Time", -1, &timeLabelRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiSmallFont->DrawText(nullptr, L"Day", -1, &dayLabelRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiSmallFont->DrawText(nullptr, L"8", -1, &spellCostRect, DT_CALCRECT | DT_SINGLELINE, 0);
	uiSmallFont->DrawText(nullptr, L"/100", -1, &sanityGoalRect, DT_CALCRECT | DT_SINGLELINE, 0);

	sanityRect = Rect(60, viewport.Height - sanityRect.bottom - 10, 60 + sanityRect.right, viewport.Height - 10);
	sanityUpdateRect = Rect(sanityRect.right, sanityRect.top, sanityRect.right + sanityUpdateRect.right, sanityRect.bottom);
	timeRect = Rect(viewport.Width - timeRect.right - 10, viewport.Height - timeRect.bottom - 10, viewport.Width - 10, viewport.Height - 10);
	dayRect = Rect(timeRect.left - dayRect.right - 10, timeRect.top, timeRect.left - 10, timeRect.bottom);
	timeLabelRect = Rect(timeRect.left, timeRect.top - timeLabelRect.bottom, timeRect.right, timeRect.top);
	dayLabelRect = Rect(dayRect.left, dayRect.top - dayLabelRect.bottom, dayRect.right, dayRect.top);
	spellCostRect = Rect(viewport.Width / 2 + 30 - spellCostRect.right, viewport.Height - 20 - spellCostRect.bottom, viewport.Width / 2 + 32, viewport.Height - 20);
	sanityGoalRect = Rect(sanityRect.right + 10, viewport.Height - 20 - sanityGoalRect.bottom, viewport.Width, viewport.Height);

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
		UITextElement(L"SanityGoal", uiSmallFont, L"/100", sanityGoalRect, 0xFFFFFFFF, DT_SINGLELINE),
		UITextElement(L"FPS", uiSmallFont, L"1000", fpsRect, 0xFFFFFFFF, DT_SINGLELINE),
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

	renderer->RemoveUIObject(uiElements[L"FPS"]);

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

	uiElements[L"Background"] = uiBackgroundTexture;
	uiElements[L"SanityIcon"] = sanityTexture;
	uiElements[L"SpellBorder"] = spellBorderTexture;
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
{
	world.reset();
	player.reset();

	engine->UnInit();

	for (auto element : uiElements)
	{
		renderer->RemoveUIObject(element.second);
		delete element.second;
	}

	renderer->SetPhysicsDraw(nullptr, nullptr);
}

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

	if (directX->IsKeyPressed(DIK_ESCAPE))
	{
		EnterState(new InGamePausedState(this, renderer));
	}
	if (directX->IsKeyPressed(DIK_F2))
	{
		showFPS = !showFPS;
		if (showFPS)
			renderer->AddUIObject(uiElements[L"FPS"]);
		else
			renderer->RemoveUIObject(uiElements[L"FPS"]);
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
	reinterpret_cast<TextRenderObject*>(uiElements[L"FPS"])->SetText(to_wstring(g_pGameClient->GetFPS()));

	if (child != nullptr)
	{
		child->Update(timeElapsed);
		return;
	}

	gameTime += timeElapsed;

	engine->Update(timeElapsed);

	D3DXVECTOR3 playerPosition;
	player->GetPosition(&playerPosition);

	D3DXVECTOR2 cameraPosition(playerPosition.x, playerPosition.y);
	renderer->SetCameraPosition(cameraPosition);

	int minutesSince730am = int(GetGameTime() * 10.0f);

	int hoursPassed = startDay * 24 + int(float(minutesSince730am + 30) / 60.0f);

	int day = (hoursPassed + 7) / 24 + 1;
	int hour = (hoursPassed + 7) % 24;
	int minute = (minutesSince730am + 30) % 60;

	if (player->GetSanity() == 0 || player->GetSanity() >= 100)
	{
		GameState* parent = this->parent;
		Renderer* renderer = this->renderer;
		int highestSanity = player->GetHighestSanity();
		int startDay = this->startDay;

		reinterpret_cast<GameClient*>(parent)->SetStartDay(day);

		parent->ExitState();
		parent->EnterState(new GameOverState(parent, renderer, day, highestSanity, hoursPassed - startDay * 24));

		return;
	}

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

GameStartState::GameStartState(GameState* parent, Renderer* renderer, int startDay)
: GameState(parent, renderer), startDay(startDay), storyText(new TextRenderObject(renderer))
{
	D3DVIEWPORT9 viewport;
	renderer->GetDirectX()->GetDevice()->GetViewport(&viewport);

	wchar_t storyBuff[512];
	const wchar_t* storyString = L"You were in an accident.  Right now you are trapped in a coma, trekking through endless, recurring nightmares.  Figments of your imagination spring forth from the ground to haunt your crippled mind.  Fight them to recover what sanity you have left and release yourself from the Nightmares\n\nDay %d";

	swprintf_s(storyBuff, storyString, startDay + 1);

	Rect storyRect(viewport.Width * 1 / 4, viewport.Height * 1 / 4, viewport.Width * 3 / 4, viewport.Height * 3 / 4);

	storyText->SetFont(renderer->GetFont(L"UISmall"));
	storyText->SetText(storyBuff);
	storyText->SetRect(storyRect);
	storyText->SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	storyText->SetFormat(DT_CENTER | DT_WORDBREAK);

	renderer->AddUIObject(storyText.get());
}

GameStartState::~GameStartState()
{
	renderer->RemoveUIObject(storyText.get());
}

void GameStartState::Input(float timeElapsed)
{
	DirectXManager* directX = renderer->GetDirectX();

	if (directX->IsKeyPressed(DIK_SPACE) || directX->IsKeyPressed(DIK_ESCAPE) || directX->IsKeyPressed(DIK_RETURN) || directX->IsKeyPressed(DIK_NUMPADENTER))
	{
		GameState* parent = this->parent;
		Renderer* renderer = this->renderer;
		int startDay = this->startDay;

		parent->ExitState();
		parent->EnterState(new InGameState(parent, renderer, startDay));
	}
}

void GameStartState::Update(float timeElapsed)
{ }

GameOverState::GameOverState(GameState* parent, Renderer* renderer, int startDay, int highestSanity, int hoursSurvived)
: GameState(parent, renderer), highestSanity(highestSanity), startDay(startDay), hoursSurvived(hoursSurvived),
gameOverText(new TextRenderObject(renderer)), descriptionText(new TextRenderObject(renderer))
{
	renderer->SetGameWorld(nullptr);

	D3DVIEWPORT9 viewport;
	renderer->GetDirectX()->GetDevice()->GetViewport(&viewport);

	const wchar_t* gameOverString = highestSanity >= 100 ? L"You Win!" : L"Game Over";
	Rect gameOverRect;
	renderer->GetFont(L"UILarge")->DrawText(nullptr, gameOverString, -1, &gameOverRect, DT_SINGLELINE | DT_CALCRECT, 0);

	gameOverRect = Rect((viewport.Width - gameOverRect.right) / 2, (viewport.Height - gameOverRect.bottom) / 2, viewport.Width, viewport.Height);

	gameOverText->SetFont(renderer->GetFont(L"UILarge"));
	gameOverText->SetText(gameOverString);
	gameOverText->SetRect(gameOverRect);
	gameOverText->SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));

	wchar_t descriptionBuff[256];
	if (highestSanity < 100)
	{
		if (hoursSurvived <= 24)
			swprintf_s(descriptionBuff, L"You survived %d %s, collecting a maximum of %d sanity", hoursSurvived, hoursSurvived == 1 ? L"hour" : L"hours", highestSanity);
		else
		{
			int days = hoursSurvived / 24;
			int hours = hoursSurvived % 24;
			const wchar_t* daysString = days == 1 ? L"day" : L"days";
			const wchar_t* hoursString = hours == 1 ? L"hour" : L"hours";
			swprintf_s(descriptionBuff, L"You survived %d %s and %d %s, collecting a maximum of %d sanity", days, daysString, hours, hoursString, highestSanity);
		}
	}
	else
	{
		swprintf_s(descriptionBuff, L"It took you %d days to find your sanity", startDay + hoursSurvived / 24);
	}
	descriptionText->SetFont(renderer->GetFont(L"UISmall"));
	descriptionText->SetText(descriptionBuff);
	descriptionText->SetRect(Rect(0, viewport.Height - 100, viewport.Width, viewport.Height));
	descriptionText->SetColor(D3DCOLOR_ARGB(255, 180, 180, 180));
	descriptionText->SetFormat(DT_CENTER);

	renderer->AddUIObject(descriptionText.get());
	renderer->AddUIObject(gameOverText.get());
}

GameOverState::~GameOverState()
{
	renderer->RemoveUIObject(gameOverText.get());
	renderer->RemoveUIObject(descriptionText.get());
}

void GameOverState::Input(float timeElapsed)
{
	DirectXManager* directX = renderer->GetDirectX();

	if (directX->IsKeyPressed(DIK_SPACE) || directX->IsKeyPressed(DIK_ESCAPE) || directX->IsKeyPressed(DIK_RETURN) || directX->IsKeyPressed(DIK_NUMPADENTER))
	{
		GameState* parent = this->parent;
		Renderer* renderer = this->renderer;
		int startDay = highestSanity >= 100 ? 0 : this->startDay;

		parent->ExitState();
		parent->EnterState(new MainMenuState(parent, renderer, startDay));
	}
}

void GameOverState::Update(float timeElapsed)
{ }

MainMenuState::MainMenuState(GameState* parent, Renderer* renderer, int startDay)
: GameState(parent, renderer), selectedMenuItem(MenuItemName::None), startDay(startDay)
{
	std::array<std::wstring, 2> itemNames = { L"Start Game", L"Quit" };

	D3DVIEWPORT9 viewport;
	renderer->GetDirectX()->GetDevice()->GetViewport(&viewport);

	Rect itemRect;
	LPD3DXFONT font = renderer->GetFont(L"UIMedium");
	font->DrawText(nullptr, itemNames[0].c_str(), -1, &itemRect, DT_SINGLELINE | DT_CALCRECT, 0);

	int itemHeight = itemRect.bottom;

	itemRect.top = (viewport.Height - itemNames.size() * itemHeight) / 2;
	itemRect.bottom = itemRect.top + itemHeight;
	itemRect.right = viewport.Width;

	int itemStartY = itemRect.top;

	for (const std::wstring& name : itemNames)
	{
		auto menuItem = new TextRenderObject(renderer);
		menuItem->SetFont(font);
		menuItem->SetRect(itemRect);
		menuItem->SetFormat(DT_SINGLELINE | DT_CENTER);
		menuItem->SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
		menuItem->SetText(name);

		uiElements[name] = menuItem;
		menuItems.push_back(menuItem);
		renderer->AddUIObject(menuItem);

		itemRect.top += itemHeight;
		itemRect.bottom += itemHeight;
	}

	auto titleTexture = new TextureRenderObject(renderer);
	titleTexture->SetTextureName(L"UI1");
	titleTexture->SetTextureClip(Rect(0, 256, 522, 256 + 110));
	titleTexture->SetPosition(D3DXVECTOR3(float(viewport.Width - 522) / 2.0f, itemStartY - 130, 0.5f));
	uiElements[L"Title"] = titleTexture;
	renderer->AddUIObject(titleTexture);

	SelectMenuItem(MenuItemName::StartGame);
}

MainMenuState::~MainMenuState()
{
	for (auto element : uiElements)
	{
		renderer->RemoveUIObject(element.second);
		delete element.second;
	}
}

void MainMenuState::SelectMenuItem(MenuItemName::Value newSelection)
{
	if (newSelection != selectedMenuItem)
	{
		if (selectedMenuItem != MenuItemName::None)
			menuItems[selectedMenuItem]->SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));

		selectedMenuItem = newSelection;

		selectedItemTween = 0.0f;
		selectedItemTweenChange = 1.5f;
	}
}

void MainMenuState::Input(float timeElapsed)
{
	if (child != nullptr)
	{
		child->Input(timeElapsed);
		return;
	}

	DirectXManager* directX = renderer->GetDirectX();

	MenuItemName::Value newSelection = selectedMenuItem;

	if (directX->IsKeyPressed(DIK_UP) || directX->IsKeyPressed(DIK_W))
		newSelection = (MenuItemName::Value) std::max<int>(0, selectedMenuItem - 1);
	else if (directX->IsKeyPressed(DIK_DOWN) || directX->IsKeyPressed(DIK_S))
		newSelection = (MenuItemName::Value) std::min<int>(menuItems.size() - 1, selectedMenuItem + 1);

	SelectMenuItem(newSelection);

	if (directX->IsKeyPressed(DIK_SPACE) || directX->IsKeyPressed(DIK_RETURN))
	{
		switch (selectedMenuItem)
		{
		case MenuItemName::StartGame:
			{
				GameState* parent = this->parent;
				Renderer* renderer = this->renderer;
				int startDay = this->startDay;
				parent->ExitState();
				parent->EnterState(new GameStartState(parent, renderer, startDay));
			}
			break;

		case MenuItemName::Quit:
			{
				reinterpret_cast<GameClient*>(parent)->Quit();
			}
			break;
		}
	}
}

void MainMenuState::Update(float timeElapsed)
{
	if (child != nullptr)
	{
		child->Update(timeElapsed);
		return;
	}

	const D3DCOLOR MaxColor = D3DCOLOR_XRGB(83, 0, 209);
	menuItems[selectedMenuItem]->SetColor(MixColors(0xFFFFFFFF, MaxColor, 0.5f * selectedItemTween));

	selectedItemTween += selectedItemTweenChange * timeElapsed;
	if (selectedItemTween <= 0.0f)
	{
		selectedItemTween = 0.0f;
		selectedItemTweenChange = -selectedItemTweenChange;
	}
	else if (selectedItemTween >= 1.0f)
	{
		selectedItemTween = 1.0f;
		selectedItemTweenChange = -selectedItemTweenChange;
	}
}