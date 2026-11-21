#include "EnergyCatcher.h"
#include "Door.h"
#include "EnergyLauncher.h"

EnergyCatcher::EnergyCatcher(Game* game, std::string door)
: Actor(game)
, mDoorName(door)
, mActive(false)
{
	Mesh* mMesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshes/EnergyCatcher.gpmesh");

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(mMesh);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(50.0f, 50.0f, 50.0f);

	GetGame()->AddCollider(this);
}

EnergyCatcher::~EnergyCatcher()
{
	GetGame()->RemoveCollider(this);
}

void EnergyCatcher::OnUpdate(float deltaTime)
{
	if (mActive && !mDoorName.empty())
	{
		for (auto collider : GetGame()->GetColliders())
		{
			Door* door = nullptr;
			door = dynamic_cast<Door*>(collider);
			if (door && mDoorName == door->GetName())
			{
				door->Open();
			}

			EnergyLauncher* energyLauncher = nullptr;
			energyLauncher = dynamic_cast<EnergyLauncher*>(collider);
			if (energyLauncher && mDoorName == energyLauncher->GetName())
			{
				energyLauncher->SetOpen(true);
			}
		}
	}
}