#include "EnergyLauncher.h"
#include "Door.h"

EnergyLauncher::EnergyLauncher(Game* game, std::string door, float coolDown)
: Actor(game)
, mCooldown(coolDown)
, mTimer(0.0f)
, mDoorName(door)
, mPellet(nullptr)
, mDoorOpen(false)
{
	Mesh* mMesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshes/EnergyLauncher.gpmesh");

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(mMesh);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(50.0f, 50.0f, 50.0f);

	GetGame()->AddCollider(this);
}

EnergyLauncher::~EnergyLauncher()
{
	GetGame()->RemoveCollider(this);
}

void EnergyLauncher::OnUpdate(float deltaTime)
{
	if (!mDoorOpen)
	{
		mTimer -= deltaTime;
		if (mTimer <= 0.0f)
		{
			mPellet = new Pellet(GetGame());

			Vector3 spawnPos = GetPosition() + GetForward() * SPAWN;
			mPellet->SetPosition(spawnPos);
			mPellet->SetRotation(GetRotation());
			mPellet->CalcWorldTransform();

			Vector3 pelletVel = GetForward() * SPEED;
			mPellet->SetVelocity(pelletVel);

			mTimer = mCooldown;
		}
	}
}
