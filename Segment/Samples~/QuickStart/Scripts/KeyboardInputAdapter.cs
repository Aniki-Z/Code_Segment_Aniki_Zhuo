using Aniki.GameplaySystems.Interaction;
using Aniki.GameplaySystems.Player;
using UnityEngine;

namespace Aniki.GameplaySystems.Samples
{
    [DefaultExecutionOrder(-100)]
    public sealed class KeyboardInputAdapter : MonoBehaviour
    {
        [SerializeField] private PlayerController playerController;
        [SerializeField] private PlayerInteractor playerInteractor;

        private void Update()
        {
            Vector2 move = new Vector2(
                GetAxis(KeyCode.A, KeyCode.D),
                GetAxis(KeyCode.S, KeyCode.W));

            playerController.SubmitInput(new PlayerInputSnapshot(
                move,
                Input.GetKeyDown(KeyCode.Space)));

            if (playerInteractor != null && Input.GetKeyDown(KeyCode.E))
            {
                playerInteractor.RequestInteraction();
            }
        }

        private static float GetAxis(KeyCode negative, KeyCode positive)
        {
            float value = 0f;
            if (Input.GetKey(negative))
            {
                value -= 1f;
            }

            if (Input.GetKey(positive))
            {
                value += 1f;
            }

            return value;
        }
    }
}
