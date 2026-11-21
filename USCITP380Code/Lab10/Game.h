#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "AudioSystem.h"
#include "Player.h"
#include "Renderer.h"
#include "LevelLoader.h"
#include "Prop.h"
#include "Portal.h"
#include "InputReplay.h"
#include "Actor.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
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
	void SetPlayer(class Player* player) { mPlayer = player; }

	const float WINDOW_WIDTH = 1024.0f;
	const float WINDOW_HEIGHT = 768.0f;

	class SoundHandle GetSound() const { return mSound; }
	void SetSound(class SoundHandle sound) { mSound = sound; }

	void AddCollider(class Actor* actor);
	void RemoveCollider(class Actor* actor);

	std::vector<class Actor*>& GetColliders() { return mColliders; }

	class Portal* GetBluePortal() { return mBluePortal; }
	class Portal* GetOrangePortal() { return mOrangePortal; }
	void SetBluePortal(Portal* portal) { mBluePortal = portal; }
	void SetOrangePortal(Portal* portal) { mOrangePortal = portal; }

	void SetNextLevel(const std::string& level) { mNextLevel = level; }
	const std::string& GetCurrentLevel() const { return mCurrentLevel; }

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

	class SoundHandle mSound;

	std::vector<class Actor*> mColliders;

	class Portal* mBluePortal = nullptr;
	class Portal* mOrangePortal = nullptr;

	std::string mCurrentLevel;
	std::string mNextLevel;
	class InputReplay* mInputReplay = nullptr;

	Uint8 mPrevKeyboardState[SDL_NUM_SCANCODES] = {0};
};
