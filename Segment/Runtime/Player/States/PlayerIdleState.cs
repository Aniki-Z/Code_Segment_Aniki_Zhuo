namespace Aniki.GameplaySystems.Player.States
{
    internal sealed class PlayerIdleState : PlayerState
    {
        public PlayerIdleState(PlayerController controller)
            : base(controller)
        {
        }

        public override void Tick(float deltaTime)
        {
            if (Controller.TryEnterDash())
            {
                return;
            }

            if (Controller.HasMoveInput)
            {
                Controller.ChangeToMove();
                return;
            }

            Controller.StopWithAcceleration(deltaTime);
        }
    }
}
