using Aniki.GameplaySystems.Interaction;
using UnityEngine;

namespace Aniki.GameplaySystems.Inventory
{
    [DisallowMultipleComponent]
    public sealed class Pickup : InteractableBehaviour
    {
        [SerializeField] private ItemDefinition item;
        [SerializeField, Min(1)] private int quantity = 1;

        public override string Prompt
        {
            get
            {
                if (item == null)
                {
                    return string.Empty;
                }

                return quantity > 1
                    ? $"Pick up {item.DisplayName} ×{quantity}"
                    : $"Pick up {item.DisplayName}";
            }
        }

        public override bool CanInteract(GameObject actor)
        {
            if (!base.CanInteract(actor) || item == null || quantity <= 0)
            {
                return false;
            }

            Inventory inventory = FindInventory(actor);
            return inventory != null && inventory.CanAdd(item, 1);
        }

        public override void Interact(GameObject actor)
        {
            Inventory inventory = FindInventory(actor);
            if (inventory == null)
            {
                return;
            }

            int accepted = inventory.TryAdd(item, quantity);
            quantity -= accepted;

            if (quantity <= 0)
            {
                Destroy(gameObject);
            }
        }

        private static Inventory FindInventory(GameObject actor)
        {
            Inventory inventory = actor.GetComponent<Inventory>();
            return inventory != null ? inventory : actor.GetComponentInParent<Inventory>();
        }
    }
}
