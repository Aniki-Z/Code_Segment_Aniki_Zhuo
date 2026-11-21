using UnityEngine;

namespace Aniki.GameplaySystems.World
{
    [ExecuteAlways]
    [DisallowMultipleComponent]
    public sealed class GridPreview : MonoBehaviour
    {
        [SerializeField] private GridGenerationSettings settings;
        [SerializeField, Min(0.05f)] private float cellSize = 1f;
        [SerializeField, Min(0.01f)] private float cellHeight = 0.08f;
        [SerializeField] private bool drawWhenNotSelected;

        private GridGenerationResult cachedResult;

        [ContextMenu("Regenerate Preview")]
        public void Regenerate()
        {
            cachedResult = settings != null
                ? ProceduralGridGenerator.Generate(settings)
                : null;
        }

        private void OnValidate()
        {
            cellSize = Mathf.Max(0.05f, cellSize);
            cellHeight = Mathf.Max(0.01f, cellHeight);
            cachedResult = null;
        }

        private void OnDrawGizmos()
        {
            if (drawWhenNotSelected)
            {
                DrawGrid();
            }
        }

        private void OnDrawGizmosSelected()
        {
            if (!drawWhenNotSelected)
            {
                DrawGrid();
            }
        }

        private void DrawGrid()
        {
            if (settings == null)
            {
                return;
            }

            if (cachedResult == null)
            {
                Regenerate();
            }

            GridMap map = cachedResult.Map;
            Matrix4x4 previousMatrix = Gizmos.matrix;
            Color previousColor = Gizmos.color;
            Gizmos.matrix = transform.localToWorldMatrix;

            Vector3 origin = new Vector3(
                -(map.Width - 1) * cellSize * 0.5f,
                0f,
                -(map.Height - 1) * cellSize * 0.5f);

            for (int y = 0; y < map.Height; y++)
            {
                for (int x = 0; x < map.Width; x++)
                {
                    Gizmos.color = GetColor(map[x, y]);
                    Vector3 position = origin + new Vector3(x * cellSize, 0f, y * cellSize);
                    Gizmos.DrawCube(position, new Vector3(cellSize * 0.92f, cellHeight, cellSize * 0.92f));
                }
            }

            Gizmos.color = Color.yellow;
            Vector3 spawnPosition = origin + new Vector3(
                cachedResult.SpawnCell.x * cellSize,
                cellHeight,
                cachedResult.SpawnCell.y * cellSize);
            Gizmos.DrawWireSphere(spawnPosition, cellSize * 0.3f);

            Gizmos.matrix = previousMatrix;
            Gizmos.color = previousColor;
        }

        private static Color GetColor(GridCellType type)
        {
            switch (type)
            {
                case GridCellType.Water:
                    return new Color(0.15f, 0.45f, 0.85f, 0.85f);
                case GridCellType.Hazard:
                    return new Color(0.85f, 0.3f, 0.2f, 0.85f);
                default:
                    return new Color(0.3f, 0.75f, 0.35f, 0.85f);
            }
        }
    }
}
