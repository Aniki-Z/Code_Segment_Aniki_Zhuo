using UnityEngine;

namespace Aniki.GameplaySystems.Interaction
{
    public interface IInteractable
    {
        Transform InteractionTransform { get; }
        string Prompt { get; }
        bool CanInteract(GameObject actor);
        void Interact(GameObject actor);
    }
}
