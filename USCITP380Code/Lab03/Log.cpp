#include "Log.h"

Log::Log(Game* game, char mType, int row)
: Actor(game)
, mRow(row)
{
	mSpriteComponent = new SpriteComponent(this, 10);
	mCollisionComponent = new CollisionComponent(this);

	switch (mType)
	{
	case 'X':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/LogX.png"));
		mCollisionComponent->SetSize(LOG_X_WIDTH, LOG_HEIGHT);
		break;
	case 'Y':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/LogY.png"));
		mCollisionComponent->SetSize(LOG_Y_WIDTH, LOG_HEIGHT);
		break;
	case 'Z':
		mSpriteComponent->SetTexture(game->GetTexture("Assets/LogZ.png"));
		mCollisionComponent->SetSize(LOG_Z_WIDTH, LOG_HEIGHT);
		break;
	default:
		break;
	}

	mMove = new WrappingMove(this);
	mMove->SetForwardSpeed(LOG_SPEED);

	if (mRow % 2 == 0)
	{
		mMove->SetDirection(Vector2(1.0f, 0.0f));
	}
	else
	{
		mMove->SetDirection(Vector2(-1.0f, 0.0f));
	}
}

Log::~Log()
{
	GetGame()->RemoveLog(this);
}