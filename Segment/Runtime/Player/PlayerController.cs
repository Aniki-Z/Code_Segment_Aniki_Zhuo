using Aniki.GameplaySystems.Core.StateMachine;
using Aniki.GameplaySystems.Player.States;
using UnityEngine;

namespace Aniki.GameplaySystems.Player
{
    [DisallowMultipleComponent]
    [RequireComponent(typeof(CharacterController))]
    public sealed class PlayerController : MonoBehaviour
    {
        [SerializeField] private PlayerMovementSettings settings;
        [SerializeField] private Transform movementReference;

        private CharacterController characterController;
        private StateMachine stateMachine;
        private PlayerIdleState idleState;
        private PlayerMoveState moveState;
        private PlayerDashState dashState;
        private PlayerStunnedState stunnedState;
        private PlayerInputSnapshot input;
        private Vector3 horizontalVelocity;
        private float verticalVelocity;
        private float dashCooldownRemaining;
        private float stunRemaining;

        public PlayerInputSnapshot Input => input;
        public string CurrentStateName => stateMachine?.Current?.GetType().Name ?? "Uninitialized";

        internal PlayerMovementSettings Settings => settings;
        internal bool HasMoveInput => input.Move.sqrMagnitude > settings.InputDeadZone * settings.InputDeadZone;
        internal bool DashRequested => input.DashPressed;
        internal bool DashReady => dashCooldownRemaining <= 0f;

        private void Awake()
        {
            characterController = GetComponent<CharacterController>();

            if (settings == null)
            {
                Debug.LogError("PlayerController requires PlayerMovementSettings.", this);
                enabled = false;
                return;
            }

            stateMachine = new StateMachine();
            idleState = new PlayerIdleState(this);
            moveState = new PlayerMoveState(this);
            dashState = new PlayerDashState(this);
            stunnedState = new PlayerStunnedState(this);
            stateMachine.Start(idleState);
        }

        private void Update()
        {
            float deltaTime = Time.deltaTime;
            dashCooldownRemaining = Mathf.Max(0f, dashCooldownRemaining - deltaTime);
            stateMachine.Tick(deltaTime);
            input = input.WithoutTransientButtons();
        }

        public void SubmitInput(PlayerInputSnapshot snapshot)
        {
            input = snapshot;
        }

        public void Stun(float duration)
        {
            if (stateMachine == null || duration <= 0f)
            {
                return;
            }

            stunRemaining = Mathf.Max(stunRemaining, duration);
            stateMachine.ChangeState(stunnedState);
        }

        internal bool TryEnterDash()
        {
            if (!DashRequested || !DashReady)
            {
                return false;
            }

            stateMachine.ChangeState(dashState);
            return true;
        }

        internal void ChangeToIdle()
        {
            stateMachine.ChangeState(idleState);
        }

        internal void ChangeToMove()
        {
            stateMachine.ChangeState(moveState);
        }

        internal void ChangeToBestLocomotionState()
        {
            stateMachine.ChangeState(HasMoveInput ? moveState : idleState);
        }

        internal Vector3 GetMoveDirection()
        {
            Transform reference = movementReference != null ? movementReference : transform;
            Vector3 forward = Vector3.ProjectOnPlane(reference.forward, Vector3.up).normalized;
            Vector3 right = Vector3.ProjectOnPlane(reference.right, Vector3.up).normalized;
            Vector3 direction = right * input.Move.x + forward * input.Move.y;
            return Vector3.ClampMagnitude(direction, 1f);
        }

        internal void MoveWithAcceleration(Vector3 direction, float speed, float deltaTime)
        {
            Vector3 targetVelocity = direction * speed;
            horizontalVelocity = Vector3.MoveTowards(
                horizontalVelocity,
                targetVelocity,
                settings.Acceleration * deltaTime);

            ApplyMovement(horizontalVelocity, deltaTime);
            RotateToward(direction, deltaTime);
        }

        internal void MoveImmediately(Vector3 direction, float speed, float deltaTime)
        {
            horizontalVelocity = direction * speed;
            ApplyMovement(horizontalVelocity, deltaTime);
            RotateToward(direction, deltaTime);
        }

        internal void StopWithAcceleration(float deltaTime)
        {
            horizontalVelocity = Vector3.MoveTowards(
                horizontalVelocity,
                Vector3.zero,
                settings.Acceleration * deltaTime);

            ApplyMovement(horizontalVelocity, deltaTime);
        }

        internal void BeginDash(out Vector3 direction, out float duration)
        {
            direction = GetMoveDirection();
            if (direction.sqrMagnitude < 0.001f)
            {
                direction = transform.forward;
            }

            direction.Normalize();
            duration = settings.DashDuration;
            dashCooldownRemaining = settings.DashCooldown;
        }

        internal bool AdvanceStun(float deltaTime)
        {
            stunRemaining = Mathf.Max(0f, stunRemaining - deltaTime);
            return stunRemaining <= 0f;
        }

        private void ApplyMovement(Vector3 horizontal, float deltaTime)
        {
            if (characterController.isGrounded && verticalVelocity < 0f)
            {
                verticalVelocity = settings.GroundedVelocity;
            }
            else
            {
                verticalVelocity += settings.Gravity * deltaTime;
            }

            Vector3 velocity = horizontal + Vector3.up * verticalVelocity;
            characterController.Move(velocity * deltaTime);
        }

        private void RotateToward(Vector3 direction, float deltaTime)
        {
            if (direction.sqrMagnitude < 0.001f)
            {
                return;
            }

            float radians = settings.RotationSpeed * Mathf.Deg2Rad * deltaTime;
            Vector3 forward = Vector3.RotateTowards(transform.forward, direction, radians, 0f);
            transform.rotation = Quaternion.LookRotation(forward, Vector3.up);
        }
    }
}
