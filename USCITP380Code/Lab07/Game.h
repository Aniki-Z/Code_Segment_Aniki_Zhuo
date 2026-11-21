#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "AudioSystem.h"
#include "Player.h"
#include "SideBlock.h"
#include "Block.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define emscripten_cancel_main_loop()
#endif

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void EmRunIteration()
	{
		if (!mIsRunning)
		{
			emscripten_cancel_main_loop();
		}
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	AudioSystem* GetAudio() const { return mAudio; }

	class Renderer* GetRenderer() const { return mRenderer; }

	class Player* GetPlayer() const { return mPlayer; }

	const float WINDOW_WIDTH = 1024.0f;
	const float WINDOW_HEIGHT = 768.0f;

	std::vector<class Block*> GetBlocks() const { return mBlocks; };
	void AddBlock(class Block* block);
	void RemoveBlock(class Block* block);

	void LoadBlocks(const std::string& fileName);

	const float BLOCK_SCALE = 25.0f;
	const float FILE_CORNER = 237.5f;
	const float BLOCK_DISTANCE = 1000.0f;
	const float EYE_X_POSITION = -300.0f;
	const float TARGET_X_POSITION = 20.0f;
	const float CAMERA_Y_Z_POSITION = 0.0f;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	// All the actors in the game
	std::vector<class Actor*> mActors;

	class Renderer* mRenderer = nullptr;
	AudioSystem* mAudio = nullptr;

	Uint32 mTicksCount = 0;
	bool mIsRunning = true;

	class Player* mPlayer;

	std::vector<class Block*> mBlocks;

	float mBlockXPosition = 0.0f;
};
