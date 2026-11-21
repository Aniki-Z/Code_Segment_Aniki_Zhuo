#include "EnemyComponent.h"

EnemyComponent::EnemyComponent(class Actor* owner)
: Component(owner)
, mHitPoints(0)
, mInvulnerableTime(0.0f)
{
	Game* game = owner->GetGame();
	game->GetEnemies().push_back(this);

	mCollisionComponent = owner->GetComponent<CollisionComponent>();
}

EnemyComponent::~EnemyComponent()
{
	Game* game = mOwner->GetGame();
	auto& enemies = game->GetEnemies();

	enemies.erase(std::remove(enemies.begin(), enemies.end(), this), enemies.end());
}

void EnemyComponent::TakeDamage()
{
	if (mInvulnerableTime <= 0.0f)
	{
		mHitPoints -= 1;

		if (mHitPoints > 0 && mOnDamageCallback)
		{
			mOnDamageCallback();
		}
		else if (mHitPoints <= 0 && mOnDeathCallback)
		{
			mOnDeathCallback();
			mOwner->SetState(ActorState::Destroy);
		}

		mInvulnerableTime = INVULNERABLE_DURATION;
	}
}

void EnemyComponent::Update(float deltaTime)
{
	if (mInvulnerableTime > 0.0f)
	{
		mInvulnerableTime -= deltaTime;
	}
}
