//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Random.h"

Game::Game()
: mPlayer(nullptr)
{
}

bool Game::Initialize()
{
	Random::Init();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		SDL_Log("Unable to initialize Renderer: %s", SDL_GetError());
		return false;
	}

	mAudio = new AudioSystem();

	LoadData();

	mTicksCount = SDL_GetTicks();

	return true;
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

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	std::vector<Actor*> copy = mActors;
	for (auto actor : copy)
	{
		actor->ProcessInput(state);
	}

	mAudio->ProcessInput(state);
}

void Game::UpdateGame()
{
	// Compute delta time
	Uint32 tickNow = SDL_GetTicks();
	// Wait until 16ms has elapsed since last frame
	while (tickNow - mTicksCount < 16)
	{
		tickNow = SDL_GetTicks();
	}

	// For 3D the games, force delta time to 16ms even if slower
	float deltaTime = 0.016f;
	mTicksCount = tickNow;

	mAudio->Update(deltaTime);

	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	// Precache all the sounds (do not remove)
	mAudio->CacheAllSounds();

	mPlayer = new Player(this);

	Matrix4 projection = Matrix4::CreatePerspectiveFOV(1.22f, WINDOW_WIDTH, WINDOW_HEIGHT, 10.0f,
													   10000.0f);
	mRenderer->SetProjectionMatrix(projection);

	Vector3 eyePosition(EYE_X_POSITION, CAMERA_Y_Z_POSITION, CAMERA_Y_Z_POSITION);
	Vector3 targetPosition(TARGET_X_POSITION, CAMERA_Y_Z_POSITION, CAMERA_Y_Z_POSITION);
	Vector3 upDirection = Vector3::UnitZ;

	Matrix4 view = Matrix4::CreateLookAt(eyePosition, targetPosition, upDirection);
	mRenderer->SetViewMatrix(view);

	GetAudio()->PlaySound("Music.ogg", true);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}
}

void Game::Shutdown()
{
	UnloadData();
	delete mAudio;

	mRenderer->Shutdown();
	delete mRenderer;

	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		mActors.erase(iter);
	}
}

void Game::AddBlock(Block* block)
{
	mBlocks.emplace_back(block);
}

void Game::RemoveBlock(Block* block)
{
	auto iter = std::find(mBlocks.begin(), mBlocks.end(), block);
	if (iter != mBlocks.end())
	{
		mBlocks.erase(iter);
	}
}

void Game::LoadBlocks(const std::string& fileName)
{
	std::ifstream file("Assets/Blocks/" + fileName);
	if (!file.is_open())
	{
		return;
	}

	std::string line;
	int row = 0;

	mBlockXPosition += BLOCK_DISTANCE;

	while (std::getline(file, line))
	{
		for (size_t col = 0; col < line.size(); col++)
		{
			if (line[col] == '.')
			{
				continue;
			}

			float yPos = -FILE_CORNER + col * BLOCK_SCALE;
			float zPos = FILE_CORNER - row * BLOCK_SCALE;

			size_t textureIndex = 3;
			if (line[col] == 'B')
			{
				textureIndex = 4;
			}

			Vector3 blockPosition(mBlockXPosition, yPos, zPos);

			Block* block = new Block(this, textureIndex);
			block->SetPosition(blockPosition);
			AddBlock(block);
		}
		row++;
	}
}
