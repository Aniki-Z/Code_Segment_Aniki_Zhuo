#include "PlayerMesh.h"

PlayerMesh::PlayerMesh(Game* game)
: Actor(game)
{
	SetScale(mScale);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/PortalGun.gpmesh"));
}

void PlayerMesh::OnUpdate(float deltaTime)
{
	Vector3 screenPos(300.0f, -250.0f, 0.4f);
	Vector3 worldPos = GetGame()->GetRenderer()->Unproject(screenPos);
	SetPosition(worldPos);

	Player* player = GetGame()->GetPlayer();
	float pitch = player->GetCameraComponent()->GetPitchAngle();
	Quaternion pitchQuat(Vector3::UnitY, pitch);

	float yaw = player->GetRotation();
	Quaternion yawQuat(Vector3::UnitZ, yaw);

	Quaternion finalQuat = Quaternion::Concatenate(pitchQuat, yawQuat);
	SetQuat(finalQuat);
}