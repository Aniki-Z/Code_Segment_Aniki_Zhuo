using System;
using System.Collections.Generic;
using Aniki.GameplaySystems.Core;
using UnityEngine;

namespace Aniki.GameplaySystems.Inventory
{
    [CreateAssetMenu(
        fileName = "WeightedLootTable",
        menuName = "Portfolio/Gameplay/Weighted Loot Table")]
    public sealed class WeightedLootTable : ScriptableObject
    {
        [SerializeField] private List<LootEntry> entries = new List<LootEntry>();

        public IReadOnlyList<LootEntry> Entries => entries;

        public bool TryPick(System.Random random, out ItemDefinition item)
        {
            if (random == null)
            {
                throw new ArgumentNullException(nameof(random));
            }

            int index = WeightedSelection.SelectIndex(entries, GetWeight, random.NextDouble());
            item = index >= 0 ? entries[index].Item : null;
            return item != null;
        }

        private static float GetWeight(LootEntry entry)
        {
            return entry != null && entry.Item != null ? entry.Weight : 0f;
        }
    }

    [Serializable]
    public sealed class LootEntry
    {
        [SerializeField] private ItemDefinition item;
        [SerializeField, Min(0f)] private float weight = 1f;

        public ItemDefinition Item => item;
        public float Weight => weight;
    }
}
