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
, mSounds()
, mMusicChannel(0)
, mPlayer(nullptr)
, mCameraPos(Vector2(0.0f, 0.0f))
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

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

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

	Mix_CloseAudio();

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
	Actor* mBackground = new Actor(this);
	SpriteComponent* mBackgroundComponent = new SpriteComponent(mBackground, 0);
	mBackgroundComponent->SetTexture(GetTexture("Assets/Background.png"));
	mBackground->SetPosition(Vector2(MIDDLE_WIDTH, MIDDLE_HEIGHT));

	std::ifstream mTextFile("Assets/Level1.txt");
	if (!mTextFile.is_open())
	{
		return;
	}

	std::string line;
	int row = 0;

	while (std::getline(mTextFile, line))
	{
		for (int col = 0; col < line.size(); col++)
		{
			char type = line[col];
			Vector2 position(FIRST_SQUARE + col * SQUARE_SIZE, FIRST_SQUARE + row * SQUARE_SIZE);

			if (type >= 'A' && type <= 'I')
			{
				Block* mBlock = new Block(this, type);
				mBlock->SetPosition(position);
				AddBlock(mBlock);
			}
			else if (type == 'P')
			{
				mPlayer = new Player(this);
				mPlayer->SetPosition(position);
			}
			else if (type == 'Y')
			{
				Spawner* mSpawner = new Spawner(this);
				mSpawner->SetPosition(position);
			}
		}
		row++;
	}

	mTextFile.close();

	mMusicChannel = Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);
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

	std::unordered_map<std::string, Mix_Chunk*>::iterator iterators;
	for (iterators = mSounds.begin(); iterators != mSounds.end(); iterators++)
	{
		Mix_FreeChunk(iterators->second);
	}
	mSounds.clear();
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

void Game::AddBlock(Block* mBlock)
{
	mBlocks.push_back(mBlock);
}

void Game::RemoveBlock(Block* mBlock)
{
	std::vector<Block*>::iterator iterator = std::find(mBlocks.begin(), mBlocks.end(), mBlock);
	if (iterator != mBlocks.end())
	{
		mBlocks.erase(iterator);
	}
}

void Game::AddEnemy(Goomba* mGoomba)
{
	mGoombas.push_back(mGoomba);
}

void Game::RemoveEnemy(Goomba* mGoomba)
{
	std::vector<Goomba*>::iterator iterator = std::find(mGoombas.begin(), mGoombas.end(), mGoomba);
	if (iterator != mGoombas.end())
	{
		mGoombas.erase(iterator);
	}
}

Mix_Chunk* Game::GetSound(const std::string& filename)
{
	std::unordered_map<std::string, Mix_Chunk*>::iterator iter = mSounds.find(filename);
	if (iter != mSounds.end())
	{
		return iter->second;
	}

	Mix_Chunk* sound = Mix_LoadWAV(filename.c_str());
	if (!sound)
	{
		SDL_Log("Failed to load sound: %s, Error: %s", filename.c_str(), Mix_GetError());
		return nullptr;
	}

	mSounds[filename] = sound;
	return sound;
}

void Game::PlaySound(const std::string& filename)
{
	Mix_Chunk* sound = GetSound(filename);
	if (sound)
	{
		Mix_PlayChannel(-1, sound, 0);
	}
}
