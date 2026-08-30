using System;

namespace Aniki.GameplaySystems.Core.StateMachine
{
    public sealed class StateMachine
    {
        private bool isTransitioning;

        public IState Current { get; private set; }

        public event Action<IState, IState> Changed;

        public void Start(IState initialState)
        {
            if (Current != null)
            {
                throw new InvalidOperationException("The state machine has already started.");
            }

            ChangeState(initialState);
        }

        public void Tick(float deltaTime)
        {
            if (deltaTime < 0f)
            {
                throw new ArgumentOutOfRangeException(nameof(deltaTime));
            }

            Current?.Tick(deltaTime);
        }

        public void ChangeState(IState nextState)
        {
            if (nextState == null)
            {
                throw new ArgumentNullException(nameof(nextState));
            }

            if (ReferenceEquals(Current, nextState))
            {
                return;
            }

            if (isTransitioning)
            {
                throw new InvalidOperationException("Nested state transitions are not supported.");
            }

            IState previousState = Current;
            isTransitioning = true;

            try
            {
                previousState?.Exit();
                Current = nextState;
                nextState.Enter();
                Changed?.Invoke(previousState, nextState);
            }
            finally
            {
                isTransitioning = false;
            }
        }
    }
}
