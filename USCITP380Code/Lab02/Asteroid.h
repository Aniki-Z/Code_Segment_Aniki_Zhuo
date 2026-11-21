#pragma once
#include "Game.h"

class Asteroid : public Actor
{
private:
	class SpriteComponent* mSpriteComponent;
	class MoveComponent* mMoveComponent;

public:
	Asteroid(class Game* game);
	~Asteroid();
	void OnUpdate(float deltaTime) override;

	static constexpr float ASTEROID_SPEED = 150.0f;
};