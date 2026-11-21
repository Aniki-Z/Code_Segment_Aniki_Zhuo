#include "Bush.h"

Bush::Bush(Game* game)
: Actor(game)
{
	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(BUSH_SIZE, BUSH_SIZE);

	mSpriteComponent = new SpriteComponent(this);
	mSpriteComponent->SetTexture(game->GetTexture("Assets/Bush.png"));

	mEnemyComponent = new EnemyComponent(this);
	mEnemyComponent->SetHitPoints(1);

	mEnemyComponent->SetOnDeathCallback([game, mOwner = this]() {
		PathFinder* pathFinder = mOwner->GetGame()->GetPathFinder();

		int row = static_cast<int>(mOwner->GetPosition().y) / 32;
		int col = static_cast<int>(mOwner->GetPosition().x) / 32;

		pathFinder->SetIsBlocked(row, col, false);

		new Effect(game, mOwner->GetPosition(), "BushDeath", "BushDie.wav");
	});
}