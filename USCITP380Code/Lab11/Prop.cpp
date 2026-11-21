#include "Prop.h"

Prop::Prop(Game* game, const std::string& meshFile, bool usesAlpha, bool hasCollision)
: Actor(game)
, mCollisionComponent(nullptr)
, mMeshComponent(nullptr)
, mHasCollision(hasCollision)
, mMeshFile(meshFile)
{
	mMeshComponent = new MeshComponent(this, usesAlpha);
	mMeshComponent->SetMesh(game->GetRenderer()->GetMesh(meshFile));

	if (hasCollision)
	{
		mCollisionComponent = new CollisionComponent(this);
		Mesh* mesh = GetGame()->GetRenderer()->GetMesh(mMeshFile);
		if (mesh)
		{
			mCollisionComponent->SetSize(mesh->GetWidth(), mesh->GetHeight(), mesh->GetDepth());
		}

		game->AddCollider(this);
	}
}

Prop::~Prop()
{
	if (mHasCollision)
	{
		GetGame()->RemoveCollider(this);
	}
}