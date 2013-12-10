#pragma once

#include <memory>

class Engine;
class GameWorld;
class Player;

class GameState
{
	GameState* parent;
	GameState* child;
public:
	GameState();

	virtual void EnterState(GameState* newState);
	virtual void ExitState();

	virtual void Update(float timeElapsed);

	virtual GameState* GetParent() const;
	virtual GameState* GetChild() const;
};

class InGameState : public GameState
{
	std::unique_ptr<Engine> engine;
	std::unique_ptr<GameWorld> world;
	std::unique_ptr<Player> player;
public:
};

class InGamePausedState : public GameState
{

};

class MainMenuState : public GameState
{

};

class HighScoreState : public GameState
{

};