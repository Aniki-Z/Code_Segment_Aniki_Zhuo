# Quick Start Sample

This sample contains two small adapters:

- `KeyboardInputAdapter` converts WASD, Space, and E into package calls.
- `ToggleInteractable` demonstrates a concrete interaction target.

## Scene setup

1. Create a capsule named `Player`.
2. Add `CharacterController`, `PlayerController`, `InteractionScanner`, `PlayerInteractor`, `Inventory`, and `KeyboardInputAdapter`.
3. Create a `PlayerMovementSettings` asset and assign it.
4. Assign the player and interactor references on `KeyboardInputAdapter`.
5. Create a cube with a collider and `ToggleInteractable`.
6. Assign a different scene object as the toggle target.
7. Enter Play Mode: use **WASD** to move, **Space** to dash, and **E** to interact.

The runtime package does not depend on this keyboard adapter. A production project can submit input from Unity Input System, another input backend, AI, replay, or network code instead.
