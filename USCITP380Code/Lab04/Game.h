#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include "Actor.h"
#include <unordered_map>
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "Random.h"
#include "Math.h"
#include "Block.h"
#include "Player.h"
#include "Spawner.h"

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

	CollisionComponent* mCollisionComponent;

	std::unordered_map<std::string, Mix_Chunk*> mSounds;
	int mMusicChannel;

	std::vector<class Block*> mBlocks;

	class Player* mPlayer;

	Vector2 mCameraPos;

	std::vector<class Goomba*> mGoombas;

public:
	Game();

	static const int WINDOW_WIDTH = 600;
	static const int WINDOW_HEIGHT = 448;
	static constexpr float MIDDLE_WIDTH = 3392.0f;
	static constexpr float MIDDLE_HEIGHT = 224.0f;
	static constexpr float SQUARE_SIZE = 32.0f;
	static constexpr float FIRST_SQUARE = 16.0f;

	bool Initialize();
	void Shutdown();
	void RunLoop();

	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	SDL_Texture* GetTexture(const std::string& fileName);

	void AddSprite(SpriteComponent* mSprite);
	void RemoveSprite(SpriteComponent* mSprite);

	CollisionComponent* GetGoalCollisionComponent() const { return mCollisionComponent; }

	Mix_Chunk* GetSound(const std::string& filename);
	void PlaySound(const std::string& filename);

	int GetMusicChannel() const { return mMusicChannel; };

	void AddBlock(class Block* mBlock);
	void RemoveBlock(class Block* mBlock);

	std::vector<class Block*>& GetBlocks() { return mBlocks; };

	Vector2& GetCameraPos() { return mCameraPos; }

	void AddEnemy(class Goomba* mGoomba);
	void RemoveEnemy(class Goomba* mGoomba);

	Player* GetPlayer() { return mPlayer; };

	std::vector<class Goomba*>& GetGoombas() { return mGoombas; };
};