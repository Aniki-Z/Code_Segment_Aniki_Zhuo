#pragma once
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "MoveComponent.h"
#include "Mesh.h"

class Bullet : public Actor
{
private:
	class MeshComponent* mMeshComponent;
	class CollisionComponent* mCollisionComponent;
	class MoveComponent* mMoveComponent;

	float mAliveTime;

public:
	Bullet(class Game* game);
	void OnUpdate(float deltaTime) override;
	void Explode(class Block* block);

	const float BULLET_SCALE = 5.0f;
	const float BULLET_SIZE = 10.0f;
	const float BULLET_SPEED = 900.0f;
	const float EXPLODE_RADIUS = 50.0f;
};