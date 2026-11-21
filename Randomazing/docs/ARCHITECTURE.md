# Architecture and design notes

## Frame lifecycle

`Game.run()` owns initialization, input, state updates, drawing, and frame pacing.

```text
Game.start()
  -> reset sprite registries and session state
  -> construct Grid (small cells + tunnels)
  -> place player into center cell
  -> create static wall layouts and HUD
  -> randomize maze topology and room contents

Game.run() at up to 60 FPS
  -> handle_events() (including pause, restart, quit)
  -> update()        (game rules, enemy & player actions, 2-axis collisions)
  -> render()        (minimap, HUD, tiles, sprites, camera blit)
  -> clock.tick(60)
```

The original project updates velocity and movement on a **fixed amount per frame** rather than elapsed delta time, so 60 FPS is used intentionally. A future improvement would be a fixed-step accumulator or delta-time-based simulation to make the game independent of rendering frame rate.

## Generation pipeline

1. Start with a 5 × 5 grid of room cells and adjacent tunnels.
2. Optionally merge nearby cells into larger horizontal, vertical, L-shaped, or four-cell rooms.
3. Use **Wilson’s algorithm** (a loop-erased random walk) to create a spanning tree connecting the available rooms.
4. Randomly open a subset of extra tunnels for additional route options.
5. Choose a battery room, lock its entry tunnels, and place a special chest.
6. Based on progression, potentially create a shooter chest.
7. Populate the remaining rooms from weighted content templates (enemies, turrets, chests, or potions).
8. After enough player room transitions, reshuffle the unoccupied portion of the world.

`Grid.randomize()` handles generation; `cells.py` manages merge/unmerge behavior. `layouts.py` contains declarative room content tables and wall/tunnel construction.

## Collision design

Each room has a `CollisionManager` with dynamic and static sprite groups. Strategies are registered for type pairs:

| Pair | Response |
| --- | --- |
| Unit × Wall | Stop movement |
| Player × TunnelEntry | Transition into the connected room when open |
| Projectile × Wall | Remove projectile |
| Projectile × Unit | Invoke projectile damage callback |
| DroppedItem × Player | Pick up item |
| CloseArea × Enemy | Apply melee interaction |
| Exit × Player | Set victory state |

Within the world tick, collisions are resolved separately along X and Y axes for more predictable tile blocking.

## Engineering changes

- Organized modules under the `randomazing` Python package with explicit dependencies.
- Separated the shared actor base, player behavior, and enemy archetypes.
- Encapsulated the main loop inside a `Game` controller; added a real exit from the main loop.
- Clock ticks regardless of pause/game-over status to prevent uncontrolled busy-spinning.
- Pauses no longer consume the in-game countdown; end states can restart without relaunching Python.
- Fixed chest loot spawning to convert world coordinates into cell-local offsets exactly once.
- Fixed wall teardown to remove sprites from all collision groups when rooms merge.
- Added smoke tests and reproducible project metadata.

## Technical debt worth discussing in an interview

- **State:** Entity classes still refer to a shared game-state dictionary. A later iteration could pass an explicit `World`/`GameContext` object to systems.
- **Frame-rate coupling:** The update uses per-frame velocities. A fixed-step update would improve consistency on slow machines.
- **Data definitions:** Room templates are Python lists; JSON/YAML or dataclasses could allow designer-friendly authoring and validation.
- **Spatial efficiency:** The current collision manager checks registered pairs directly, without a grid-based broad phase.
- **Persistence and polish:** No save system or external art/audio pipeline is present; this version is a playable-code portfolio candidate rather than a shipped commercial title.
