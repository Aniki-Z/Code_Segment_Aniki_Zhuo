#include "Game.h"

Spawner::Spawner(Game* game)
: Actor(game)
{
}

void Spawner::OnUpdate(float deltaTime)
{
	Player* mPlayer = GetGame()->GetPlayer();

	float mCreateDistance = abs(GetPosition().x - mPlayer->GetPosition().x);
	if (mCreateDistance <= CREATE_DISTANCE)
	{
		Goomba* goomba = new Goomba(GetGame());
		goomba->SetPosition(GetPosition());
		GetGame()->AddEnemy(goomba);
		SetState(ActorState::Destroy);
	}
}