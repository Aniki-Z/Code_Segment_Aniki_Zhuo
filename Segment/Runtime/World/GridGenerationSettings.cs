using UnityEngine;

namespace Aniki.GameplaySystems.World
{
    [CreateAssetMenu(
        fileName = "GridGenerationSettings",
        menuName = "Portfolio/Gameplay/Grid Generation Settings")]
    public sealed class GridGenerationSettings : ScriptableObject
    {
        [SerializeField, Min(2)] private int width = 30;
        [SerializeField, Min(2)] private int height = 30;
        [SerializeField] private int seed = 12345;
        [SerializeField, Min(0.001f)] private float noiseScale = 0.09f;
        [SerializeField, Range(0f, 1f)] private float waterThreshold = 0.35f;
        [SerializeField, Range(0f, 1f)] private float hazardThreshold = 0.73f;
        [SerializeField, Range(0, 8)] private int smoothingPasses = 2;
        [SerializeField] private bool sealBorder = true;
        [SerializeField] private bool keepLargestGroundRegion = true;

        public GridGenerationConfig CreateConfig()
        {
            return new GridGenerationConfig(
                width,
                height,
                seed,
                noiseScale,
                waterThreshold,
                hazardThreshold,
                smoothingPasses,
                sealBorder,
                keepLargestGroundRegion);
        }

        [ContextMenu("Randomize Seed")]
        private void RandomizeSeed()
        {
            seed = System.Environment.TickCount;
        }

        private void OnValidate()
        {
            width = Mathf.Max(2, width);
            height = Mathf.Max(2, height);
            noiseScale = Mathf.Max(0.001f, noiseScale);
            waterThreshold = Mathf.Clamp(waterThreshold, 0f, 0.99f);
            hazardThreshold = Mathf.Clamp(hazardThreshold, waterThreshold + 0.01f, 1f);
        }
    }
}
