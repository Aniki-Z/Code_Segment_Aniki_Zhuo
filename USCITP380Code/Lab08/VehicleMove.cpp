#include "VehicleMove.h"
#include "CSVHelper.h"
#include <fstream>
#include "Game.h"

VehicleMove::VehicleMove(Actor* owner)
: Component(owner, 50)
, mIsAccelerate(false)
, mTurnState(TurnState::NotTurn)
, mVelocity(0.0f, 0.0f, 0.0f)
, mAngularVelocity(0.0f)
, mAccelerateTimer(0.0f)
, mCurrentLap(0)
, mLastCheckpointIndex(-1)
, mCheckpoint(0)
{
	std::ifstream file("Assets/HeightMap/Checkpoints.csv");
	if (file)
	{
		std::string line;
		std::getline(file, line);
		while (std::getline(file, line))
		{
			std::vector<std::string> tokens = CSVHelper::Split(line);

			int minX = std::stoi(tokens[1]);
			int maxX = std::stoi(tokens[2]);
			int minY = std::stoi(tokens[3]);
			int maxY = std::stoi(tokens[4]);

			std::vector<int> element;
			element.push_back(minX);
			element.push_back(maxX);
			element.push_back(minY);
			element.push_back(maxY);

			mCheckpoints.push_back(element);
		}
	}
}
void VehicleMove::Update(float deltaTime)
{
	ApplyAcceleration(deltaTime);
	UpdatePosition(deltaTime);
	ApplyDrag();
	UpdateRotation(deltaTime);
	CheckCheckpointProgression();
}

void VehicleMove::ApplyAcceleration(float deltaTime)
{
	if (mIsAccelerate)
	{
		mAccelerateTimer += deltaTime;
		if (mAccelerateTimer > ACC_RAMP_TIME)
		{
			mAccelerateTimer = ACC_RAMP_TIME;
		}

		float alpha = mAccelerateTimer / ACC_RAMP_TIME;
		float acceleration = MIN_ACC_MAG + alpha * (MAX_ACC_MAG - MIN_ACC_MAG);

		Vector3 forward = mOwner->GetForward();
		mVelocity += forward * acceleration * deltaTime;
	}
	else
	{
		mAccelerateTimer = 0.0f;
	}
}

void VehicleMove::UpdatePosition(float deltaTime)
{
	Vector3 position = mOwner->GetPosition();
	position += mVelocity * deltaTime;

	HeightMap* heightMap = mOwner->GetGame()->GetHeightMap();
	if (heightMap)
	{
		Vector2 pos2D(position.x, position.y);
		if (heightMap->IsOnTrack(pos2D))
		{
			float targetZ = heightMap->GetHeight(pos2D);
			position.z = Math::Lerp(position.z, targetZ, 0.1f);
		}
	}

	mOwner->SetPosition(position);
}

void VehicleMove::ApplyDrag()
{
	if (mIsAccelerate)
	{
		mVelocity *= LINEAR_DRAG;
	}
	else
	{
		mVelocity *= LINEAR_DRAG_WITHOUT_ACC;
	}
}

void VehicleMove::UpdateRotation(float deltaTime)
{
	if (mTurnState == TurnState::TurnLeft)
	{
		mAngularVelocity -= ANGULAR_ACC * deltaTime;
	}
	else if (mTurnState == TurnState::TurnRight)
	{
		mAngularVelocity += ANGULAR_ACC * deltaTime;
	}

	float rotation = mOwner->GetRotation();
	rotation += mAngularVelocity * deltaTime;
	mOwner->SetRotation(rotation);

	mAngularVelocity *= ANGULAR_DRAG;
}

void VehicleMove::CheckCheckpointProgression()
{
	Vector3 position = mOwner->GetPosition();
	Vector2 pos(position.x, position.y);
	Vector2 cellPos = mOwner->GetGame()->GetHeightMap()->WorldToCell(pos);
	int cellX = static_cast<int>(cellPos.x);
	int cellY = static_cast<int>(cellPos.y);

	int nextCheckpointIndex = (mLastCheckpointIndex + 1) % mCheckpoints.size();
	std::vector<int> nextCheckpoint = mCheckpoints[nextCheckpointIndex];
	mCheckpoint = nextCheckpointIndex;

	if (cellX >= nextCheckpoint[0] && cellX <= nextCheckpoint[1] && cellY >= nextCheckpoint[2] &&
		cellY <= nextCheckpoint[3])
	{
		mLastCheckpointIndex = nextCheckpointIndex;

		if (mLastCheckpointIndex == 0)
		{
			mCurrentLap++;
			OnLapChange(mCurrentLap);
		}
	}
}

float VehicleMove::GetDistanceToNextCheckpoint() const
{
	if (mCheckpoint < 0 || mCheckpoint >= static_cast<int>(mCheckpoints.size()))
	{
		return std::numeric_limits<float>::max();
	}

	std::vector<int> checkpoint = mCheckpoints[mCheckpoint];

	Vector2 checkpointPos((checkpoint[0] + checkpoint[1]) / 2.0f,
						  (checkpoint[2] + checkpoint[3]) / 2.0f);

	Vector2 worldPos(mOwner->GetPosition().x, mOwner->GetPosition().y);
	Vector2 cellPos = mOwner->GetGame()->GetHeightMap()->WorldToCell(worldPos);

	return (cellPos - checkpointPos).Length();
}
