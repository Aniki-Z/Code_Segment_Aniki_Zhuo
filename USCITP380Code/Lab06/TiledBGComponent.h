#pragma once
#include "Game.h"
#include "CSVHelper.h"
#include <fstream>
#include <sstream>

class TiledBGComponent : public SpriteComponent
{
private:
	int mTileWidth;
	int mTileHeight;
	std::vector<std::vector<int>> mTileMap;

public:
	TiledBGComponent(class Actor* owner, int drawOrder = 50);

	void Draw(SDL_Renderer* renderer) override;
	void LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight);
};