#pragma once
#include "Game.h"

class Laser : public Actor
{
private:
	class SpriteComponent* mSpriteComponent;
	class MoveComponent* mMoveComponent;

	float mLifetime;

public:
	Laser(class Game* game);
	void OnUpdate(float deltaTime) override;

	static constexpr float LASER_DISTANCE = 70.0f;

	static constexpr float LASER_SPEED = 400.0f;
};