#include "Vehicle.h"

Vehicle::Vehicle(Game* game, char mType, int row)
: Actor(game)
, mRow(row)
{
	mSpriteComponent = new SpriteComponent(this, 10);
	mCollisionComponent = new CollisionComponent(this);

	switch (mType)
	{
	case 'A':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/CarA.png"));
		mCollisionComponent->SetSize(CAR_SIZE, CAR_SIZE);
		break;
	case 'B':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/CarB.png"));
		mCollisionComponent->SetSize(CAR_SIZE, CAR_SIZE);
		break;
	case 'C':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/CarC.png"));
		mCollisionComponent->SetSize(CAR_SIZE, CAR_SIZE);
		break;
	case 'D':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/CarD.png"));
		mCollisionComponent->SetSize(CAR_SIZE, CAR_SIZE);
		break;
	case 'T':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/Truck.png"));
		mCollisionComponent->SetSize(TRACK_XSIZE, TRACK_YSIZE);
		break;
	default:
		break;
	}

	mMove = new WrappingMove(this);
	mMove->SetForwardSpeed(MOVE_SPEED);

	if (mRow % 2 == 0)
	{
		mMove->SetDirection(Vector2(1.0f, 0.0f));
	}
	else
	{
		mMove->SetDirection(Vector2(-1.0f, 0.0f));
	}
}

Vehicle::~Vehicle()
{
	GetGame()->RemoveVehicle(this);
}

void Vehicle::OnUpdate(float deltaTime)
{
	Frog* mFrog = GetGame()->GetFrog();
	Vector2 mFrogPosition = mFrog->GetPosition();

	Vector2 mVehiclePosition = GetPosition();
	Vector2 mVehicleDirection = mMove->GetDirection();

	Vector2 mToFrog = mFrogPosition - mVehiclePosition;
	Vector2 mToFrogOnX(mToFrog.x, 0.0f);

	float mDot = Vector2::Dot(mToFrog, mToFrogOnX);
	float mToFrogMagnitude = std::sqrt((mToFrog.y * mToFrog.y) + (mToFrog.x * mToFrog.x));
	float mToFrogOnXMagnitude = std::sqrt((mToFrog.x * mToFrog.x));
	float mAngle = std::acos(mDot / (mToFrogMagnitude * mToFrogOnXMagnitude));

	if (mToFrog.x * mVehicleDirection.x > 0 && mAngle < Math::Pi / 12 && mAngle > -Math::Pi / 12)
	{
		mMove->SetForwardSpeed(MOVE_SPEED / 2);
	}
	else
	{
		mMove->SetForwardSpeed(MOVE_SPEED);
	}

	Actor::OnUpdate(deltaTime);
}