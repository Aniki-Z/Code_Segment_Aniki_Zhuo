using System;
using UnityEngine;

namespace Aniki.GameplaySystems.Inventory
{
    [Serializable]
    public sealed class InventorySlot
    {
        [SerializeField] private ItemDefinition item;
        [SerializeField, Min(0)] private int quantity;

        public ItemDefinition Item => item;
        public int Quantity => quantity;
        public bool IsEmpty => item == null || quantity <= 0;
        public int FreeSpace => IsEmpty ? 0 : Mathf.Max(0, item.MaximumStack - quantity);

        internal void Set(ItemDefinition definition, int amount)
        {
            item = definition;
            quantity = Mathf.Max(0, amount);

            if (quantity == 0)
            {
                item = null;
            }
        }

        internal int Add(int amount)
        {
            if (IsEmpty || amount <= 0)
            {
                return 0;
            }

            int accepted = Mathf.Min(amount, FreeSpace);
            quantity += accepted;
            return accepted;
        }

        internal int Remove(int amount)
        {
            if (IsEmpty || amount <= 0)
            {
                return 0;
            }

            int removed = Mathf.Min(amount, quantity);
            quantity -= removed;

            if (quantity == 0)
            {
                item = null;
            }

            return removed;
        }
    }
}
