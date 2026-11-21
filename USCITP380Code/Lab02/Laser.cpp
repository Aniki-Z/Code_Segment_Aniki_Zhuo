#include "Laser.h"
#include "Asteroid.h"

Laser::Laser(class Game* game)
: Actor(game)
, mLifetime(1.0f)
{
	mSpriteComponent = new SpriteComponent(this);
	mSpriteComponent->SetTexture(game->GetTexture("Assets/Laser.png"));

	mMoveComponent = new MoveComponent(this);
	mMoveComponent->SetForwardSpeed(LASER_SPEED);
}

void Laser::OnUpdate(float deltaTime)
{
	Actor::OnUpdate(deltaTime);

	mLifetime -= deltaTime;

	if (mLifetime <= 0.0f)
	{
		SetState(ActorState::Destroy);
	}

	std::vector<Asteroid*> mAsteroids = GetGame()->GetAsteroids();
	for (size_t i = 0; i < mAsteroids.size(); i++)
	{
		float distance = Vector2::Distance(GetPosition(), mAsteroids[i]->GetPosition());
		if (distance <= LASER_DISTANCE)
		{
			SetState(ActorState::Destroy);
			mAsteroids[i]->SetState(ActorState::Destroy);
			break;
		}
	}
}