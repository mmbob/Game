#include "TileFileParser.h"

#include <fstream>
#ifdef _DEBUG
#define JSON_DEBUG
#endif
#include <libjson.h>

#include "GameWorld.h"
#include "Util.h"

template <>
bool TileFileParser::GetValueOrDefault<bool>(const JSONNode& node, const std::string& childName, bool defaultValue)
{
	auto iter = node.find_nocase(childName);
	if (iter == node.end())
		return defaultValue;
	return iter->as_bool();
}

template <>
int TileFileParser::GetValueOrDefault<int>(const JSONNode& node, const std::string& childName, int defaultValue)
{
	auto iter = node.find_nocase(childName);
	if (iter == node.end())
		return defaultValue;
	return iter->as_int();
}

template <>
std::string TileFileParser::GetValueOrDefault<std::string>(const JSONNode& node, const std::string& childName, std::string defaultValue)
{
	auto iter = node.find_nocase(childName);
	if (iter == node.end())
		return defaultValue;
	return iter->as_string();
}

TileFileParser::TileFileParser(GameWorld* gameWorld)
: gameWorld(gameWorld)
{ }

int TileFileParser::LoadFile(const std::wstring& fileName)
{
	std::ifstream tileFile(std::string(fileName.begin(), fileName.end()));
	if (!tileFile.is_open())
		return 0;
	tileFile.seekg(0, std::ios::end);
	int fileSize = (int) tileFile.tellg();
	fileSize++;
	tileFile.seekg(0, std::ios::beg);

	std::unique_ptr<char[]> fileData(new char[fileSize + 1]);
	tileFile.get(fileData.get(), fileSize + 1, '\0');

	JSONNode root = libjson::parse(fileData.get());

	Tile* tileList = const_cast<Tile*>(gameWorld->GetTileList());

//	int tilesetCount = root["count"].as_int();

	int tilesLoaded = 0;

	for (auto tileset : root["tilesets"].as_array())
	{
		int tilesetID = tileset["id"].as_int();
		std::string tilesetName = tileset["name"].as_string();

		for (auto tile : tileset["tiles"].as_array())
		{
			tilesLoaded++;
			int tileID = tile["id"].as_int();
			tileList[tileID].ID = tileID;
			tileList[tileID].Tileset = tilesetID;

			std::string tileName = tile["name"].as_string();

			std::string rawTextureClip = tile["texture_clip"].as_string();
			ParseRect(rawTextureClip, &tileList[tileID].TextureClip);

			tileList[tileID].Flags = 0;
			bool blockMovement = GetValueOrDefault(tile, "block_movement", false);
			bool blockSight = GetValueOrDefault(tile, "block_sight", false);
			if (blockMovement || blockSight)
			{
				tileList[tileID].Flags |= (blockMovement ? TileFlags::BlockMovement : 0) | (blockSight ? TileFlags::BlockSight : 0);
				std::string rawBlockType = tile["block_type"].as_string();
				if (rawBlockType == "rectangle")
				{
					tileList[tileID].Flags |= TileFlags::BlockRectangle;

					std::string rawBlockRect = tile["block_rectangle"].as_string();
					ParseRect(rawBlockRect, &tileList[tileID].BlockRect);
				}
				else if (rawBlockType == "polygon")
				{
					tileList[tileID].Flags |= TileFlags::BlockPolygon;

					std::vector<POINT>& blockPoints = tileList[tileID].BlockPoints;

					std::string rawBlockPolygon = tile["block_polygon"].as_string();
					while (rawBlockPolygon.length() > 0)
					{
						POINT point;
						point.x = atoi(rawBlockPolygon.c_str());
						rawBlockPolygon = rawBlockPolygon.substr(rawBlockPolygon.find(',') + 1);
						point.y = atoi(rawBlockPolygon.c_str());
						blockPoints.push_back(point);

						int nextPointIndex = rawBlockPolygon.find(';') + 1;
						if (nextPointIndex == 0)
							rawBlockPolygon = "";
						else
							rawBlockPolygon = rawBlockPolygon.substr(nextPointIndex);
					}
				}
			}
		}
	}

	return tilesLoaded;
}

bool TileFileParser::ParseRect(std::string text, Rect* pRect)
{
	Rect& rect = *pRect;
	rect.left = atoi(text.c_str());
	text = text.substr(text.find(',') + 1);
	rect.top = atoi(text.c_str());
	text = text.substr(text.find(',') + 1);
	rect.right = atoi(text.c_str());
	text = text.substr(text.find(',') + 1);
	rect.bottom = atoi(text.c_str());

	return true;
}