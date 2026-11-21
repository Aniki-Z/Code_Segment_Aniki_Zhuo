using System.Collections.Generic;
using UnityEngine;

namespace Aniki.GameplaySystems.CameraSystem
{
    [DisallowMultipleComponent]
    public sealed class SplitScreenCameraManager : MonoBehaviour
    {
        [SerializeField] private TwoPlayerSplit twoPlayerSplit = TwoPlayerSplit.Horizontal;
        [SerializeField] private List<Camera> cameras = new List<Camera>();

        public IReadOnlyList<Camera> Cameras => cameras;

        private void OnEnable()
        {
            ApplyLayout();
        }

        private void OnValidate()
        {
            if (!Application.isPlaying)
            {
                ApplyLayout();
            }
        }

        public bool RegisterCamera(Camera playerCamera)
        {
            if (playerCamera == null || cameras.Contains(playerCamera) || cameras.Count >= 4)
            {
                return false;
            }

            cameras.Add(playerCamera);
            ApplyLayout();
            return true;
        }

        public bool UnregisterCamera(Camera playerCamera)
        {
            bool removed = cameras.Remove(playerCamera);
            if (removed)
            {
                ApplyLayout();
            }

            return removed;
        }

        [ContextMenu("Apply Layout")]
        public void ApplyLayout()
        {
            cameras.RemoveAll(playerCamera => playerCamera == null);

            int activeCount = Mathf.Min(cameras.Count, 4);
            if (activeCount == 0)
            {
                return;
            }

            Rect[] viewports = SplitScreenLayout.Calculate(activeCount, twoPlayerSplit);
            for (int index = 0; index < cameras.Count; index++)
            {
                Camera playerCamera = cameras[index];
                bool shouldEnable = index < activeCount;
                playerCamera.enabled = shouldEnable;

                if (shouldEnable)
                {
                    playerCamera.rect = viewports[index];
                }
            }
        }
    }
}
