//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"

Game::Game()
: mWindow(nullptr)
, mRenderer(nullptr)
, mIsRunning(true)
, mPaddle{WALL_THICKNESS / 2, WINDOW_HEIGHT / 2}
, mBall{WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2}
, mBallVelocity{BALL_VELOCITY, BALL_VELOCITY}
, mPaddleDirection(0)
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

	return true;
}

void Game::ProcessInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			mIsRunning = false;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(nullptr);

	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mPaddleDirection = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDirection = 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDirection = -1;
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

	mBall.x += static_cast<int>(mBallVelocity.x * mDeltaTime);
	mBall.y += static_cast<int>(mBallVelocity.y * mDeltaTime);

	if (mPaddleDirection != 0)
	{
		mPaddle.y -= static_cast<int>(mPaddleDirection * BALL_VELOCITY * mDeltaTime);

		if (mPaddle.y < (PADDLE_HEIGHT / 2 + WALL_THICKNESS))
		{
			mPaddle.y = PADDLE_HEIGHT / 2 + WALL_THICKNESS;
		}
		else if (mPaddle.y > (WINDOW_HEIGHT - PADDLE_HEIGHT / 2 - WALL_THICKNESS))
		{
			mPaddle.y = WINDOW_HEIGHT - PADDLE_HEIGHT / 2 - WALL_THICKNESS;
		}
	}

	if (mBall.y - WALL_THICKNESS * 3 / 2 <= 0)
	{
		mBall.y = WALL_THICKNESS * 3 / 2;
		mBallVelocity.y *= -1;
	}
	else if (mBall.y + WALL_THICKNESS * 3 / 2 >= WINDOW_HEIGHT)
	{
		mBall.y = WINDOW_HEIGHT - WALL_THICKNESS * 3 / 2;
		mBallVelocity.y *= -1;
	}
	else if (mBall.x + WALL_THICKNESS * 3 / 2 >= WINDOW_WIDTH)
	{
		mBall.x = WINDOW_WIDTH - WALL_THICKNESS * 3 / 2;
		mBallVelocity.x *= -1;
	}
	else if (mBall.x - WALL_THICKNESS / 2 <= mPaddle.x + WALL_THICKNESS / 2 &&
			 mBall.y + WALL_THICKNESS / 2 <= mPaddle.y + PADDLE_HEIGHT / 2 &&
			 mBall.y - WALL_THICKNESS / 2 >= mPaddle.y - PADDLE_HEIGHT / 2)
	{
		mBall.x = mPaddle.x + WALL_THICKNESS;
		mBallVelocity.x *= -1;
	}

	if (mBall.x + WALL_THICKNESS / 2 < 0)
	{
		mIsRunning = false;
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	SDL_Rect topWall{0, 0, WINDOW_WIDTH, WALL_THICKNESS};
	SDL_RenderFillRect(mRenderer, &topWall);

	SDL_Rect bottomWall{0, WINDOW_HEIGHT - WALL_THICKNESS, WINDOW_WIDTH, WALL_THICKNESS};
	SDL_RenderFillRect(mRenderer, &bottomWall);

	SDL_Rect rightWall{WINDOW_WIDTH - WALL_THICKNESS, 0, WALL_THICKNESS, WINDOW_HEIGHT};
	SDL_RenderFillRect(mRenderer, &rightWall);

	SDL_Rect mPaddleOutfit{mPaddle.x - WALL_THICKNESS / 2, mPaddle.y - PADDLE_HEIGHT / 2,
						   WALL_THICKNESS, PADDLE_HEIGHT};
	SDL_RenderFillRect(mRenderer, &mPaddleOutfit);

	SDL_Rect mBallOutfit{mBall.x - WALL_THICKNESS / 2, mBall.y - WALL_THICKNESS / 2, WALL_THICKNESS,
						 WALL_THICKNESS};
	SDL_RenderFillRect(mRenderer, &mBallOutfit);

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
	SDL_DestroyRenderer(mRenderer);
	mRenderer = nullptr;

	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	SDL_Quit();
}
