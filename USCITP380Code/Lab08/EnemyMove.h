#pragma once
#include "VehicleMove.h"
#include "Math.h"

class EnemyMove : public VehicleMove
{
private:
	std::vector<Vector3> mPath;
	int mNextTargetIndex;

public:
	EnemyMove(class Actor* owner);
	void Update(float deltaTime) override;

	const float DISTANCE_BEFORE_NEXT_NODE = 100.0f;
	const float VELOCITY_LENGTH_SETPOINT = 50.0f;
};