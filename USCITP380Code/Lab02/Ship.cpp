#include "Ship.h"
#include "Laser.h"

Ship::Ship(Game* game)
: Actor(game)
, mCooldown(0.0f)
{
	mSpriteComponent = new SpriteComponent(this);
	mSpriteComponent->SetTexture(game->GetTexture("Assets/Ship.png"));

	mMoveComponent = new MoveComponent(this);
}

void Ship::OnProcessInput(const uint8_t* keyState)
{
	float mAngularSpeed = 0.0f;
	float mForwardSpeed = 0.0f;

	if (keyState[SDL_SCANCODE_A])
	{
		mAngularSpeed += SHIP_ANGULAR;
	}

	if (keyState[SDL_SCANCODE_D])
	{
		mAngularSpeed -= SHIP_ANGULAR;
	}

	if (keyState[SDL_SCANCODE_W])
	{
		mForwardSpeed += SHIP_SPEED;
	}

	if (keyState[SDL_SCANCODE_S])
	{
		mForwardSpeed -= SHIP_SPEED;
	}

	mMoveComponent->SetAngularSpeed(mAngularSpeed);
	mMoveComponent->SetForwardSpeed(mForwardSpeed);

	if (mForwardSpeed == 0.0f)
	{
		mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/Ship.png"));
	}
	else
	{
		mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/ShipThrust.png"));
	}

	if (keyState[SDL_SCANCODE_SPACE] && mCooldown <= 0.0f)
	{
		Laser* mLaser = new Laser(GetGame());

		mLaser->SetPosition(GetPosition());
		mLaser->SetRotation(GetRotation());

		mCooldown = 1.0f;
	}
}

void Ship::OnUpdate(float deltaTime)
{
	Actor::OnUpdate(deltaTime);

	if (mCooldown > 0.0f)
	{
		mCooldown -= deltaTime;
	}
}