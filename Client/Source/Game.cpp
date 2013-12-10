#include "Game.h"

#include <windowsx.h>
#include <cassert>

#include "PhysicsDebug.h"
#include "SanityCrystal.h"
#include "Enemy.h"
#include "Util.h"
#include "WorldGenerator.h"

const wchar_t* GameClient::WindowClassName = L"fa3a766d-cc01-4644-98fe-fa9008e7a20d";
const wchar_t* GameClient::WindowTitle = L"Nightmares";

GameClient* g_pGameClient = nullptr;

GameClient::GameClient() : renderer(), player(&renderer, &engine), gameTime(0.0f), paused(false)
{
	srand(GetTickCount());
}

bool GameClient::InitInstance(HINSTANCE instance)
{
	HANDLE mutex = CreateMutex(nullptr, false, L"31c4b2f9-8bde-490a-91bc-fca8f4a89c42");
	if (mutex == nullptr)
		return false;

	if (GetLastError() == ERROR_ALREADY_EXISTS)		// Another game window is already open
	{
		HWND otherWindow = FindWindow(WindowClassName, nullptr);

		SetForegroundWindow(otherWindow);

		return false;
	}

	this->instance = instance;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.hInstance = instance;
	wc.lpszClassName = WindowClassName;
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WinProc;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);

	bool success = (RegisterClassEx(&wc) != 0);
	if (!success)
		return false;

	return true;
}

void GameClient::InitFonts()
{
	HDC context = GetDC(mainWindow);
	const int LargeHeight = -MulDiv(32, GetDeviceCaps(context, LOGPIXELSY), 72);
	const int MediumHeight = -MulDiv(24, GetDeviceCaps(context, LOGPIXELSY), 72);
	const int SmallHeight = -MulDiv(16, GetDeviceCaps(context, LOGPIXELSY), 72);
	ReleaseDC(mainWindow, context);

	D3DXFONT_DESC desc;
	desc.Height = LargeHeight;
	desc.Width = 0;
	desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	desc.CharSet = DEFAULT_CHARSET;
	desc.Italic = false;
	desc.Weight = FW_ULTRALIGHT;
	desc.PitchAndFamily = DEFAULT_PITCH || FF_DONTCARE;
	desc.Quality = DEFAULT_QUALITY;
	wcscpy_s(desc.FaceName, L"Tahoma");

	LPD3DXFONT uiSmallFont;
	LPD3DXFONT uiMediumFont;
	LPD3DXFONT uiLargeFont;

	HRESULT result = D3DXCreateFontIndirect(directX.GetDevice(), &desc, &uiLargeFont);
	assert(SUCCEEDED(result));

	desc.Height = MediumHeight;

	result = D3DXCreateFontIndirect(directX.GetDevice(), &desc, &uiMediumFont);
	assert(SUCCEEDED(result));

	desc.Height = SmallHeight;

	result = D3DXCreateFontIndirect(directX.GetDevice(), &desc, &uiSmallFont);
	assert(SUCCEEDED(result));

	uiLargeFont->PreloadCharacters('0', '9');
	uiLargeFont->PreloadCharacters('-', '-');
	uiLargeFont->PreloadCharacters('+', '+');

	uiMediumFont->PreloadCharacters('0', '9');
	uiMediumFont->PreloadCharacters('a', 'a');
	uiMediumFont->PreloadCharacters('p', 'p');
	uiMediumFont->PreloadCharacters('m', 'm');
	uiMediumFont->PreloadCharacters(':', ':');

	fonts[L"UILarge"] = uiLargeFont;
	fonts[L"UIMedium"] = uiMediumFont;
	fonts[L"UISmall"] = uiSmallFont;
}

void GameClient::UnInit()
{
	for (auto iter : fonts)
	{
		iter.second->Release();
	}

	for (auto iter : uiElements)
	{
		delete iter.second;
	}
}

void GameClient::CreateMainWindow(int width, int height, bool)
{
	DWORD exStyles = WS_EX_OVERLAPPEDWINDOW;
	DWORD styles = WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;

	RECT rc;
	rc.left = 0; rc.top = 0; rc.right = width; rc.bottom = height;	// fix

	AdjustWindowRectEx(&rc, styles, false, exStyles);	// check

	mainWindow = CreateWindowEx(exStyles, WindowClassName, WindowTitle, styles, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, HWND_DESKTOP, nullptr, instance, nullptr);

	GetClientRect(mainWindow, &rc);
	assert(rc.right == width && rc.bottom == height);
}

