using System;
using System.Collections.Generic;
using UnityEngine;

namespace Aniki.GameplaySystems.World
{
    public static class ProceduralGridGenerator
    {
        private static readonly int[] NeighborX = { 1, -1, 0, 0 };
        private static readonly int[] NeighborY = { 0, 0, 1, -1 };

        public static GridGenerationResult Generate(GridGenerationSettings settings)
        {
            if (settings == null)
            {
                throw new ArgumentNullException(nameof(settings));
            }

            return Generate(settings.CreateConfig());
        }

        public static GridGenerationResult Generate(GridGenerationConfig config)
        {
            GridMap map = CreateNoiseMap(config);

            for (int pass = 0; pass < config.SmoothingPasses; pass++)
            {
                map = Smooth(map, config.SealBorder);
            }

            EnsureGroundExists(map);

            List<int> largestGroundRegion = FindLargestGroundRegion(map);
            if (config.KeepLargestGroundRegion)
            {
                KeepOnlyRegion(map, largestGroundRegion);
            }

            Vector2Int spawnCell = FindNearestCellToCenter(map, GridCellType.Ground);
            return new GridGenerationResult(map, spawnCell);
        }

        private static GridMap CreateNoiseMap(GridGenerationConfig config)
        {
            GridMap map = new GridMap(config.Width, config.Height);
            System.Random random = new System.Random(config.Seed);
            float offsetX = random.Next(-100000, 100000);
            float offsetY = random.Next(-100000, 100000);

            for (int y = 0; y < config.Height; y++)
            {
                for (int x = 0; x < config.Width; x++)
                {
                    if (config.SealBorder && IsBorder(x, y, config.Width, config.Height))
                    {
                        map[x, y] = GridCellType.Water;
                        continue;
                    }

                    float sample = Mathf.PerlinNoise(
                        (x + offsetX) * config.NoiseScale,
                        (y + offsetY) * config.NoiseScale);

                    if (sample < config.WaterThreshold)
                    {
                        map[x, y] = GridCellType.Water;
                    }
                    else if (sample > config.HazardThreshold)
                    {
                        map[x, y] = GridCellType.Hazard;
                    }
                    else
                    {
                        map[x, y] = GridCellType.Ground;
                    }
                }
            }

            return map;
        }

        private static GridMap Smooth(GridMap source, bool sealBorder)
        {
            GridMap result = source.Copy();

            for (int y = 0; y < source.Height; y++)
            {
                for (int x = 0; x < source.Width; x++)
                {
                    if (sealBorder && IsBorder(x, y, source.Width, source.Height))
                    {
                        result[x, y] = GridCellType.Water;
                        continue;
                    }

                    int ground = 0;
                    int water = 0;
                    int hazard = 0;

                    for (int offsetY = -1; offsetY <= 1; offsetY++)
                    {
                        for (int offsetX = -1; offsetX <= 1; offsetX++)
                        {
                            if (offsetX == 0 && offsetY == 0)
                            {
                                continue;
                            }

                            int neighborX = x + offsetX;
                            int neighborY = y + offsetY;
                            GridCellType neighbor = source.IsInside(neighborX, neighborY)
                                ? source[neighborX, neighborY]
                                : GridCellType.Water;

                            switch (neighbor)
                            {
                                case GridCellType.Ground:
                                    ground++;
                                    break;
                                case GridCellType.Hazard:
                                    hazard++;
                                    break;
                                default:
                                    water++;
                                    break;
                            }
                        }
                    }

                    GridCellType current = source[x, y];
                    int currentCount = current == GridCellType.Ground
                        ? ground
                        : current == GridCellType.Hazard
                            ? hazard
                            : water;

                    GridCellType majority = current;
                    int majorityCount = currentCount;

                    if (ground > majorityCount)
                    {
                        majority = GridCellType.Ground;
                        majorityCount = ground;
                    }

                    if (water > majorityCount)
                    {
                        majority = GridCellType.Water;
                        majorityCount = water;
                    }

                    if (hazard > majorityCount)
                    {
                        majority = GridCellType.Hazard;
                        majorityCount = hazard;
                    }

                    if (majorityCount >= 5)
                    {
                        result[x, y] = majority;
                    }
                }
            }

            return result;
        }

        private static void EnsureGroundExists(GridMap map)
        {
            if (map.CountCells(GridCellType.Ground) > 0)
            {
                return;
            }

            map[map.Width / 2, map.Height / 2] = GridCellType.Ground;
        }

        private static List<int> FindLargestGroundRegion(GridMap map)
        {
            bool[] visited = new bool[map.Count];
            List<int> largest = new List<int>();

            for (int index = 0; index < map.Count; index++)
            {
                if (visited[index] || map[index] != GridCellType.Ground)
                {
                    continue;
                }

                List<int> region = FloodFill(map, index, visited);
                if (region.Count > largest.Count)
                {
                    largest = region;
                }
            }

            return largest;
        }

        private static List<int> FloodFill(GridMap map, int startIndex, bool[] visited)
        {
            Queue<int> frontier = new Queue<int>();
            List<int> region = new List<int>();
            frontier.Enqueue(startIndex);
            visited[startIndex] = true;

            while (frontier.Count > 0)
            {
                int index = frontier.Dequeue();
                region.Add(index);
                map.FromIndex(index, out int x, out int y);

                for (int direction = 0; direction < NeighborX.Length; direction++)
                {
                    int neighborX = x + NeighborX[direction];
                    int neighborY = y + NeighborY[direction];
                    if (!map.IsInside(neighborX, neighborY))
                    {
                        continue;
                    }

                    int neighborIndex = map.ToIndex(neighborX, neighborY);
                    if (visited[neighborIndex] || map[neighborIndex] != GridCellType.Ground)
                    {
                        continue;
                    }

                    visited[neighborIndex] = true;
                    frontier.Enqueue(neighborIndex);
                }
            }

            return region;
        }

        private static void KeepOnlyRegion(GridMap map, List<int> region)
        {
            bool[] keep = new bool[map.Count];
            for (int index = 0; index < region.Count; index++)
            {
                keep[region[index]] = true;
            }

            for (int index = 0; index < map.Count; index++)
            {
                if (map[index] == GridCellType.Ground && !keep[index])
                {
                    map[index] = GridCellType.Water;
                }
            }
        }

        private static Vector2Int FindNearestCellToCenter(GridMap map, GridCellType type)
        {
            Vector2 center = new Vector2((map.Width - 1) * 0.5f, (map.Height - 1) * 0.5f);
            float bestDistance = float.PositiveInfinity;
            Vector2Int best = new Vector2Int(map.Width / 2, map.Height / 2);

            for (int y = 0; y < map.Height; y++)
            {
                for (int x = 0; x < map.Width; x++)
                {
                    if (map[x, y] != type)
                    {
                        continue;
                    }

                    float distance = ((Vector2)new Vector2Int(x, y) - center).sqrMagnitude;
                    if (distance < bestDistance)
                    {
                        bestDistance = distance;
                        best = new Vector2Int(x, y);
                    }
                }
            }

            return best;
        }

        private static bool IsBorder(int x, int y, int width, int height)
        {
            return x == 0 || y == 0 || x == width - 1 || y == height - 1;
        }
    }
}
