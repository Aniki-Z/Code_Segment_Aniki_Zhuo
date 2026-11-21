#include "HealthComponent.h"

HealthComponent::HealthComponent(class Actor* owner, float health)
: Component(owner)
, mHealth(health)
{
}

void HealthComponent::TakeDamage(float amount, const Vector3& damageLocation)
{
	if (IsDead())
	{
		return;
	}

	mHealth -= amount;

	if (mOnDamage)
	{
		mOnDamage(damageLocation);
	}

	if (IsDead() && mOnDeath)
	{
		mOnDeath();
	}
}