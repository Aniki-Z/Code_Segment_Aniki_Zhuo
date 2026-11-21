using Aniki.GameplaySystems.CameraSystem;
using NUnit.Framework;
using UnityEngine;

namespace Aniki.GameplaySystems.Tests
{
    public sealed class SplitScreenLayoutTests
    {
        [Test]
        public void OnePlayerUsesFullViewport()
        {
            Rect[] result = SplitScreenLayout.Calculate(1, TwoPlayerSplit.Horizontal);

            Assert.AreEqual(new Rect(0f, 0f, 1f, 1f), result[0]);
        }

        [Test]
        public void FourPlayersUseQuadrants()
        {
            Rect[] result = SplitScreenLayout.Calculate(4, TwoPlayerSplit.Horizontal);

            Assert.AreEqual(4, result.Length);
            Assert.AreEqual(new Rect(0f, 0.5f, 0.5f, 0.5f), result[0]);
            Assert.AreEqual(new Rect(0.5f, 0f, 0.5f, 0.5f), result[3]);
        }

        [Test]
        public void TwoPlayerModeChangesOrientation()
        {
            Rect[] horizontal = SplitScreenLayout.Calculate(2, TwoPlayerSplit.Horizontal);
            Rect[] vertical = SplitScreenLayout.Calculate(2, TwoPlayerSplit.Vertical);

            Assert.AreEqual(1f, horizontal[0].width);
            Assert.AreEqual(0.5f, horizontal[0].height);
            Assert.AreEqual(0.5f, vertical[0].width);
            Assert.AreEqual(1f, vertical[0].height);
        }
    }
}
