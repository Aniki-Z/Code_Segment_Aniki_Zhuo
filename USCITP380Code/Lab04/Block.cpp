#include "Block.h"

Block::Block(Game* game, char mType)
: Actor(game)
{
	std::string texture = "Assets/Block";
	texture += mType;
	texture += ".png";

	mSpriteComponent = new SpriteComponent(this);
	mSpriteComponent->SetTexture(game->GetTexture(texture));

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(BLOCK_SIZE, BLOCK_SIZE);
}

Block::~Block()
{
	GetGame()->RemoveBlock(this);
}