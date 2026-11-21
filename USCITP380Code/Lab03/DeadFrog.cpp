#include "DeadFrog.h"

DeadFrog::DeadFrog(Game* game, const Vector2& position)
: Actor(game)
, mTimeToDestroy(0.0f)
{
	SetPosition(position);
	mSpriteComponent = new SpriteComponent(this);
	mSpriteComponent->SetTexture(game->GetTexture("Assets/Dead.png"));
}

void DeadFrog::OnUpdate(float deltaTime)
{
	mTimeToDestroy += deltaTime;

	if (mTimeToDestroy > INTERVAL_MAX)
	{
		GetGame()->RemoveActor(this);
	}

	Actor::OnUpdate(deltaTime);
}
