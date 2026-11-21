#include "CollisionComponent.h"
#include "Actor.h"
#include <algorithm>

CollisionComponent::CollisionComponent(class Actor* owner)
: Component(owner)
, mWidth(0.0f)
, mHeight(0.0f)
, mDepth(0.0f)
{
}

CollisionComponent::~CollisionComponent()
{
}

bool CollisionComponent::Intersect(const CollisionComponent* other) const
{
	Vector3 thisMin = GetMin();
	Vector3 thisMax = GetMax();
	Vector3 otherMin = other->GetMin();
	Vector3 otherMax = other->GetMax();

	bool noIntersection = thisMax.x < otherMin.x || thisMax.y < otherMin.y ||
						  thisMax.z < otherMin.z || otherMax.x < thisMin.x ||
						  otherMax.y < thisMin.y || otherMax.z < thisMin.z;

	return !noIntersection;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale().x / 2.0f;
	v.y -= mWidth * mOwner->GetScale().y / 2.0f;
	v.z -= mHeight * mOwner->GetScale().z / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale().x / 2.0f;
	v.y += mWidth * mOwner->GetScale().y / 2.0f;
	v.z += mHeight * mOwner->GetScale().z / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(const CollisionComponent* other, Vector3& offset) const
{
	offset = Vector3::Zero;

	if (!Intersect(other))
	{
		return CollSide::None;
	}

	float backDist = other->GetMin().x - GetMax().x;
	float frontDist = other->GetMax().x - GetMin().x;
	float leftDist = other->GetMin().y - GetMax().y;
	float rightDist = other->GetMax().y - GetMin().y;
	float bottomDist = other->GetMin().z - GetMax().z;
	float topDist = other->GetMax().z - GetMin().z;

	float minOverlap = std::min({Math::Abs(leftDist), Math::Abs(rightDist), Math::Abs(topDist),
								 Math::Abs(bottomDist), Math::Abs(backDist), Math::Abs(frontDist)});

	if (minOverlap == Math::Abs(backDist))
	{
		offset.x = backDist;
		return CollSide::Back;
	}
	if (minOverlap == Math::Abs(frontDist))
	{
		offset.x = frontDist;
		return CollSide::Front;
	}
	if (minOverlap == Math::Abs(leftDist))
	{
		offset.y = leftDist;
		return CollSide::Left;
	}
	if (minOverlap == Math::Abs(rightDist))
	{
		offset.y = rightDist;
		return CollSide::Right;
	}
	if (minOverlap == Math::Abs(topDist))
	{
		offset.z = topDist;
		return CollSide::Top;
	}
	if (minOverlap == Math::Abs(bottomDist))
	{
		offset.z = bottomDist;
		return CollSide::Bottom;
	}

	return CollSide::None;
}
