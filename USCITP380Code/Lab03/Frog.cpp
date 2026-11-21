#include "Frog.h"
#include <unordered_map>

Frog::Frog(Game* game)
: Actor(game)
{
	mSpriteComponent = new SpriteComponent(this);
	mSpriteComponent->SetTexture(game->GetTexture("Assets/Frog.png"));

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(FROG_SIZE, FROG_SIZE);
}

Frog::~Frog()
{
}

void Frog::OnProcessInput(const uint8_t* keyState)
{
	Vector2 mNewPosition = GetPosition();

	static std::unordered_map<SDL_Scancode, bool> mLastFrame;

	SDL_Scancode keys[] = {SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D};
	Vector2 movements[] = {Vector2(0.0f, -FROG_MOVE), Vector2(0.0f, FROG_MOVE),
						   Vector2(-FROG_MOVE, 0.0f), Vector2(FROG_MOVE, 0.0f)};

	for (size_t i = 0; i < 4; i++)
	{
		SDL_Scancode key = keys[i];
		Vector2 move = movements[i];

		if (keyState[key] && !mLastFrame[key])
		{
			mNewPosition += move;
		}

		mLastFrame[key] = keyState[key];
	}

	mNewPosition.x = Math::Clamp(mNewPosition.x, 0.0f, WIDTH_MAX);
	mNewPosition.y = Math::Clamp(mNewPosition.y, FROG_CLAMP, HEIGHT_MAX);

	SetPosition(mNewPosition);
}

void Frog::OnUpdate(float deltaTime)
{
	Game* game = GetGame();
	std::vector<Vehicle*>& mVehicle = game->GetVehicles();

	for (size_t i = 0; i < mVehicle.size(); i++)
	{
		const Vehicle* vehicle = mVehicle[i];
		if (mCollisionComponent->Intersect(vehicle->GetCollisionComponent()))
		{
			new DeadFrog(game, GetPosition());
			SetPosition(game->GetStartPosition());
			break;
		}
	}

	bool mInRiver = false;

	std::vector<Log*>& mLog = game->GetLogs();

	for (size_t i = 0; i < mLog.size(); i++)
	{
		const Log* log = mLog[i];
		Vector2 mOffset;
		CollSide mResult = mCollisionComponent->GetMinOverlap(log->GetCollisionComponent(),
															  mOffset);

		if (mResult != CollSide::None)
		{
			mInRiver = true;

			Vector2 newPosition =
				Vector2(GetPosition().x + log->GetWrappingMove()->GetDirection().x *
											  log->GetWrappingMove()->GetForwardSpeed() * deltaTime,
						log->GetPosition().y);
			SetPosition(newPosition);

			if (mResult == CollSide::Left)
			{
				newPosition.x += mOffset.x + FROG_POSITION_CHANGE;
			}
			else if (mResult == CollSide::Right)
			{
				newPosition.x += mOffset.x - FROG_POSITION_CHANGE;
			}

			SetPosition(newPosition);
		}
	}

	if (!mInRiver && GetPosition().y >= DEAD_FROG_LOWER && GetPosition().y <= DEAD_FROG_UPPER)
	{
		new DeadFrog(game, GetPosition());
		SetPosition(game->GetStartPosition());
	}

	if (mCollisionComponent->Intersect(game->GetGoalCollisionComponent()))
	{
		SetPosition(game->GetGoalPosition());
		SetState(ActorState::Paused);
	}

	if (GetState() != ActorState::Paused && GetPosition().y < DEAD_FROG_LOWER)
	{
		new DeadFrog(game, GetPosition());
		SetPosition(game->GetStartPosition());
	}

	Actor::OnUpdate(deltaTime);
}