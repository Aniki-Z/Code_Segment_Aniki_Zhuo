#pragma once
#include "SDL2/SDL.h"
#include "Actor.h"
#include <unordered_map>
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "Random.h"
#include "Math.h"
#include "Frog.h"
#include "Vehicle.h"
#include "Log.h"

class Game
{
private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	std::unordered_map<std::string, SDL_Texture*> mTextures;

	bool mIsRunning;

	Uint32 mFrameCount;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	std::vector<Actor*> mActors;
	std::vector<SpriteComponent*> mSprites;

	void LoadData();
	void UnloadData();

	class Frog* mFrog;
	std::vector<class Vehicle*> mVehicles;
	std::vector<class Log*> mLogs;
	CollisionComponent* mCollisionComponent;

public:
	Game();

	static const int WINDOW_WIDTH = 448;
	static const int WINDOW_HEIGHT = 512;
	static constexpr float MIDDLE_WIDTH = 224.0f;
	static constexpr float MIDDLE_HEIGHT = 256.0f;
	static const int SQUARE_SIZE = 32;
	static const int FIRST_SQUARE_Y = 80;

	Vector2 mStartPosition;
	Vector2 mGoalPosition;

	bool Initialize();
	void Shutdown();
	void RunLoop();

	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	SDL_Texture* GetTexture(const std::string& fileName);

	void AddSprite(SpriteComponent* mSprite);
	void RemoveSprite(SpriteComponent* mSprite);

	void AddVehicle(class Vehicle* mVehicles);
	void RemoveVehicle(class Vehicle* mVehicles);

	void AddLog(class Log* mLog);
	void RemoveLog(class Log* mLog);

	class Frog* GetFrog() { return mFrog; }

	std::vector<class Vehicle*>& GetVehicles() { return mVehicles; }

	std::vector<class Log*>& GetLogs() { return mLogs; }

	Vector2& GetStartPosition() { return mStartPosition; }

	Vector2& GetGoalPosition() { return mGoalPosition; }

	CollisionComponent* GetGoalCollisionComponent() const { return mCollisionComponent; }
};