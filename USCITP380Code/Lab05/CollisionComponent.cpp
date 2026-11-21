#include "CollisionComponent.h"
#include "Actor.h"
#include <algorithm>

CollisionComponent::CollisionComponent(class Actor* owner)
: Component(owner)
, mWidth(0.0f)
, mHeight(0.0f)
{
}

CollisionComponent::~CollisionComponent()
{
}

bool CollisionComponent::Intersect(const CollisionComponent* other) const
{
	bool isNotOverlap = (GetMax().x <= other->GetMin().x || GetMin().x >= other->GetMax().x ||
						 GetMax().y <= other->GetMin().y || GetMin().y >= other->GetMax().y);
	return !isNotOverlap;
}

Vector2 CollisionComponent::GetMin() const
{
	float mScale = mOwner->GetScale();
	Vector2 mPosition = mOwner->GetPosition();
	return Vector2(mPosition.x - (mWidth * mScale) / 2.0f, mPosition.y - (mHeight * mScale) / 2.0f);
}

Vector2 CollisionComponent::GetMax() const
{
	float mScale = mOwner->GetScale();
	Vector2 mPosition = mOwner->GetPosition();
	return Vector2(mPosition.x + (mWidth * mScale) / 2.0f, mPosition.y + (mHeight * mScale) / 2.0f);
}

const Vector2& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(const CollisionComponent* other, Vector2& offset) const
{
	offset = Vector2::Zero;

	if (!Intersect(other))
	{
		return CollSide::None;
	}

	float leftDist = other->GetMin().x - GetMax().x;
	float rightDist = other->GetMax().x - GetMin().x;
	float topDist = other->GetMin().y - GetMax().y;
	float bottomDist = other->GetMax().y - GetMin().y;

	float mMinOverlapX = std::min(std::abs(leftDist), std::abs(rightDist));
	float mMinOverlapY = std::min(std::abs(topDist), std::abs(bottomDist));

	if (mMinOverlapX < mMinOverlapY)
	{
		if (std::abs(leftDist) < std::abs(rightDist))
		{
			offset.x = leftDist;
			return CollSide::Left;
		}
		else
		{
			offset.x = rightDist;
			return CollSide::Right;
		}
	}
	else
	{
		if (std::abs(topDist) < std::abs(bottomDist))
		{
			offset.y = topDist;
			return CollSide::Top;
		}
		else
		{
			offset.y = bottomDist;
			return CollSide::Bottom;
		}
	}
}
