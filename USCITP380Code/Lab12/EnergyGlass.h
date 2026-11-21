#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Game.h"

class EnergyGlass : public Actor
{
private:
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;

public:
	EnergyGlass(Game* game);
	~EnergyGlass();
};