LRESULT CALLBACK GameClient::WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_ERASEBKGND:
		break;

	case WM_ACTIVATE:
		break;

	case WM_CLOSE:
		DestroyWindow(window);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(window, message, wParam, lParam);
	}

	return 0;
}

void GameClient::Init(HINSTANCE instance)
{
	InitInstance(instance);

	CreateMainWindow(ResolutionX, ResolutionY, false);

	directX.Init();
	directX.InitDeviceObjects(mainWindow, 800, 600);

	renderer.Init(&directX, mainWindow);
	renderer.SetGameArea(Rect(0, 0, ResolutionX, ResolutionY - 100));

	InitFonts();

	world.Init(new WorldGenerator(&renderer, &engine));
	renderer.SetGameWorld(&world);

	D3DVIEWPORT9 viewport;
	directX.GetDevice()->GetViewport(&viewport);

	Rect sanityRect;
	Rect sanityUpdateRect;
	Rect dayRect;
	Rect timeRect;
	Rect dayLabelRect;
	Rect timeLabelRect;
	Rect spellCostRect;

	fonts[L"UILarge"]->DrawText(nullptr, L"0099", -1, &sanityRect, DT_CALCRECT | DT_SINGLELINE, 0);
	fonts[L"UILarge"]->DrawText(nullptr, L"+99", -1, &sanityUpdateRect, DT_CALCRECT | DT_SINGLELINE, 0);
	fonts[L"UIMedium"]->DrawText(nullptr, L"12:00 am", -1, &timeRect, DT_CALCRECT | DT_SINGLELINE, 0);
	fonts[L"UIMedium"]->DrawText(nullptr, L"999", -1, &dayRect, DT_CALCRECT | DT_SINGLELINE, 0);
	fonts[L"UISmall"]->DrawText(nullptr, L"Time", -1, &timeLabelRect, DT_CALCRECT | DT_SINGLELINE, 0);
	fonts[L"UISmall"]->DrawText(nullptr, L"Day", -1, &dayLabelRect, DT_CALCRECT | DT_SINGLELINE, 0);
	fonts[L"UISmall"]->DrawText(nullptr, L"8", -1, &spellCostRect, DT_CALCRECT | DT_SINGLELINE, 0);

	sanityRect = Rect(60, viewport.Height - sanityRect.bottom - 10, 60 + sanityRect.right, viewport.Height - 10);
	sanityUpdateRect = Rect(sanityRect.right, sanityRect.top, sanityRect.right + sanityUpdateRect.right, sanityRect.bottom);
	timeRect = Rect(viewport.Width - timeRect.right - 10, viewport.Height - timeRect.bottom - 10, viewport.Width - 10, viewport.Height - 10);
	dayRect = Rect(timeRect.left - dayRect.right - 10, timeRect.top, timeRect.left - 10, timeRect.bottom);
	timeLabelRect = Rect(timeRect.left, timeRect.top - timeLabelRect.bottom, timeRect.right, timeRect.top);
	dayLabelRect = Rect(dayRect.left, dayRect.top - dayLabelRect.bottom, dayRect.right, dayRect.top);
	spellCostRect = Rect(viewport.Width / 2 + 30 - spellCostRect.right, viewport.Height - 20 - spellCostRect.bottom, viewport.Width / 2 + 32, viewport.Height - 20);

	struct UITextElement
	{
		UITextElement(const std::wstring& name, const std::wstring& fontName, const std::wstring& text, Rect& textRect, D3DCOLOR textColor, DWORD textFormat)
		: name(name), fontName(fontName), text(text), textRect(textRect), textColor(textColor), textFormat(textFormat)
		{ }

		std::wstring name;
		std::wstring fontName;
		std::wstring text;
		Rect textRect;
		D3DCOLOR textColor;
		DWORD textFormat;
	};

	UITextElement elements[] =
	{
		UITextElement(L"SanityCount", L"UILarge", L"", sanityRect, 0xFFFFFFFF, DT_SINGLELINE),
		UITextElement(L"SanityCountUpdate", L"UIMedium", L"", sanityUpdateRect, 0xFFFFFFFF, DT_SINGLELINE),
		UITextElement(L"Day", L"UIMedium", L"", dayRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"Time", L"UIMedium", L"", timeRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"DayLabel", L"UISmall", L"Day", dayLabelRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"TimeLabel", L"UISmall", L"Time", timeLabelRect, 0xFFFFFFFF, DT_SINGLELINE | DT_CENTER),
		UITextElement(L"SpellCost", L"UISmall", L"1", spellCostRect, 0xFFFFFFFF, DT_SINGLELINE),
	};

	for (UITextElement& element : elements)
	{
		auto renderObject = new TextRenderObject(&renderer);
		renderObject->SetFont(fonts[element.fontName]);
		renderObject->SetColor(element.textColor);
		renderObject->SetText(element.text);
		renderObject->SetRect(element.textRect);
		renderObject->SetFormat(element.textFormat);

		uiElements[element.name] = renderObject;
		renderer.AddUIObject(renderObject);
	}

	auto uiBackgroundTexture = new TextureRenderObject(&renderer);
	uiBackgroundTexture->SetPosition(D3DXVECTOR3(0.0f, 500.0f, 0.25f));
	uiBackgroundTexture->SetTextureName(L"UI1");
	uiBackgroundTexture->SetTextureClip(Rect(0, 0, 800, 100));

	auto sanityTexture = new TextureRenderObject(&renderer);
	sanityTexture->SetPosition(D3DXVECTOR3(10.0f, float(ResolutionY) - 10.0f - 46.0f, 0.2f));
	sanityTexture->SetTextureName(L"UI1");
	sanityTexture->SetTextureClip(Rect(0, 101, 32, 143));

	auto spellBorderTexture = new TextureRenderObject(&renderer);
	spellBorderTexture->SetPosition(D3DXVECTOR3(float(ResolutionX - 96) / 2.0f, float(ResolutionY) - 98.0f, 0.2f));
	spellBorderTexture->SetTextureName(L"UI1");
	spellBorderTexture->SetTextureClip(Rect(832, 0, 832 + 96, 96));

	auto darknessTexture = new TextureRenderObject(&renderer);
	darknessTexture->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	darknessTexture->SetTextureName(L"UI1");
	darknessTexture->SetTextureClip(Rect(0, 1023 - 500, 800, 1024));

	renderer.AddUIObject(uiBackgroundTexture);
	renderer.AddUIObject(sanityTexture);
	renderer.AddUIObject(spellBorderTexture);
	renderer.AddUIObject(darknessTexture);

	for (int i = 0; i < 10; ++i)
	{
		auto spellTexture = new TextureRenderObject(&renderer);
		spellTexture->SetPosition(D3DXVECTOR3(float(ResolutionX - 64) / 2.0f, float(ResolutionY) - 82.0f, 0.2f));
		spellTexture->SetTextureName(L"UI1");
		spellTexture->SetTextureClip(Rect(i * 64, 192, i * 64 + 64, 256));

		std::wstring spellTextureName = L"Spell";
		spellTextureName += '0' + (i + 1) % 10;

		uiElements[spellTextureName] = spellTexture;
	}

	renderer.AddUIObject(uiElements[L"Spell1"]);

	D3DXVECTOR3 playerPosition = D3DXVECTOR3(20.0f * WorldChunk::ChunkSize, 20.0f * WorldChunk::ChunkSize, 0.5f);

	engine.Init(&world);
	engine.AddEntity(&player, EntityType::Dynamic);

	player.SetPosition(playerPosition);

	engine.SetPlayer(&player);

	std::deque<Point> path;
	engine.FindPath(Point(int(playerPosition.x), int(playerPosition.y)), Point(int(playerPosition.x) + 5, int(playerPosition.y) + 5), &path);

	auto debugDraw = new PhysicsDebugDraw(&renderer);
	engine.GetPhysics()->SetDebugDraw(debugDraw);
	renderer.SetPhysicsDraw(engine.GetPhysics(), debugDraw);
}

