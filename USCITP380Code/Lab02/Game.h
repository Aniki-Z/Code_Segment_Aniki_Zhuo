#pragma once
#include "SDL2/SDL.h"
#include "Actor.h"
#include <unordered_map>
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Random.h"
#include "Math.h"

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

	std::vector<class Asteroid*> mAsteroids;

public:
	Game();

	static constexpr float WINDOW_WIDTH = 1024;
	static constexpr float WINDOW_HEIGHT = 768;
	static constexpr float MIDDLE_WIDTH = 512.0f;
	static constexpr float MIDDLE_HEIGHT = 384.0f;

	bool Initialize();
	void Shutdown();
	void RunLoop();

	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	SDL_Texture* GetTexture(const std::string& fileName);

	void AddSprite(SpriteComponent* mSprite);
	void RemoveSprite(SpriteComponent* mSprite);

	void AddAsteroid(class Asteroid* mAsteroid);
	void RemoveAsteroid(class Asteroid* mAsteroid);

	const std::vector<class Asteroid*>& GetAsteroids() const { return mAsteroids; }
};