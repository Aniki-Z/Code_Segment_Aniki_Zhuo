using Aniki.GameplaySystems.Core;
using NUnit.Framework;

namespace Aniki.GameplaySystems.Tests
{
    public sealed class WeightedSelectionTests
    {
        [TestCase(0.00d, 0)]
        [TestCase(0.24d, 0)]
        [TestCase(0.25d, 1)]
        [TestCase(0.99d, 1)]
        public void SelectIndexUsesRelativeWeights(double sample, int expected)
        {
            float[] weights = { 1f, 3f };

            int result = WeightedSelection.SelectIndex(weights, value => value, sample);

            Assert.AreEqual(expected, result);
        }

        [Test]
        public void NonPositiveAndInvalidWeightsAreIgnored()
        {
            float[] weights = { -1f, 0f, float.NaN, 2f };

            int result = WeightedSelection.SelectIndex(weights, value => value, 0.5d);

            Assert.AreEqual(3, result);
        }

        [Test]
        public void EmptyWeightSetReturnsMinusOne()
        {
            float[] weights = { 0f, -2f };

            int result = WeightedSelection.SelectIndex(weights, value => value, 0.5d);

            Assert.AreEqual(-1, result);
        }
    }
}
