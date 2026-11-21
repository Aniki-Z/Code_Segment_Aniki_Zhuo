#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "Game.h"

class PlayerMesh : public Actor
{
private:
	class MeshComponent* mMeshComponent;

public:
	PlayerMesh(Game* game);
	void OnUpdate(float deltaTime) override;

	Vector3 mScale = Vector3(1.0f, 2.5f, 2.5f);
};