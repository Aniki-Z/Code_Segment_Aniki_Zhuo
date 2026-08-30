using UnityEngine;

namespace Aniki.GameplaySystems.Inventory
{
    [CreateAssetMenu(
        fileName = "ItemDefinition",
        menuName = "Portfolio/Gameplay/Item Definition")]
    public sealed class ItemDefinition : ScriptableObject
    {
        [SerializeField] private string id = "item.id";
        [SerializeField] private string displayName = "Item";
        [SerializeField] private Sprite icon;
        [SerializeField, Min(1)] private int maximumStack = 1;

        public string Id => id;
        public string DisplayName => displayName;
        public Sprite Icon => icon;
        public int MaximumStack => maximumStack;

        private void OnValidate()
        {
            id = id == null ? string.Empty : id.Trim();
            maximumStack = Mathf.Max(1, maximumStack);
        }
    }
}
