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
#include "Ship.h"
#include "Asteroid.h"
#include "Laser.h"

Game::Game()
: mWindow(nullptr)
, mRenderer(nullptr)
, mIsRunning(true)
, mFrameCount(0)
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
	Actor* mStars = new Actor(this);
	SpriteComponent* mStarsComponent = new SpriteComponent(mStars, 0);
	mStarsComponent->SetTexture(GetTexture("Assets/Stars.png"));
	mStars->SetPosition(Vector2(MIDDLE_WIDTH, MIDDLE_HEIGHT));

	Ship* mShip = new Ship(this);
	mShip->SetPosition(Vector2(MIDDLE_WIDTH, MIDDLE_HEIGHT));

	for (size_t i = 0; i < 10; i++)
	{
		new Asteroid(this);
	}
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

void Game::AddAsteroid(Asteroid* mAsteroid)
{
	mAsteroids.push_back(mAsteroid);
}

void Game::RemoveAsteroid(Asteroid* mAsteroid)
{
	std::vector<Asteroid*>::iterator iterator = std::find(mAsteroids.begin(), mAsteroids.end(),
														  mAsteroid);
	if (iterator != mAsteroids.end())
	{
		mAsteroids.erase(iterator);
	}
}
