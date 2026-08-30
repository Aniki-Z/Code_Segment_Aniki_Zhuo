using Aniki.GameplaySystems.Core.StateMachine;

namespace Aniki.GameplaySystems.Player.States
{
    internal abstract class PlayerState : IState
    {
        protected PlayerState(PlayerController controller)
        {
            Controller = controller;
        }

        protected PlayerController Controller { get; }

        public virtual void Enter()
        {
        }

        public abstract void Tick(float deltaTime);

        public virtual void Exit()
        {
        }
    }
}
