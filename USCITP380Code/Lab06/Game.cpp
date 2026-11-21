//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include "Actor.h"
#include <SDL2/SDL_image.h>
#include <fstream>
#include <string>
#include <iostream>

Game::Game()
: mWindow(nullptr)
, mRenderer(nullptr)
, mIsRunning(true)
, mFrameCount(0)
, mCollisionComponent(nullptr)
, mMusicChannel(0)
, mPlayer(nullptr)
, mCameraPos(Vector2(0.0f, 0.0f))
, mAudioSystem(nullptr)
, mPathFinder(nullptr)
{
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		return false;
	}

	mWindow = SDL_CreateWindow("GameWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							   WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	mRenderer = SDL_CreateRenderer(mWindow, -1,
								   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		return false;
	}

	Random::Init();

	mAudioSystem = new AudioSystem();

	LoadData();

	return true;
}

void Game::ProcessInput()
{
	SDL_Event mEvent;

	while (SDL_PollEvent(&mEvent))
	{
		if (mEvent.type == SDL_QUIT)
		{
			mIsRunning = false;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(nullptr);

	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	std::vector<Actor*> mCopyActors = mActors;
	for (size_t i = 0; i < mCopyActors.size(); i++)
	{
		mCopyActors[i]->ProcessInput(keyState);
	}
}

void Game::UpdateGame()
{
	while ((SDL_GetTicks() - mFrameCount) < 16)
	{
	}

	Uint32 mCurrentFrame = SDL_GetTicks();
	float mDeltaTime = (mCurrentFrame - mFrameCount) / 1000.0f;
	mFrameCount = mCurrentFrame;

	if (mDeltaTime > 0.033f)
	{
		mDeltaTime = 0.033f;
	}

	mAudioSystem->Update(mDeltaTime);

	UpdateMusic();

	std::vector<Actor*> mCopyActors = mActors;
	for (size_t i = 0; i < mCopyActors.size(); i++)
	{
		mCopyActors[i]->Update(mDeltaTime);
	}

	std::vector<Actor*> mDestroyedActors;
	for (size_t i = 0; i < mActors.size(); i++)
	{
		if (mActors[i]->GetState() == ActorState::Destroy)
		{
			mDestroyedActors.push_back(mActors[i]);
		}
	}

	for (size_t i = 0; i < mDestroyedActors.size(); i++)
	{
		delete mDestroyedActors[i];
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);

	for (size_t i = 0; i < mSprites.size(); i++)
	{
		if (mSprites[i]->IsVisible())
		{
			mSprites[i]->Draw(mRenderer);
		}
	}

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_RenderPresent(mRenderer);
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	UnloadData();

	delete mAudioSystem;

	if (mRenderer)
	{
		SDL_DestroyRenderer(mRenderer);
		mRenderer = nullptr;
	}

	if (mWindow)
	{
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}

	IMG_Quit();

	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.push_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	std::vector<Actor*>::iterator iterator = std::find(mActors.begin(), mActors.end(), actor);
	if (iterator != mActors.end())
	{
		mActors.erase(iterator);
	}
}

void Game::LoadData()
{
	mAudioSystem->CacheAllSounds();

	mPathFinder = new PathFinder(this);

	Actor* mBackground = new Actor(this);
	TiledBGComponent* mBackgroundComponent = new TiledBGComponent(mBackground);
	mBackgroundComponent->SetTexture(GetTexture("Assets/Map/Tiles.png"));
	mBackgroundComponent->LoadTileCSV("Assets/Map/Tiles.csv", TAIL_HEIGHT, TAIL_HEIGHT);

	LoadObjectsFromCSV("Assets/Map/Objects.csv");
}

void Game::UnloadData()
{
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	std::unordered_map<std::string, SDL_Texture*>::iterator iterator;
	for (iterator = mTextures.begin(); iterator != mTextures.end(); iterator++)
	{
		SDL_DestroyTexture(iterator->second);
	}
	mTextures.clear();
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	std::unordered_map<std::string, SDL_Texture*>::iterator iterator = mTextures.find(fileName);
	if (iterator != mTextures.end())
	{
		return iterator->second;
	}

	SDL_Surface* mSurface = IMG_Load(fileName.c_str());
	if (mSurface == nullptr)
	{
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, mSurface);
	SDL_FreeSurface(mSurface);

	if (texture == nullptr)
	{
		return nullptr;
	}

	mTextures[fileName] = texture;
	return texture;
}

void Game::AddSprite(SpriteComponent* mSprite)
{
	mSprites.push_back(mSprite);
	std::sort(mSprites.begin(), mSprites.end(),
			  [](SpriteComponent* mSpritea, SpriteComponent* mSpriteb) {
				  return mSpritea->GetDrawOrder() < mSpriteb->GetDrawOrder();
			  });
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	std::vector<SpriteComponent*>::iterator iterator = std::find(mSprites.begin(), mSprites.end(),
																 sprite);
	if (iterator != mSprites.end())
	{
		mSprites.erase(iterator);
	}
}

void Game::PlaySound(const std::string& filename)
{
}

void Game::LoadObjectsFromCSV(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return;
	}

	std::string line;
	std::getline(file, line);

	while (std::getline(file, line))
	{
		std::vector<std::string> tokens = CSVHelper::Split(line);
		if (tokens.size() < 5)
		{
			continue;
		}

		std::string type = tokens[0];
		float x = std::stof(tokens[1]);
		float y = std::stof(tokens[2]);
		float width = std::stof(tokens[3]);
		float height = std::stof(tokens[4]);

		x += width / 2;
		y += height / 2;

		if (type == "Player")
		{
			mPlayer = new Player(this);
			mPlayer->SetPosition(Vector2(x, y));
		}
		else if (type == "Bush")
		{
			Bush* bush = new Bush(this);
			bush->SetPosition(Vector2(x, y));
			mBushs.push_back(bush);
		}
		else if (type == "Soldier")
		{
			float rowStart = std::stof(tokens[5]);
			float colStart = std::stof(tokens[6]);
			float rowEnd = std::stof(tokens[7]);
			float colEnd = std::stof(tokens[8]);

			PathNode* startNode = mPathFinder->GetPathNode(rowStart, colStart);
			PathNode* endNode = mPathFinder->GetPathNode(rowEnd, colEnd);

			Soldier* soldier = new Soldier(this, startNode, endNode);
			soldier->SetPosition(Vector2(x, y));
		}
		else
		{
			Collider* collider = new Collider(this, width, height);
			collider->SetPosition(Vector2(x, y));
			mColliders.push_back(collider);
		}
	}

	file.close();
}

void Game::PlayGameMusic()
{
	mMusicStartHandle = GetAudio()->PlaySound("MusicStart.ogg", false);
	mIsLoopPlaying = false;
}

void Game::UpdateMusic()
{
	if (!mIsLoopPlaying && GetAudio()->GetSoundState(mMusicStartHandle) == SoundState::Stopped)
	{
		GetAudio()->PlaySound("MusicLoop.ogg", true);
		mIsLoopPlaying = true;
	}
}