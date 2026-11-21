#pragma once
#include "Game.h"
#include "MeshComponent.h"
#include "Mesh.h"

class SideBlock : public Actor
{
private:
	class MeshComponent* mMeshComponent;

public:
	SideBlock(class Game* game, size_t textureIndex);
	void OnUpdate(float deltaTime) override;

	const float BLOCK_SCALE = 500.0f;
};
