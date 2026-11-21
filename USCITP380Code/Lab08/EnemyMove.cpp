#include "EnemyMove.h"
#include "CSVHelper.h"
#include <fstream>
#include "Game.h"

EnemyMove::EnemyMove(Actor* owner)
: VehicleMove(owner)
, mNextTargetIndex(1)
{
	std::ifstream pathFile("Assets/HeightMap/Path.csv");
	if (pathFile)
	{
		std::string line;
		std::getline(pathFile, line);
		while (std::getline(pathFile, line))
		{
			std::vector<std::string> tokens = CSVHelper::Split(line);
			int row = std::stoi(tokens[1]);
			int col = std::stoi(tokens[2]);
			Vector3 worldPos = mOwner->GetGame()->GetHeightMap()->CellToWorld(row, col);
			mPath.push_back(worldPos);
		}

		if (!mPath.empty())
		{
			mOwner->SetPosition(mPath[0]);
		}
	}
}

void EnemyMove::Update(float deltaTime)
{
	if (mPath.empty())
	{
		return;
	}

	Vector3 position = mOwner->GetPosition();
	Vector3 targetPos = mPath[mNextTargetIndex];

	Vector3 toTarget = targetPos - position;
	float distance = toTarget.Length();

	if (distance < DISTANCE_BEFORE_NEXT_NODE)
	{
		mNextTargetIndex = (mNextTargetIndex + 1) % mPath.size();
		targetPos = mPath[mNextTargetIndex];
		toTarget = targetPos - position;
	}

	toTarget.Normalize();

	Vector3 forward = mOwner->GetForward();

	float dot = Vector3::Dot(forward, toTarget);
	SetAccelerate(dot > 0.3f);

	Vector3 cross = Vector3::Cross(forward, toTarget);

	if (cross.z > 0)
	{
		SetCarState(TurnState::TurnRight);
	}
	else if (cross.z < 0)
	{
		SetCarState(TurnState::TurnLeft);
	}
	else
	{
		SetCarState(TurnState::NotTurn);
	}

	if (GetVelocity().Length() < VELOCITY_LENGTH_SETPOINT)
	{
		SetVelocity(GetVelocity() += forward * 100.0f * deltaTime);
	}

	VehicleMove::Update(deltaTime);
}