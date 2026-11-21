#pragma once
#include "Game.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Mesh.h"

class Block : public Actor
{
private:
	class MeshComponent* mMeshComponent;
	class CollisionComponent* mCollisionComponent;
	bool mIsExploding;
	bool mIsExplode;

public:
	Block(class Game* game, size_t textureIndex);
	~Block();
	void OnUpdate(float deltaTime) override;

	const float BLOCK_SCALE = 25.0f;
	const float BLOCK_SIZE = 1.0f;
	const float INTER_DISTANCE = 2000.0f;

	CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; };
	bool CanExplode() const { return mIsExploding; };
	void SetExplode(bool judge) { mIsExplode = judge; };
	bool GetExplode() const { return mIsExplode; };
};
