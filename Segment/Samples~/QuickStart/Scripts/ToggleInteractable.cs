using Aniki.GameplaySystems.Interaction;
using UnityEngine;

namespace Aniki.GameplaySystems.Samples
{
    public sealed class ToggleInteractable : InteractableBehaviour
    {
        [SerializeField] private GameObject target;
        [SerializeField] private string enablePrompt = "Enable object";
        [SerializeField] private string disablePrompt = "Disable object";

        public override string Prompt
        {
            get
            {
                return target != null && target.activeSelf
                    ? disablePrompt
                    : enablePrompt;
            }
        }

        public override bool CanInteract(GameObject actor)
        {
            return base.CanInteract(actor) && target != null && target != gameObject;
        }

        public override void Interact(GameObject actor)
        {
            target.SetActive(!target.activeSelf);
        }
    }
}
