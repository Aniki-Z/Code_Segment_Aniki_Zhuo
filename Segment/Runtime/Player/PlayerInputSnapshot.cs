using UnityEngine;

namespace Aniki.GameplaySystems.Player
{
    public readonly struct PlayerInputSnapshot
    {
        public PlayerInputSnapshot(Vector2 move, bool dashPressed)
        {
            Move = Vector2.ClampMagnitude(move, 1f);
            DashPressed = dashPressed;
        }

        public Vector2 Move { get; }
        public bool DashPressed { get; }

        public PlayerInputSnapshot WithoutTransientButtons()
        {
            return new PlayerInputSnapshot(Move, false);
        }
    }
}
