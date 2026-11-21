#pragma once
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
