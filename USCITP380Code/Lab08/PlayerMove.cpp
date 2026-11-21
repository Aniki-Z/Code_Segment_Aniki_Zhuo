#include "PlayerMove.h"
#include "Random.h"
#include "Game.h"

PlayerMove::PlayerMove(Actor* owner)
: VehicleMove(owner)
{
	mHeightMap = mOwner->GetGame()->GetHeightMap();
	Vector3 initialposition = mHeightMap->CellToWorld(39, 58);
	mOwner->SetPosition(initialposition);
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	if (keyState[SDL_SCANCODE_W])
	{
		SetAccelerate(true);
	}
	else
	{
		SetAccelerate(false);
	}

	if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D])
	{
		SetCarState(TurnState::TurnLeft);
	}
	else if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A])
	{
		SetCarState(TurnState::TurnRight);
	}
	else
	{
		SetCarState(TurnState::NotTurn);
	}
}

void PlayerMove::Update(float deltaTime)
{
	VehicleMove::Update(deltaTime);
}

void PlayerMove::OnLapChange(int newLap)
{
	if (newLap < THIRD_LAP)
	{
		mOwner->GetGame()->GetPlayer()->GetPlayerUI()->OnLapChange(newLap);
		if (newLap == SECOND_LAP)
		{
			mOwner->GetGame()->GetAudio()->StopSound(mOwner->GetGame()->GetSound(), 250);
			mOwner->GetGame()->GetAudio()->PlaySound("FinalLap.wav", false);
			mOwner->GetGame()->SetSound(
				mOwner->GetGame()->GetAudio()->PlaySound("MusicFast.ogg", true, 4000));
		}
	}
	else
	{
		mOwner->GetGame()->GetAudio()->StopSound(mOwner->GetGame()->GetSound(), 250);

		if (mOwner->GetGame()->GetPlayer()->GetPlayerMove()->GetLap() >
			mOwner->GetGame()->GetEnemy()->GetEnemyMove()->GetLap())
		{
			mOwner->GetGame()->GetPlayer()->GetPlayerUI()->SetRaceState(PlayerUI::Won);
			mOwner->GetGame()->GetAudio()->PlaySound("Won.wav", false);
		}
		else
		{
			mOwner->GetGame()->GetPlayer()->GetPlayerUI()->SetRaceState(PlayerUI::Lost);
			mOwner->GetGame()->GetAudio()->PlaySound("Lost.wav", false);
		}
		mOwner->GetGame()->GetPlayer()->SetState(ActorState::Paused);
		mOwner->GetGame()->GetEnemy()->SetState(ActorState::Paused);
	}
}
