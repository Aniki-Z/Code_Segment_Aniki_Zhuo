#pragma once

#include "SDL2/SDL.h"

class Game
{
private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	bool mIsRunning;

	SDL_Point mPaddle;
	SDL_Point mBall;

	SDL_Point mBallVelocity;

	int mPaddleDirection;
	Uint32 mFrameCount;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

public:
	Game();

	bool Initialize();
	void Shutdown();
	void RunLoop();

	static const int WALL_THICKNESS = 20;
	static const int PADDLE_HEIGHT = 150;
	static const int WINDOW_WIDTH = 1024;
	static const int WINDOW_HEIGHT = 768;
	static const int BALL_VELOCITY = 300;
};