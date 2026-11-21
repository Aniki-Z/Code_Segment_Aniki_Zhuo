#include "VOTrigger.h"
#include "Game.h"
#include "EnergyLauncher.h"
#include "Door.h"

VOTrigger::VOTrigger(Game* game, std::string doorName, std::string nextLevel,
					 std::vector<std::string> sounds, std::vector<std::string> subtitles)
: Actor(game)
, mDoorName(doorName)
, mNextLevel(nextLevel)
, mSounds(sounds)
, mSubtitles(subtitles)
, mCurrentSoundIndex(0)
, mHasActivated(false)
, mPrevFKeyState(false)
{
	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(1.0f, 1.0f, 1.0f);

	mSound = game->GetAudio();
}

void VOTrigger::OnUpdate(float deltaTime)
{
	Player* player = GetGame()->GetPlayer();
	if (player->GetPlayerMove()->GetDead() &&
		mSound->GetSoundState(mCurrentSoundHandle) != SoundState::Stopped)
	{
		mSound->StopSound(mCurrentSoundHandle);
	}

	if (mHasActivated)
	{
		if (mSound->GetSoundState(mCurrentSoundHandle) == SoundState::Stopped)
		{
			PlayNextSound();
		}
	}
	else
	{
		if (mCollisionComponent->Intersect(player->GetCollisionComponent()))
		{
			mHasActivated = true;
			PlayNextSound();
		}
	}
}

void VOTrigger::OnProcessInput(const Uint8* keyState, Uint32 mouseButtons,
							   const Vector2& relativeMouse)
{
	bool isFPressed = keyState[SDL_SCANCODE_F] != 0;

	if (isFPressed && !mPrevFKeyState && mHasActivated)
	{
		if (mSound->GetSoundState(mCurrentSoundHandle) == SoundState::Playing)
		{
			mSound->StopSound(mCurrentSoundHandle);
		}
	}

	mPrevFKeyState = isFPressed;
}

void VOTrigger::PlayNextSound()
{
	Player* player = GetGame()->GetPlayer();
	if (mCurrentSoundIndex < mSounds.size())
	{
		mCurrentSoundHandle = mSound->PlaySound(mSounds[mCurrentSoundIndex]);

		if (mCurrentSoundIndex < mSubtitles.size() && player->GetHUD())
		{
			player->GetHUD()->ShowSubtitle(mSubtitles[mCurrentSoundIndex]);
		}

		mCurrentSoundIndex++;
	}
	else
	{
		if (player->GetHUD())
		{
			player->GetHUD()->ShowSubtitle("");
		}

		for (auto collider : GetGame()->GetColliders())
		{
			Door* door = nullptr;
			door = dynamic_cast<Door*>(collider);
			if (door && mDoorName == door->GetName())
			{
				door->Open();
			}
		}

		if (!mNextLevel.empty())
		{
			mGame->SetNextLevel(mNextLevel);
		}

		SetState(ActorState::Destroy);
	}
}