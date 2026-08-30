using System;
using UnityEngine;

namespace Aniki.GameplaySystems.CameraSystem
{
    public static class SplitScreenLayout
    {
        public static Rect[] Calculate(int playerCount, TwoPlayerSplit twoPlayerSplit)
        {
            switch (playerCount)
            {
                case 1:
                    return new[]
                    {
                        new Rect(0f, 0f, 1f, 1f)
                    };

                case 2:
                    return twoPlayerSplit == TwoPlayerSplit.Horizontal
                        ? CreateHorizontalPair()
                        : CreateVerticalPair();

                case 3:
                    return new[]
                    {
                        new Rect(0f, 0.5f, 1f, 0.5f),
                        new Rect(0f, 0f, 0.5f, 0.5f),
                        new Rect(0.5f, 0f, 0.5f, 0.5f)
                    };

                case 4:
                    return new[]
                    {
                        new Rect(0f, 0.5f, 0.5f, 0.5f),
                        new Rect(0.5f, 0.5f, 0.5f, 0.5f),
                        new Rect(0f, 0f, 0.5f, 0.5f),
                        new Rect(0.5f, 0f, 0.5f, 0.5f)
                    };

                default:
                    throw new ArgumentOutOfRangeException(
                        nameof(playerCount),
                        "Split-screen layout supports one to four players.");
            }
        }

        private static Rect[] CreateHorizontalPair()
        {
            return new[]
            {
                new Rect(0f, 0.5f, 1f, 0.5f),
                new Rect(0f, 0f, 1f, 0.5f)
            };
        }

        private static Rect[] CreateVerticalPair()
        {
            return new[]
            {
                new Rect(0f, 0f, 0.5f, 1f),
                new Rect(0.5f, 0f, 0.5f, 1f)
            };
        }
    }
}
