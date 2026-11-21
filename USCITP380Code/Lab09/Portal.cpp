#include "Portal.h"
#include "Game.h"
#include "PortalMeshComponent.h"
#include "Renderer.h"

Portal::Portal(class Game* game, bool isBlue)
: Actor(game)
{
	PortalMeshComponent* mPortalMeshComponent = new PortalMeshComponent(this);
	MeshComponent* mMeshComponent = new MeshComponent(this, true);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Portal.gpmesh"));

	if (isBlue)
	{
		mPortalMeshComponent->SetTextureIndex(0);
		mMeshComponent->SetTextureIndex(2);
	}
	else
	{
		mPortalMeshComponent->SetTextureIndex(1);
		mMeshComponent->SetTextureIndex(3);
	}
}