void GameClient::Input(float)
{
	directX.Update();

	static int escapeCheck = 0;
	if (escapeCheck > 0)
		escapeCheck--;
	if (escapeCheck == 0 && directX.IsKeyPressed(DIK_ESCAPE))
	{
		paused = !paused;
		escapeCheck = 30;
	}

	int playerWeapon = -1;
	if (directX.IsKeyPressed(DIK_1))
		playerWeapon = 0;
	else if (directX.IsKeyPressed(DIK_2))
		playerWeapon = 1;
	else if (directX.IsKeyPressed(DIK_3))
		playerWeapon = 2;
	else if (directX.IsKeyPressed(DIK_4))
		playerWeapon = 3;
	else if (directX.IsKeyPressed(DIK_5))
		playerWeapon = 4;

	if (playerWeapon != -1)
	{
		player.ChangeWeapon(playerWeapon);

		std::wstring spellTextureName = L"Spell";
		spellTextureName += '0' + (playerWeapon + 1) % 10;
		
		for (int i = 0; i < 10; ++i)
		{
			std::wstring spellTextureName = L"Spell";
			spellTextureName += '0' + (i + 1) % 10;

			renderer.RemoveUIObject(uiElements[spellTextureName]);
		}

		renderer.AddUIObject(uiElements[spellTextureName]);

		std::wstring costText;
		costText += L'0' + wchar_t(player.GetCurrentWeaponCost());

		reinterpret_cast<TextRenderObject*>(uiElements[L"SpellCost"])->SetText(costText);
	}
}

