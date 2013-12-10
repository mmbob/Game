#pragma once

#include <memory>
#include <unordered_map>

class Engine;
class GameWorld;
class Player;
class Renderer;
class IRenderObject;

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

	std::unordered_map<std::wstring, IRenderObject*> uiElements;
public:
	InGameState(GameState* parent, Renderer* renderer);
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
	int highestSanity;
	int hoursSurvived;
public:
	GameOverState(GameState* parent, Renderer* renderer, int highestSanity, int hoursSurvived);
	virtual ~GameOverState();

	virtual void Input(float timeElapsed);
	virtual void Update(float timeElapsed);
};

class MainMenuState : public GameState
{
public:
	MainMenuState(GameState* parent, Renderer* renderer);
	virtual ~MainMenuState();

	virtual void Update(float timeElapsed);
};

class HighScoreState : public GameState
{
public:
	HighScoreState(GameState* parent, Renderer* renderer);
	virtual ~HighScoreState();

	virtual void Update(float timeElapsed);
};