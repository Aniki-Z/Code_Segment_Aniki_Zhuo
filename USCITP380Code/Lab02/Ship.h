#pragma once
#include "Game.h"

class Ship : public Actor
{
private:
	class SpriteComponent* mSpriteComponent;
	class MoveComponent* mMoveComponent;

	float mCooldown;

public:
	static constexpr float SHIP_SPEED = 400.0f;
	static constexpr float SHIP_ANGULAR = Math::TwoPi;

	Ship(class Game* game);
	void OnProcessInput(const uint8_t* keyState) override;
	void OnUpdate(float deltaTime) override;
};