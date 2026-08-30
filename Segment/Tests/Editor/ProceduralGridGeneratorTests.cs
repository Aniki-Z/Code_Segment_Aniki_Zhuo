using System.Collections.Generic;
using Aniki.GameplaySystems.World;
using NUnit.Framework;
using UnityEngine;

namespace Aniki.GameplaySystems.Tests
{
    public sealed class ProceduralGridGeneratorTests
    {
        [Test]
        public void SameSeedProducesSameMapAndSpawn()
        {
            GridGenerationConfig config = CreateConfig(417);

            GridGenerationResult first = ProceduralGridGenerator.Generate(config);
            GridGenerationResult second = ProceduralGridGenerator.Generate(config);

            Assert.IsTrue(first.Map.ContentEquals(second.Map));
            Assert.AreEqual(first.SpawnCell, second.SpawnCell);
        }

        [Test]
        public void SpawnCellIsGround()
        {
            GridGenerationResult result = ProceduralGridGenerator.Generate(CreateConfig(99));

            Assert.AreEqual(
                GridCellType.Ground,
                result.Map[result.SpawnCell.x, result.SpawnCell.y]);
        }

        [Test]
        public void LargestRegionFilteringLeavesConnectedGround()
        {
            GridGenerationResult result = ProceduralGridGenerator.Generate(CreateConfig(1201));
            GridMap map = result.Map;
            int totalGround = map.CountCells(GridCellType.Ground);
            int reachableGround = CountReachableGround(map, result.SpawnCell);

            Assert.AreEqual(totalGround, reachableGround);
        }

        private static GridGenerationConfig CreateConfig(int seed)
        {
            return new GridGenerationConfig(
                32,
                24,
                seed,
                0.085f,
                0.35f,
                0.74f,
                2,
                true,
                true);
        }

        private static int CountReachableGround(GridMap map, Vector2Int start)
        {
            Queue<Vector2Int> queue = new Queue<Vector2Int>();
            HashSet<Vector2Int> visited = new HashSet<Vector2Int>();
            queue.Enqueue(start);
            visited.Add(start);

            Vector2Int[] directions =
            {
                Vector2Int.right,
                Vector2Int.left,
                Vector2Int.up,
                Vector2Int.down
            };

            while (queue.Count > 0)
            {
                Vector2Int current = queue.Dequeue();
                for (int index = 0; index < directions.Length; index++)
                {
                    Vector2Int next = current + directions[index];
                    if (!map.IsInside(next.x, next.y) ||
                        map[next.x, next.y] != GridCellType.Ground ||
                        visited.Contains(next))
                    {
                        continue;
                    }

                    visited.Add(next);
                    queue.Enqueue(next);
                }
            }

            return visited.Count;
        }
    }
}
