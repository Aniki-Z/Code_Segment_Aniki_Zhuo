using UnityEngine;

namespace Aniki.GameplaySystems.Player
{
    [CreateAssetMenu(
        fileName = "PlayerMovementSettings",
        menuName = "Portfolio/Gameplay/Player Movement Settings")]
    public sealed class PlayerMovementSettings : ScriptableObject
    {
        [SerializeField, Min(0f)] private float moveSpeed = 6f;
        [SerializeField, Min(0f)] private float acceleration = 30f;
        [SerializeField, Min(0f)] private float rotationSpeed = 720f;
        [SerializeField] private float gravity = -25f;
        [SerializeField] private float groundedVelocity = -2f;
        [SerializeField, Min(0f)] private float dashSpeed = 14f;
        [SerializeField, Min(0.01f)] private float dashDuration = 0.18f;
        [SerializeField, Min(0f)] private float dashCooldown = 0.45f;
        [SerializeField, Range(0f, 1f)] private float inputDeadZone = 0.1f;

        public float MoveSpeed => moveSpeed;
        public float Acceleration => acceleration;
        public float RotationSpeed => rotationSpeed;
        public float Gravity => gravity;
        public float GroundedVelocity => groundedVelocity;
        public float DashSpeed => dashSpeed;
        public float DashDuration => dashDuration;
        public float DashCooldown => dashCooldown;
        public float InputDeadZone => inputDeadZone;

        private void OnValidate()
        {
            dashDuration = Mathf.Max(0.01f, dashDuration);
            inputDeadZone = Mathf.Clamp01(inputDeadZone);
        }
    }
}
