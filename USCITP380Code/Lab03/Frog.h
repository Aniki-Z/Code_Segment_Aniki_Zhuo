#pragma once
#include "Game.h"
#include "DeadFrog.h"

class Frog : public Actor
{
private:
	class SpriteComponent* mSpriteComponent;
	class CollisionComponent* mCollisionComponent;

public:
	Frog(class Game* game);
	~Frog();
	void OnProcessInput(const uint8_t* keyState) override;
	void OnUpdate(float deltaTime) override;

	static constexpr float FROG_SIZE = 25.0f;
	static constexpr float FROG_MOVE = 32.0f;
	static constexpr float WIDTH_MAX = 416.0f;
	static constexpr float HEIGHT_MAX = 464.0f;
	static constexpr float DEAD_FROG_LOWER = 90.0f;
	static constexpr float DEAD_FROG_UPPER = 255.0f;
	static constexpr float FROG_POSITION_CHANGE = 16.0f;
	static constexpr float FROG_CLAMP = 80.0f;
};