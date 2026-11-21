#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "Goomba.h"

class Spawner : public Actor
{
public:
	Spawner(Game* game);
	void OnUpdate(float deltaTime) override;

	static constexpr float CREATE_DISTANCE = 600.0f;
};