using System;
using System.Collections.Generic;

namespace Aniki.GameplaySystems.Core
{
    public static class WeightedSelection
    {
        public static int SelectIndex<T>(
            IReadOnlyList<T> items,
            Func<T, float> getWeight,
            double unitSample)
        {
            if (items == null)
            {
                throw new ArgumentNullException(nameof(items));
            }

            if (getWeight == null)
            {
                throw new ArgumentNullException(nameof(getWeight));
            }

            if (unitSample < 0d || unitSample >= 1d)
            {
                throw new ArgumentOutOfRangeException(nameof(unitSample));
            }

            double totalWeight = 0d;
            for (int index = 0; index < items.Count; index++)
            {
                float weight = getWeight(items[index]);
                if (IsValidWeight(weight))
                {
                    totalWeight += weight;
                }
            }

            if (totalWeight <= 0d)
            {
                return -1;
            }

            double target = unitSample * totalWeight;
            double accumulated = 0d;
            int lastValidIndex = -1;

            for (int index = 0; index < items.Count; index++)
            {
                float weight = getWeight(items[index]);
                if (!IsValidWeight(weight))
                {
                    continue;
                }

                lastValidIndex = index;
                accumulated += weight;
                if (target < accumulated)
                {
                    return index;
                }
            }

            return lastValidIndex;
        }

        private static bool IsValidWeight(float weight)
        {
            return weight > 0f && !float.IsNaN(weight) && !float.IsInfinity(weight);
        }
    }
}
