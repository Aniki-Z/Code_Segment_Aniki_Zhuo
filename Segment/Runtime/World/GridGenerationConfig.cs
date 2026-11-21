using System;

namespace Aniki.GameplaySystems.World
{
    public readonly struct GridGenerationConfig
    {
        public GridGenerationConfig(
            int width,
            int height,
            int seed,
            float noiseScale,
            float waterThreshold,
            float hazardThreshold,
            int smoothingPasses,
            bool sealBorder,
            bool keepLargestGroundRegion)
        {
            if (width < 2)
            {
                throw new ArgumentOutOfRangeException(nameof(width));
            }

            if (height < 2)
            {
                throw new ArgumentOutOfRangeException(nameof(height));
            }

            if (noiseScale <= 0f)
            {
                throw new ArgumentOutOfRangeException(nameof(noiseScale));
            }

            if (waterThreshold < 0f || waterThreshold > 1f)
            {
                throw new ArgumentOutOfRangeException(nameof(waterThreshold));
            }

            if (hazardThreshold < 0f || hazardThreshold > 1f || hazardThreshold <= waterThreshold)
            {
                throw new ArgumentOutOfRangeException(nameof(hazardThreshold));
            }

            if (smoothingPasses < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(smoothingPasses));
            }

            Width = width;
            Height = height;
            Seed = seed;
            NoiseScale = noiseScale;
            WaterThreshold = waterThreshold;
            HazardThreshold = hazardThreshold;
            SmoothingPasses = smoothingPasses;
            SealBorder = sealBorder;
            KeepLargestGroundRegion = keepLargestGroundRegion;
        }

        public int Width { get; }
        public int Height { get; }
        public int Seed { get; }
        public float NoiseScale { get; }
        public float WaterThreshold { get; }
        public float HazardThreshold { get; }
        public int SmoothingPasses { get; }
        public bool SealBorder { get; }
        public bool KeepLargestGroundRegion { get; }
    }
}
