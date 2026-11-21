#include "PlayerMove.h"
#include "Random.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
, mVelocity(Vector3(400.0f, 0.0f, 0.0f))
, mNextBlock(0.0f)
, mLastBlockIndex(0)
, mBullet(nullptr)
, mInvulnerableTime(0.0f)
, mSpeedMultiplier(1.0f)
, mElapsedTime(0.0f)
, mIsRolling(false)
, mRollTimer(0.0f)
{
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	float yMove = 0.0f;
	float zMove = 0.0f;

	if (keyState[SDL_SCANCODE_W])
	{
		zMove += MOVE_SPEED;
	}
	if (keyState[SDL_SCANCODE_S])
	{
		zMove -= MOVE_SPEED;
	}
	if (keyState[SDL_SCANCODE_D])
	{
		yMove += MOVE_SPEED;
	}
	if (keyState[SDL_SCANCODE_A])
	{
		yMove -= MOVE_SPEED;
	}

	if (keyState[SDL_SCANCODE_Q] && !mPrevKeyState[SDL_SCANCODE_Q] && !mIsRolling)
	{
		mIsRolling = true;
		mRollTimer = 0.5f;

		AddingShield();
	}

	if (keyState[SDL_SCANCODE_SPACE] && !mPrevKeyState[SDL_SCANCODE_SPACE])
	{
		mBullet = new Bullet(GetGame());
		mBullet->SetPosition(mOwner->GetPosition());
		mBullet->SetRotation(mOwner->GetRotation());
		GetGame()->GetAudio()->PlaySound("Shoot.wav");
	}

	memcpy(mPrevKeyState, keyState, SDL_NUM_SCANCODES);

	mVelocity.y = yMove;
	mVelocity.z = zMove;
}

void PlayerMove::Update(float deltaTime)
{
	MoveComponent::Update(deltaTime);

	UpdateShieldAndAudio(deltaTime);
	UpdateSpeed(deltaTime);
	UpdatePosition(deltaTime);
	HandleCollisions();
	UpdateCameraView();
	SpawnSideBlocks();
	LoadObstacleBlocks();
	UpdateRolling(deltaTime);
}

void PlayerMove::UpdateShieldAndAudio(float deltaTime)
{
	Player* player = mOwner->GetGame()->GetPlayer();
	AudioSystem* audio = mOwner->GetGame()->GetAudio();

	if (player->GetShield() != 0)
	{
		audio->PlaySound("ShipLoop.ogg");
	}

	if (player->GetShield() == 1)
	{
		if (audio->GetSoundState(mAlarm) != SoundState::Playing)
		{
			mAlarm = audio->PlaySound("DamageAlert.ogg", true);
		}
	}
	else if (audio->GetSoundState(mAlarm) == SoundState::Playing)
	{
		audio->StopSound(mAlarm);
	}
}

void PlayerMove::UpdateSpeed(float deltaTime)
{
	mElapsedTime += deltaTime;
	if (mElapsedTime >= MAX_ELPASED_TIME)
	{
		mSpeedMultiplier += SPEED_MULTIPLIER;
		mElapsedTime = 0.0f;
	}

	if (mInvulnerableTime > 0.0f)
	{
		mInvulnerableTime -= deltaTime;
	}
}

void PlayerMove::UpdatePosition(float deltaTime)
{
	Vector3 position = mOwner->GetPosition();
	mVelocity.x = 400.0f * GetGame()->GetPlayer()->GetPlayerMove()->GetSpeedMultiplier();
	position += mVelocity * deltaTime;

	position.y = Math::Clamp(position.y, -Y_MAX, Y_MAX);
	position.z = Math::Clamp(position.z, -X_MAX, X_MAX);

	mOwner->SetPosition(position);
}

void PlayerMove::HandleCollisions()
{
	Player* player = mOwner->GetGame()->GetPlayer();
	std::vector<Block*> blocks = mOwner->GetGame()->GetBlocks();

	for (Block* block : blocks)
	{
		if (player->GetCollisionComponent()->Intersect(block->GetCollisionComponent()))
		{
			block->SetState(ActorState::Destroy);

			if (mInvulnerableTime <= 0.0f)
			{
				player->ReduceShield();
				mInvulnerableTime = 1.0f;
			}

			if (player->GetShield() > 0)
			{
				mOwner->GetGame()->GetAudio()->PlaySound("ShipHit.wav");
			}
			else
			{
				mOwner->GetGame()->GetAudio()->PlaySound("ShipDie.wav");
			}

			if (player->GetShield() <= 0)
			{
				mOwner->SetState(ActorState::Paused);
				mOwner->GetGame()->GetAudio()->StopSound(mAlarm);
			}
		}
	}
}

void PlayerMove::UpdateCameraView()
{
	Vector3 position = mOwner->GetPosition();
	Vector3 eye = position;
	eye.x -= H_DISTANCE;
	eye.z = 0.0f;

	Vector3 target = position;
	target.x += TARGET_DISTANCE;

	Matrix4 view = Matrix4::CreateLookAt(eye, target, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(view);
}

void PlayerMove::SpawnSideBlocks()
{
	while (mNextBlock < mOwner->GetPosition().x + CHECK_DISTANCE)
	{
		Game* game = mOwner->GetGame();
		int leftrightorder = static_cast<int>(mNextBlock / 500) % 4;
		if (leftrightorder == 3)
		{
			leftrightorder = 0;
		}

		int toporder = static_cast<int>(mNextBlock / 500) % 2 + 6;

		SideBlock* leftBlock = new SideBlock(game, leftrightorder);
		leftBlock->SetPosition(Vector3(mNextBlock, SIDE_BLOCKS_FIX_POSITION, 0.0f));

		SideBlock* rightBlock = new SideBlock(game, leftrightorder);
		rightBlock->SetPosition(Vector3(mNextBlock, -SIDE_BLOCKS_FIX_POSITION, 0.0f));

		SideBlock* downBlock = new SideBlock(game, 5);
		downBlock->SetPosition(Vector3(mNextBlock, 0.0f, -SIDE_BLOCKS_FIX_POSITION));

		SideBlock* upBlock = new SideBlock(game, toporder);
		upBlock->SetPosition(Vector3(mNextBlock, 0.0f, SIDE_BLOCKS_FIX_POSITION));

		mNextBlock += SIDE_BLOCKS_FIX_POSITION;
	}
}

void PlayerMove::LoadObstacleBlocks()
{
	int maxLoadIndex = static_cast<int>((mOwner->GetPosition().x + CHECK_DISTANCE) / 1000.0f) + 1;

	while (mLastBlockIndex < maxLoadIndex)
	{
		int fileIndex = (mLastBlockIndex > 20) ? Random::GetIntRange(1, 20) : mLastBlockIndex;
		mOwner->GetGame()->LoadBlocks(std::to_string(fileIndex) + ".txt");
		mLastBlockIndex++;
	}
}

void PlayerMove::UpdateRolling(float deltaTime)
{
	if (mIsRolling)
	{
		mRollTimer -= deltaTime;
		float rollSpeed = ROLL_LINEAR_SPEED * Math::Pi;

		if (mRollTimer > 0.0f)
		{
			mOwner->SetRollAngle(mOwner->GetRollAngle() + rollSpeed * deltaTime);
		}
		else
		{
			mIsRolling = false;
			mOwner->SetRollAngle(0.0f);
		}
	}
}

void PlayerMove::AddingShield()
{
	mOwner->GetGame()->GetPlayer()->AddShield();
	GetGame()->GetAudio()->PlaySound("Boost.wav");
}
