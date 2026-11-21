#include "WrappingMove.h"

WrappingMove::WrappingMove(Actor* mOwner)
: MoveComponent(mOwner)
, mDirection(Vector2::Zero)
{
}

void WrappingMove::Update(float deltaTime)
{
	Vector2 pos = mOwner->GetPosition();
	pos += mDirection * mForwardSpeed * deltaTime;

	if (pos.x <= 0.0f)
	{
		pos.x = SCREEN_WIDTH;
	}
	else if (pos.x >= SCREEN_WIDTH)
	{
		pos.x = 0.0f;
	}

	mOwner->SetPosition(pos);
}
