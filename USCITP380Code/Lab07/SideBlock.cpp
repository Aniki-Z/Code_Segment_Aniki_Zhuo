#include "SideBlock.h"

SideBlock::SideBlock(Game* game, size_t textureIndex)
: Actor(game)
{
	SetScale(BLOCK_SCALE);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));

	mMeshComponent->SetTextureIndex(textureIndex);
}

void SideBlock::OnUpdate(float deltaTime)
{
	if (GetPosition().x < GetGame()->GetPlayer()->GetPosition().x - 2000.0f)
	{
		SetState(ActorState::Destroy);
	}
}
