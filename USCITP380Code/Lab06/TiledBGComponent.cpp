#include "TiledBGComponent.h"
#include "Game.h"
#include <iostream>

TiledBGComponent::TiledBGComponent(Actor* owner, int drawOrder)
: SpriteComponent(owner, drawOrder)
, mTileWidth(0)
, mTileHeight(0)
{
}

void TiledBGComponent::Draw(SDL_Renderer* renderer)
{
	if (!mTexture)
	{
		return;
	}

	Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();
	int tilesPerRow = mTexWidth / mTileWidth;

	for (size_t i = 0; i < mTileMap.size(); i++)
	{
		for (size_t j = 0; j < mTileMap[i].size(); j++)
		{
			int tileIndex = mTileMap[i][j];
			if (tileIndex >= 0)
			{
				SDL_Rect srcRect;
				srcRect.w = mTileWidth;
				srcRect.h = mTileHeight;
				srcRect.x = (tileIndex % tilesPerRow) * mTileWidth;
				srcRect.y = (tileIndex / tilesPerRow) * mTileHeight;

				SDL_Rect destRect;
				destRect.w = mTileWidth;
				destRect.h = mTileHeight;
				destRect.x = static_cast<int>(j * mTileWidth - cameraPos.x);
				destRect.y = static_cast<int>(i * mTileHeight - cameraPos.y);

				SDL_RenderCopy(renderer, mTexture, &srcRect, &destRect);
			}
		}
	}
}

void TiledBGComponent::LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight)
{
	mTileWidth = tileWidth;
	mTileHeight = tileHeight;
	mTileMap.clear();

	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty())
		{
			continue;
		}

		std::vector<std::string> tokens = CSVHelper::Split(line);
		std::vector<int> i;
		for (const std::string& token : tokens)
		{
			i.push_back(std::stoi(token));
		}
		mTileMap.push_back(i);
	}
	file.close();
}
