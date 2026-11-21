#pragma once
#include "Component.h"
#include "Math.h"
#include <functional>

class HealthComponent : public Component
{
public:
	HealthComponent(class Actor* owner, float health = 100.0f);

	void SetOnDamageCallback(const std::function<void(const Vector3&)>& callback)
	{
		mOnDamage = callback;
	}
	void SetOnDeathCallback(const std::function<void()>& callback) { mOnDeath = callback; }

	float GetHealth() const { return mHealth; }
	bool IsDead() const { return mHealth <= 0.0f; }

	void TakeDamage(float amount, const Vector3& damageLocation);

private:
	float mHealth;
	std::function<void(const Vector3&)> mOnDamage;
	std::function<void()> mOnDeath;
};