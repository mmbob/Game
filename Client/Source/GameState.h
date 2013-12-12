#pragma once

#include <memory>
#include <unordered_map>

class Engine;
class GameWorld;
class Player;
class Renderer;
class IRenderObject;
class TextRenderObject;
class TextureRenderObject;

class GameState
{
protected:
	Renderer* renderer;

	GameState* parent;
	GameState* child;
public:
	GameState(GameState* parent, Renderer* renderer);
	virtual ~GameState();

	virtual void EnterState(GameState* newState);
	virtual void ExitState();

	virtual void Input(float timeElapsed);
	virtual void Update(float timeElapsed);
	virtual void Render(float timeElapsed);

	virtual GameState* GetParent() const;
	virtual GameState* GetChild() const;
};

class InGameState : public GameState
{
	std::unique_ptr<Engine> engine;
	std::unique_ptr<GameWorld> world;
	std::unique_ptr<Player> player;

	float gameTime;
	bool isDayTime;
	int startDay;

	std::unordered_map<std::wstring, IRenderObject*> uiElements;
public:
	InGameState(GameState* parent, Renderer* renderer, int startDay);
	virtual ~InGameState();

	virtual void EnterState(GameState* newState);
	virtual void ExitState();

	virtual void Input(float timeElapsed);
	virtual void Update(float timeElapsed);
	virtual void Render(float timeElapsed);

	Player* GetPlayer() const;

	float GetGameTime() const;
	bool IsDayTime() const;
};

class InGamePausedState : public GameState
{
	float totalTimeElapsed;
public:
	InGamePausedState(GameState* parent, Renderer* renderer);
	virtual ~InGamePausedState();

	virtual void Input(float timeElapsed);
	virtual void Update(float timeElapsed);
};

class GameOverState : public GameState
{
	int startDay;
	int highestSanity;
	int hoursSurvived;

	std::unique_ptr<TextRenderObject> gameOverText;
	std::unique_ptr<TextRenderObject> descriptionText;
public:
	GameOverState(GameState* parent, Renderer* renderer, int startDay, int highestSanity, int hoursSurvived);
	virtual ~GameOverState();

	virtual void Input(float timeElapsed);
	virtual void Update(float timeElapsed);
};

class MainMenuState : public GameState
{
	struct MenuItemName
	{
		enum Value
		{
			None = -1,
			StartGame = 0,
			Quit = 1,
		};
	};

	std::unordered_map<std::wstring, IRenderObject*> uiElements;

	std::vector<IRenderObject*> menuItems;
	MenuItemName::Value selectedMenuItem;
	float selectedItemTween;
	float selectedItemTweenChange;
	int startDay;

	void SelectMenuItem(MenuItemName::Value newSelection);
public:
	MainMenuState(GameState* parent, Renderer* renderer, int startDay);
	virtual ~MainMenuState();

	virtual void Input(float timeElapsed);
	virtual void Update(float timeElapsed);
};

class HighScoreState : public GameState
{
public:
	HighScoreState(GameState* parent, Renderer* renderer);
	virtual ~HighScoreState();

	virtual void Update(float timeElapsed);
};