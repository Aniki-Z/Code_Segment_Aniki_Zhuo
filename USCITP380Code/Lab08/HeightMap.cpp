#include "HeightMap.h"
#include "CSVHelper.h"
#include <iostream>
#include <fstream>

HeightMap::HeightMap(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file)
	{
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::vector<std::string> tokens = CSVHelper::Split(line);
		std::vector<int> rowValues;

		for (const std::string& token : tokens)
		{
			rowValues.push_back(std::stoi(token));
		}

		mGrid.push_back(rowValues);
	}
}

bool HeightMap::IsCellOnTrack(int row, int col) const
{
	if (row < 0 || col < 0 || row > mGrid.size() - 1 || col > mGrid[0].size() - 1)
	{
		return false;
	}

	return mGrid[row][col] != -1;
}

float HeightMap::GetHeightFromCell(int row, int col) const
{
	return -40.0f + (mGrid[row][col] * 5.0f);
}

Vector3 HeightMap::CellToWorld(int row, int col) const
{
	if (!IsCellOnTrack(row, col))
	{
		return Vector3::Zero;
	}

	float x = GRID_TOP - CELL_SIZE * row;
	float y = GRID_LEFT + CELL_SIZE * col;
	float z = GetHeightFromCell(row, col);

	return Vector3(x, y, z);
}

Vector2 HeightMap::WorldToCell(const Vector2& pos) const
{
	int row = static_cast<int>((std::abs(pos.x - GRID_TOP) + CELL_SIZE / 2.0f) / CELL_SIZE);
	int col = static_cast<int>((std::abs(pos.y - GRID_LEFT) + CELL_SIZE / 2.0f) / CELL_SIZE);

	return Vector2(static_cast<float>(row), static_cast<float>(col));
}

bool HeightMap::IsOnTrack(const Vector2& pos) const
{
	Vector2 cell = WorldToCell(pos);

	return IsCellOnTrack(static_cast<int>(cell.x), static_cast<int>(cell.y));
}

float HeightMap::GetHeight(const Vector2& pos) const
{
	if (!IsOnTrack(pos))
	{
		return -1000.0f;
	}

	Vector2 cell = WorldToCell(pos);
	return GetHeightFromCell(static_cast<int>(cell.x), static_cast<int>(cell.y));
}
