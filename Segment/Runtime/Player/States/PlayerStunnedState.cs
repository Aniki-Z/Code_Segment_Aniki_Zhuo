namespace Aniki.GameplaySystems.Player.States
{
    internal sealed class PlayerStunnedState : PlayerState
    {
        public PlayerStunnedState(PlayerController controller)
            : base(controller)
        {
        }

        public override void Tick(float deltaTime)
        {
            Controller.StopWithAcceleration(deltaTime);
            if (Controller.AdvanceStun(deltaTime))
            {
                Controller.ChangeToBestLocomotionState();
            }
        }
    }
}
