using UnityEngine;

namespace Aniki.GameplaySystems.Interaction
{
    public abstract class InteractableBehaviour : MonoBehaviour, IInteractable
    {
        public virtual Transform InteractionTransform => transform;
        public abstract string Prompt { get; }

        public virtual bool CanInteract(GameObject actor)
        {
            return isActiveAndEnabled && actor != null;
        }

        public abstract void Interact(GameObject actor);
    }
}
