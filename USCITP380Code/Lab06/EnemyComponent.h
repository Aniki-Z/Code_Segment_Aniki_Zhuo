#pragma once
#include "Game.h"
#include "Component.h"
#include "CollisionComponent.h"
#include <functional>

class EnemyComponent : public Component
{
private:
	CollisionComponent* mCollisionComponent;

	int mHitPoints;
	float mInvulnerableTime;
	static constexpr float INVULNERABLE_DURATION = 0.25f;

	std::function<void()> mOnDamageCallback;
	std::function<void()> mOnDeathCallback;

public:
	EnemyComponent(class Actor* owner);
	~EnemyComponent();

	CollisionComponent* GetCollisionComponent() { return mCollisionComponent; };
	void TakeDamage();
	void Update(float deltaTime) override;

	int GetHitPoints() const { return mHitPoints; }
	void SetHitPoints(int points) { mHitPoints = points; }

	void SetOnDamageCallback(std::function<void()> callback)
	{
		mOnDamageCallback = std::move(callback);
	}
	void SetOnDeathCallback(std::function<void()> callback)
	{
		mOnDeathCallback = std::move(callback);
	}
};
