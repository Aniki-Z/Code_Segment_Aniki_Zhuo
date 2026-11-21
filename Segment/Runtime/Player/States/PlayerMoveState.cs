namespace Aniki.GameplaySystems.Player.States
{
    internal sealed class PlayerMoveState : PlayerState
    {
        public PlayerMoveState(PlayerController controller)
            : base(controller)
        {
        }

        public override void Tick(float deltaTime)
        {
            if (Controller.TryEnterDash())
            {
                return;
            }

            if (!Controller.HasMoveInput)
            {
                Controller.ChangeToIdle();
                return;
            }

            Controller.MoveWithAcceleration(
                Controller.GetMoveDirection(),
                Controller.Settings.MoveSpeed,
                deltaTime);
        }
    }
}
