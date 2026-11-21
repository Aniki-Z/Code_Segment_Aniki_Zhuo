#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "AudioSystem.h"
#include "Player.h"
#include "HeightMap.h"
#include "Enemy.h"
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

	AudioSystem* GetAudio() { return mAudio; }

	class Renderer* GetRenderer() { return mRenderer; }

	class Player* GetPlayer() const { return mPlayer; }
	class Enemy* GetEnemy() const { return mEnemy; }

	const float WINDOW_WIDTH = 1024.0f;
	const float WINDOW_HEIGHT = 768.0f;

	class HeightMap* GetHeightMap() const { return mHeightMap; };

	class SoundHandle GetSound() const { return mSound; };
	void SetSound(class SoundHandle sound) { mSound = sound; };

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

	class HeightMap* mHeightMap;
	class Enemy* mEnemy;

	float mStartTimer;
	bool mStartcheck = true;

	class SoundHandle mSound;
};
