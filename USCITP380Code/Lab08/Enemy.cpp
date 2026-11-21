#include "Enemy.h"
#include "Game.h"

Enemy::Enemy(Game* game)
: Actor(game)
{
	SetScale(0.75f);

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));
	mMesh->SetTextureIndex(6);

	mEnemyMove = new EnemyMove(this);
}