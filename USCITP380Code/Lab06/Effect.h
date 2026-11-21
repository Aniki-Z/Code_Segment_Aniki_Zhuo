#pragma once
#include "Game.h"

class Effect : public Actor
{
private:
	float mLifetime;
	AnimatedSprite* mAnimatedSprite;

public:
	Effect(Game* game, const Vector2& position, const std::string& animationName,
		   const std::string& soundName);

	void OnUpdate(float deltaTime) override;
};
