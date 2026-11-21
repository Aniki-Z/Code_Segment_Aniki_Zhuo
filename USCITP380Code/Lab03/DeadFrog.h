#pragma once
#include "Game.h"

class DeadFrog : public Actor
{
private:
	SpriteComponent* mSpriteComponent;
	float mTimeToDestroy;

public:
	DeadFrog(Game* game, const Vector2& position);
	void OnUpdate(float deltaTime) override;

	static constexpr float INTERVAL_MAX = 0.5f;
};
