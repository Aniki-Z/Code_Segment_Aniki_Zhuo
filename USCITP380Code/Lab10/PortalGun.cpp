#include "PortalGun.h"

PortalGun::PortalGun(Game* game)
: Actor(game)
{
	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(SIZE, SIZE, SIZE);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/PortalGun.gpmesh"));
}

void PortalGun::OnUpdate(float deltaTime)
{
	float rotationSpeed = Math::Pi * deltaTime;
	SetRotation(GetRotation() + rotationSpeed);

	if (GetGame()->GetPlayer()->GetCollisionComponent()->Intersect(mCollisionComponent))
	{
		this->SetState(ActorState::Destroy);
		GetGame()->GetPlayer()->HasGunNow();
	}
}