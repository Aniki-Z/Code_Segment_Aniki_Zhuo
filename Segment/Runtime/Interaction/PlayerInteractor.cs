using System;
using UnityEngine;

namespace Aniki.GameplaySystems.Interaction
{
    [DisallowMultipleComponent]
    [RequireComponent(typeof(InteractionScanner))]
    public sealed class PlayerInteractor : MonoBehaviour
    {
        [SerializeField] private InteractionScanner scanner;
        [SerializeField] private GameObject actor;

        private InteractableBehaviour current;

        public IInteractable Current => current;
        public string CurrentPrompt => current != null ? current.Prompt : string.Empty;

        public event Action<IInteractable> FocusChanged;

        private void Awake()
        {
            if (scanner == null)
            {
                scanner = GetComponent<InteractionScanner>();
            }

            if (actor == null)
            {
                actor = gameObject;
            }
        }

        private void Update()
        {
            RefreshFocus();
        }

        private void OnDisable()
        {
            SetCurrent(null);
        }

        public bool RequestInteraction()
        {
            if (current == null || !current.CanInteract(actor))
            {
                RefreshFocus();
            }

            if (current == null)
            {
                return false;
            }

            current.Interact(actor);
            RefreshFocus();
            return true;
        }

        public void RefreshFocus()
        {
            scanner.TryGetBest(actor, out InteractableBehaviour candidate);
            SetCurrent(candidate);
        }

        private void SetCurrent(InteractableBehaviour candidate)
        {
            if (current == candidate)
            {
                return;
            }

            current = candidate;
            FocusChanged?.Invoke(current);
        }
    }
}
