#include "Asteroid.h"

Asteroid::Asteroid(Game* game)
: Actor(game)
{
	mSpriteComponent = new SpriteComponent(this);
	mSpriteComponent->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	mMoveComponent = new MoveComponent(this);
	mMoveComponent->SetForwardSpeed(ASTEROID_SPEED);

	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));
	SetPosition(
		Random::GetVector(Vector2(0.0f, Game::WINDOW_HEIGHT), Vector2(Game::WINDOW_WIDTH, 0.0f)));

	game->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}

void Asteroid::OnUpdate(float deltaTime)
{
	Actor::OnUpdate(deltaTime);

	Vector2 mCurrentPosition = GetPosition();

	if (mCurrentPosition.x < 0.0f)
	{
		mCurrentPosition.x = Game::WINDOW_WIDTH;
	}
	else if (mCurrentPosition.x > Game::WINDOW_WIDTH)
	{
		mCurrentPosition.x = 0.0f;
	}

	if (mCurrentPosition.y < 0.0f)
	{
		mCurrentPosition.y = Game::WINDOW_HEIGHT;
	}
	else if (mCurrentPosition.y > Game::WINDOW_HEIGHT)
	{
		mCurrentPosition.y = 0.0f;
	}

	SetPosition(mCurrentPosition);
}