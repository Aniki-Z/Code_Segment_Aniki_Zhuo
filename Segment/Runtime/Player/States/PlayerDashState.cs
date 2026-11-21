using UnityEngine;

namespace Aniki.GameplaySystems.Player.States
{
    internal sealed class PlayerDashState : PlayerState
    {
        private Vector3 direction;
        private float remainingTime;

        public PlayerDashState(PlayerController controller)
            : base(controller)
        {
        }

        public override void Enter()
        {
            Controller.BeginDash(out direction, out remainingTime);
        }

        public override void Tick(float deltaTime)
        {
            Controller.MoveImmediately(
                direction,
                Controller.Settings.DashSpeed,
                deltaTime);

            remainingTime -= deltaTime;
            if (remainingTime <= 0f)
            {
                Controller.ChangeToBestLocomotionState();
            }
        }
    }
}
