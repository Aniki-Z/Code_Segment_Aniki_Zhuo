#include "Block.h"

Block::Block(Game* game, size_t textureIndex)
: Actor(game)
, mIsExplode(false)
{
	SetScale(BLOCK_SCALE);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mMeshComponent->SetTextureIndex(textureIndex);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

	mIsExploding = false;
	if (textureIndex == 4)
	{
		mIsExploding = true;
	}
}

Block::~Block()
{
	GetGame()->RemoveBlock(this);
}

void Block::OnUpdate(float deltaTime)
{
	if (GetPosition().x + INTER_DISTANCE < GetGame()->GetPlayer()->GetPosition().x)
	{
		GetGame()->RemoveBlock(this);
		SetState(ActorState::Destroy);
	}
}
