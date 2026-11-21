using System.Collections.Generic;
using Aniki.GameplaySystems.Core.StateMachine;
using NUnit.Framework;

namespace Aniki.GameplaySystems.Tests
{
    public sealed class StateMachineTests
    {
        [Test]
        public void TransitionRunsExitBeforeNextEnter()
        {
            List<string> calls = new List<string>();
            TestState first = new TestState("first", calls);
            TestState second = new TestState("second", calls);
            StateMachine machine = new StateMachine();

            machine.Start(first);
            machine.ChangeState(second);

            CollectionAssert.AreEqual(
                new[] { "first.enter", "first.exit", "second.enter" },
                calls);
            Assert.AreSame(second, machine.Current);
        }

        [Test]
        public void ChangingToCurrentStateDoesNothing()
        {
            TestState state = new TestState("state", new List<string>());
            StateMachine machine = new StateMachine();

            machine.Start(state);
            machine.ChangeState(state);

            Assert.AreEqual(1, state.EnterCount);
            Assert.AreEqual(0, state.ExitCount);
        }

        [Test]
        public void TickIsForwardedToCurrentState()
        {
            TestState state = new TestState("state", new List<string>());
            StateMachine machine = new StateMachine();
            machine.Start(state);

            machine.Tick(0.25f);

            Assert.AreEqual(0.25f, state.TotalTime, 0.0001f);
        }

        private sealed class TestState : IState
        {
            private readonly string name;
            private readonly List<string> calls;

            public TestState(string name, List<string> calls)
            {
                this.name = name;
                this.calls = calls;
            }

            public int EnterCount { get; private set; }
            public int ExitCount { get; private set; }
            public float TotalTime { get; private set; }

            public void Enter()
            {
                EnterCount++;
                calls.Add(name + ".enter");
            }

            public void Tick(float deltaTime)
            {
                TotalTime += deltaTime;
            }

            public void Exit()
            {
                ExitCount++;
                calls.Add(name + ".exit");
            }
        }
    }
}
