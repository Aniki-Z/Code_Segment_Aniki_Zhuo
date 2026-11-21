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
#include "Player.h"
#include "AnimatedSprite.h"
#include "AudioSystem.h"
#include "PathNode.h"
#include "TiledBGComponent.h"
#include "Collider.h"
#include "Bush.h"
#include "Soldier.h"
#include "PathFinder.h"
#include "SoldierAI.h"
#include "EnemyComponent.h"
#include "Effect.h"

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

	int mMusicChannel;

	class Player* mPlayer;

	Vector2 mCameraPos;

	std::vector<class Collider*> mColliders;

	class AudioSystem* mAudioSystem;

	std::vector<class Bush*> mBushs;

	std::vector<class Soldier*> mSoldiers;

	class PathFinder* mPathFinder;

	std::vector<class EnemyComponent*> mEnemies;

	SoundHandle mMusicStartHandle;
	bool mIsLoopPlaying = false;

public:
	Game();

	static const int WINDOW_WIDTH = 512;
	static const int WINDOW_HEIGHT = 448;
	static constexpr float MIDDLE_WIDTH = 256.0f;
	static constexpr float MIDDLE_HEIGHT = 224.0f;
	static constexpr float TAIL_HEIGHT = 32.0f;

	bool Initialize();
	void Shutdown();
	void RunLoop();

	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);

	SDL_Texture* GetTexture(const std::string& fileName);

	void AddSprite(SpriteComponent* mSprite);
	void RemoveSprite(SpriteComponent* mSprite);

	CollisionComponent* GetGoalCollisionComponent() const { return mCollisionComponent; }

	void PlaySound(const std::string& filename);

	int GetMusicChannel() const { return mMusicChannel; };

	Vector2& GetCameraPos() { return mCameraPos; };
	void SetCameraPos(Vector2 position) { mCameraPos = position; };

	Player* GetPlayer() { return mPlayer; };

	void LoadObjectsFromCSV(const std::string& fileName);

	const std::vector<class Collider*> GetColliders() const { return mColliders; };

	const std::vector<class Bush*> GetBushs() const { return mBushs; };

	std::vector<class Soldier*> GetSoldiers() const { return mSoldiers; };

	PathFinder* GetPathFinder() const { return mPathFinder; };

	std::vector<class EnemyComponent*>& GetEnemies() { return mEnemies; }

	AudioSystem* GetAudio() { return mAudioSystem; };

	void PlayGameMusic();
	void UpdateMusic();
};