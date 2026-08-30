using System;
using UnityEngine;

namespace Aniki.GameplaySystems.World
{
    public sealed class GridGenerationResult
    {
        public GridGenerationResult(GridMap map, Vector2Int spawnCell)
        {
            Map = map ?? throw new ArgumentNullException(nameof(map));
            SpawnCell = spawnCell;
        }

        public GridMap Map { get; }
        public Vector2Int SpawnCell { get; }
    }
}
