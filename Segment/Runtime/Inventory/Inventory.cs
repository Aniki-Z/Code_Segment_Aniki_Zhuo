using System;
using System.Collections.Generic;
using UnityEngine;

namespace Aniki.GameplaySystems.Inventory
{
    [DisallowMultipleComponent]
    public sealed class Inventory : MonoBehaviour
    {
        [SerializeField, Min(1)] private int capacity = 8;
        [SerializeField] private List<InventorySlot> slots = new List<InventorySlot>();

        public IReadOnlyList<InventorySlot> Slots => slots;

        public event Action Changed;

        private void Awake()
        {
            NormalizeSlots();
        }

        private void OnValidate()
        {
            capacity = Mathf.Max(1, capacity);
            NormalizeSlots();
        }

        public int TryAdd(ItemDefinition item, int amount)
        {
            if (item == null || amount <= 0)
            {
                return 0;
            }

            NormalizeSlots();
            int remaining = amount;

            for (int index = 0; index < slots.Count && remaining > 0; index++)
            {
                InventorySlot slot = slots[index];
                if (!slot.IsEmpty && slot.Item == item)
                {
                    remaining -= slot.Add(remaining);
                }
            }

            for (int index = 0; index < slots.Count && remaining > 0; index++)
            {
                InventorySlot slot = slots[index];
                if (!slot.IsEmpty)
                {
                    continue;
                }

                int accepted = Mathf.Min(remaining, item.MaximumStack);
                slot.Set(item, accepted);
                remaining -= accepted;
            }

            int added = amount - remaining;
            if (added > 0)
            {
                Changed?.Invoke();
            }

            return added;
        }

        public int TryRemove(ItemDefinition item, int amount)
        {
            if (item == null || amount <= 0)
            {
                return 0;
            }

            int remaining = amount;
            for (int index = slots.Count - 1; index >= 0 && remaining > 0; index--)
            {
                InventorySlot slot = slots[index];
                if (!slot.IsEmpty && slot.Item == item)
                {
                    remaining -= slot.Remove(remaining);
                }
            }

            int removed = amount - remaining;
            if (removed > 0)
            {
                Changed?.Invoke();
            }

            return removed;
        }

        public int GetQuantity(ItemDefinition item)
        {
            if (item == null)
            {
                return 0;
            }

            int total = 0;
            for (int index = 0; index < slots.Count; index++)
            {
                InventorySlot slot = slots[index];
                if (!slot.IsEmpty && slot.Item == item)
                {
                    total += slot.Quantity;
                }
            }

            return total;
        }

        public bool CanAdd(ItemDefinition item, int amount)
        {
            if (item == null || amount <= 0)
            {
                return false;
            }

            int available = 0;
            for (int index = 0; index < slots.Count; index++)
            {
                InventorySlot slot = slots[index];
                if (slot.IsEmpty)
                {
                    available += item.MaximumStack;
                }
                else if (slot.Item == item)
                {
                    available += slot.FreeSpace;
                }

                if (available >= amount)
                {
                    return true;
                }
            }

            return false;
        }

        private void NormalizeSlots()
        {
            if (slots == null)
            {
                slots = new List<InventorySlot>();
            }

            while (slots.Count < capacity)
            {
                slots.Add(new InventorySlot());
            }

            if (slots.Count > capacity)
            {
                slots.RemoveRange(capacity, slots.Count - capacity);
            }
        }
    }
}
