#pragma once

#include <string>
#define NDEBUG
#include <libjson.h>

#include "Util.h"

class GameWorld;
class JSONNode;

class TileFileParser
{
	GameWorld* gameWorld;

	static bool ParseRect(std::string text, Rect* pRect);

	template <typename T>
	static T GetValueOrDefault(const JSONNode& node, const std::string& childName, T defaultValue);
public:
	TileFileParser(GameWorld* gameWorld);

	int LoadFile(const std::wstring& fileName);
};