void GameClient::Update(float timeElapsed)
{
	engine.Update(timeElapsed);

	D3DXVECTOR3 playerPosition;
	player.GetPosition(&playerPosition);

	D3DXVECTOR2 cameraPosition(playerPosition.x, playerPosition.y);
	renderer.SetCameraPosition(cameraPosition);

	int minutesSince730am = int(GetGameTime());

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

	const D3DCOLOR MaxColor = D3DCOLOR_XRGB(43, 69, 138);

	D3DCOLOR ambientColor = MixColors(MaxColor, 0xFFFFFFFF, colorFactor);
	renderer.SetAmbientColor(ambientColor);

	wchar_t timeBuff[256];
	swprintf_s(timeBuff, L"%d:%02d %s", hour != 0 ? ((hour - 1) % 12) + 1 : 12, minute, suffix.c_str());

	reinterpret_cast<TextRenderObject*>(uiElements[L"Day"])->SetText(to_wstring(day));
	reinterpret_cast<TextRenderObject*>(uiElements[L"Time"])->SetText(timeBuff);
	
	reinterpret_cast<TextRenderObject*>(uiElements[L"SanityCount"])->SetText(to_wstring(player.GetSanity()));
}

void GameClient::Render(float)
{
	renderer.Render();
}

int GameClient::MainLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	float lastSecond = 0.0f;
	int framesPerSecond = 0;

	bool quit = false;
	const int TimePerFrame = 1000 / 60;
	LARGE_INTEGER lastFrame;
	QueryPerformanceCounter(&lastFrame);
	while (!quit)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		LARGE_INTEGER currentFrame;
		QueryPerformanceCounter(&currentFrame);

		float timeElapsed = float(currentFrame.QuadPart - lastFrame.QuadPart) / float(frequency.QuadPart);

		if (int(timeElapsed * 1000) < TimePerFrame)
			Sleep(TimePerFrame - int(timeElapsed * 1000));

		LARGE_INTEGER oldLastFrame = lastFrame;
		lastFrame = currentFrame;
		QueryPerformanceCounter(&currentFrame);

		timeElapsed = float(currentFrame.QuadPart - oldLastFrame.QuadPart) / float(frequency.QuadPart);
		if (!paused)
		{
			gameTime += 1.0f / 60.0f;

			OutputDebugString(L"Begin frame\n");

			Input(timeElapsed);
			Update(timeElapsed);
			Render(timeElapsed);

			OutputDebugString(L"End frame\n");
		}
		else
			Input(timeElapsed);

		framesPerSecond++;
		if (gameTime > lastSecond + 1.0f)
		{
			lastSecond = gameTime;
			framesPerSecond = 0;
		}
	}

	return msg.wParam;
}

float GameClient::GetGameTime() const
{
	return gameTime;
}

const Player* GameClient::GetPlayer() const
{
	return &player;
}

LPD3DXFONT GameClient::GetFont(const std::wstring& name)
{
	return fonts[name];
}