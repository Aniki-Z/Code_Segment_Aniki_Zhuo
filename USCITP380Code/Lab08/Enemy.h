#pragma once
#include "Actor.h"
#include "EnemyMove.h"
#include "MeshComponent.h"

class Enemy : public Actor
{
private:
	class MeshComponent* mMesh;
	class EnemyMove* mEnemyMove;

public:
	Enemy(class Game* game);

	EnemyMove* GetEnemyMove() const { return mEnemyMove; }
};