#include "Soldier.h"

Soldier::Soldier(Game* game, PathNode* start, PathNode* end)
: Actor(game)
{
	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(SOLDIER_SIZE, SOLDIER_SIZE);

	mAnimatedSprite = new AnimatedSprite(this, 200);
	mAnimatedSprite->LoadAnimations("Assets/Soldier");
	mAnimatedSprite->SetAnimation("WalkDown");
	mAnimatedSprite->SetAnimFPS(5.0f);

	mSoldierAI = new SoldierAI(this);
	mSoldierAI->Setup(start, end);

	mEnemyComponent = new EnemyComponent(this);
	mEnemyComponent->SetHitPoints(2);

	mEnemyComponent->SetOnDamageCallback([this]() {
		mSoldierAI->SetIsStunned(true);
		mSoldierAI->SetStunTime(mSoldierAI->GetStunDuration());
		mAnimatedSprite->SetIsPaused(true);

		new Effect(GetGame(), GetPosition(), "Hit", "EnemyHit.wav");
	});

	mEnemyComponent->SetOnDeathCallback([this]() {
		new Effect(GetGame(), GetPosition(), "Death", "EnemyDie.wav");
		SetState(ActorState::Destroy);
	});
}