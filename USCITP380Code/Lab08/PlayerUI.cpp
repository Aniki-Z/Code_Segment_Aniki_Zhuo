#include "PlayerUI.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Actor.h"

PlayerUI::PlayerUI(Actor* owner)
: UIComponent(owner)
{
	Renderer* r = owner->GetGame()->GetRenderer();
	mLapTextures = std::vector<Texture*>{
		r->GetTexture("Assets/UI/Lap1.png"),
		r->GetTexture("Assets/UI/FinalLap.png"),
	};

	mFirstTexture = r->GetTexture("Assets/UI/1st.png");
	mSecondTexture = r->GetTexture("Assets/UI/2nd.png");

	mGoTexture = r->GetTexture("Assets/UI/Go.png");
	mReadyTexture = r->GetTexture("Assets/UI/Ready.png");
}

void PlayerUI::Update(float deltaTime)
{
	mGoDisplayTimer -= deltaTime;
	mLapDisplayTimer -= deltaTime;
}

void PlayerUI::Draw(Shader* shader)
{
	if (mGoDisplayTimer > 0.0f)
	{
		if (mOwner->GetState() == ActorState::Paused)
		{
			DrawTexture(shader, mReadyTexture, Vector2(0.0f, 100.0f));
		}
		else if (mOwner->GetState() == ActorState::Active)
		{
			DrawTexture(shader, mGoTexture, Vector2(0.0f, 100.0f));
		}
	}

	if (mLapDisplayTimer > 0.0f)
	{
		DrawTexture(shader, mLapTextures[mLapIndex], Vector2(0.0f, 200.0f), 0.75f);
	}

	if (mRaceState == Won)
	{
		DrawTexture(shader, mFirstTexture, Vector2(0.0f, 100.0f));
	}
	else if (mRaceState == Lost)
	{
		DrawTexture(shader, mSecondTexture, Vector2(0.0f, 100.0f));
	}

	// Figure out what place to show
	if (mOwner->GetState() == ActorState::Active)
	{
		bool inFirst = IsPlayerInFirst();
		if (inFirst)
		{
			DrawTexture(shader, mFirstTexture, Vector2(400.0f, 320.0f), 0.5f);
		}
		else
		{
			DrawTexture(shader, mSecondTexture, Vector2(400.0f, 320.0f), 0.5f);
		}
	}
}

void PlayerUI::OnLapChange(int lapNum)
{
	if (lapNum >= 1 && lapNum <= mLapTextures.size())
	{
		mLapIndex = lapNum - 1;
		mLapDisplayTimer = 3.0f;
	}
}

bool PlayerUI::IsPlayerInFirst() const
{
	PlayerMove* player = GetGame()->GetPlayer()->GetPlayerMove();
	EnemyMove* enemy = GetGame()->GetEnemy()->GetEnemyMove();

	int playerLap = player->GetLap();
	int enemyLap = enemy->GetLap();
	int playerCheckpointIndex = player->GetChekPoint();
	int enemyCheckpointIndex = enemy->GetChekPoint();

	if (playerLap != enemyLap)
	{
		return playerLap > enemyLap;
	}
	else if (playerCheckpointIndex != enemyCheckpointIndex)
	{
		return playerCheckpointIndex > enemyCheckpointIndex;
	}
	else
	{
		return player->GetDistanceToNextCheckpoint() <= enemy->GetDistanceToNextCheckpoint();
	}
}
