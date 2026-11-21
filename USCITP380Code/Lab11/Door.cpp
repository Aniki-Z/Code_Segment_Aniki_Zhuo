#include "Door.h"
#include "Mesh.h"

Door::Door(Game* game, std::string name)
: Actor(game)
, mName(name)
, mIsOpen(false)
, mLeftDoor(nullptr)
, mRightDoor(nullptr)
, mIsOpening(false)
, mOpenTime(0.0f)
{
	Mesh* mMesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshes/DoorFrame.gpmesh");

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(mMesh);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(mMesh->GetWidth(), mMesh->GetHeight(), mMesh->GetDepth());

	GetGame()->AddCollider(this);

	mLeftDoor = new Actor(game, this);
	MeshComponent* leftMeshComp = new MeshComponent(mLeftDoor);
	leftMeshComp->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/DoorLeft.gpmesh"));

	mRightDoor = new Actor(game, this);
	MeshComponent* rightMeshComp = new MeshComponent(mRightDoor);
	rightMeshComp->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/DoorRight.gpmesh"));
}

Door::~Door()
{
	GetGame()->RemoveCollider(this);

	delete mLeftDoor;
	delete mRightDoor;
}

void Door::Open()
{
	if (!mIsOpen && !mIsOpening)
	{
		mIsOpening = true;
		mOpenTime = 0.0f;

		GetGame()->RemoveCollider(this);
	}
}

void Door::OnUpdate(float deltaTime)
{
	if (mIsOpening && !mIsOpen)
	{
		mOpenTime += deltaTime;
		float t = mOpenTime / OPEN_DURATION;
		t = Math::Clamp(t, 0.0f, 1.0f);

		Vector3 leftPos = Vector3::Lerp(Vector3::Zero, Vector3(0.0f, -100.0f, 0.0f), t);
		Vector3 rightPos = Vector3::Lerp(Vector3::Zero, Vector3(0.0f, 100.0f, 0.0f), t);

		mLeftDoor->SetPosition(leftPos);
		mRightDoor->SetPosition(rightPos);

		if (t >= 1.0f)
		{
			mIsOpening = false;
			mIsOpen = true;
		}
	}
}