# Randomazing

**A procedural, top-down action maze game built with Python and Pygame.**

Navigate a changing network of rooms, fight enemies, collect batteries, and reach the exit before the countdown runs out. All visuals are rendered with Pygame primitives; **no external sprites or asset downloads are needed**.

> **Repository status:** Refactored from an earlier working-style codebase. The full application entry point and game rules are present. Source compilation, module import resolution, static checks, and simulated smoke checks were verified during refactoring, but a live Pygame playthrough was **not** available in the refactoring environment. Please perform the manual checklist below before marking the project as fully playtested.

## Quick start

**Prerequisites:** Python **3.11+** and a desktop system with a display.

```bash
# Clone the repository, then run these commands from its root:
python -m venv .venv

# Windows:
.venv\Scripts\activate
# macOS / Linux:
source .venv/bin/activate

python -m pip install -e .
python -m randomazing
```

Alternative after installation: `randomazing`.

If Pygame reports a missing display or an SDL initialization error, run the game from a local graphical desktop. The headless `dummy` display is intended for automated tests, not interactive play.

## How to play

**Goal:** Collect **10 batteries** before the first timer runs out. The exit will then appear on the minimap; reach it before the shorter escape countdown expires. The maze periodically regenerates as you travel between rooms.

| Input | Action |
| --- | --- |
| `W`, `A`, `S`, `D` | Move |
| Mouse move + left click | Aim and use the equipped item |
| Hold right mouse button | Shield that absorbs up to 3 hits per activation (movement slows) |
| `Q` / `E` | Switch inventory item |
| `Esc` | Pause / resume (the countdown pauses too) |
| `R` | Restart after winning, dying, or running out of time |
| Window close button | Quit |

The permanent starter weapon is a **knife**. Other pickups include **keys** (open chests), **health potions**, and a **shooter**. Enemy variants include regular ranged enemies, key guards, rotating turrets, and elites. The lower-left HUD tracks health, score, moves, inventory, batteries, and remaining time.

## What makes it interesting

- **Procedural maze generation:** Wilson’s loop-erased random-walk algorithm connects room cells into a traversable maze, with extra corridors and several merged room shapes.
- **Dynamic level changes:** The map can reroll after several room transitions, changing routes while preserving the active player room.
- **Reusable collision strategies:** Collision handling dispatches behaviors based on pairs of sprite types (blocking, pickups, damage, tunnel travel, and melee interactions).
- **Action gameplay:** Directional melee arcs, projectile combat, enemy state machines, knock-back-style pickup motion, consumable items, and a limited-use shield.
- **Camera and minimap:** The camera smoothly follows the player while clamping to active room boundaries; the minimap is generated from current cell/tunnel topology.
- **Two-stage win condition:** Battery collection unlocks the exit and switches to a short escape timer.

## Code organization

```text
Randomazing/
├── src/randomazing/
│   ├── __main__.py       # `python -m randomazing` entry point
│   ├── application.py    # Game lifecycle, input, simulation, and rendering
│   ├── settings.py       # Gameplay parameters and display dimensions
│   ├── state.py          # Resettable shared session state
│   ├── geometry.py       # Directions and collision/math utilities
│   ├── grid.py           # Maze generation and minimap
│   ├── cells.py          # Room sizes, layouts, spawning, and merging
│   ├── layouts.py        # Room-content presets and tile construction
│   ├── tunnels.py       # Maze connectivity and transitions
│   ├── tiles.py         # Walls, tunnels, chests, exits
│   ├── collision.py     # Strategy-based collision manager
│   ├── actors.py        # Shared unit/health functionality
│   ├── player.py        # Inventory, input-driven combat, movement
│   ├── enemies.py       # Enemy archetypes and attack patterns
│   ├── projectiles.py   # Bullet entities
│   ├── items.py         # Collectibles and pickup effects
│   ├── close_area.py    # Melee and close-range interactions
│   ├── camera.py        # Camera smoothing and bounds
│   └── hud.py           # UI and game status
├── tests/                # Headless game smoke tests and unit tests
├── .github/workflows/    # CI tests on multiple Python versions
├── docs/ARCHITECTURE.md  # System overview and technical decisions
└── pyproject.toml        # Package metadata, dependencies, CLI script
```

The original prototype used a single shared dictionary and sprite registries. This refactor preserves that small-game architecture rather than pretending it is a fully decoupled ECS. The `Game` controller owns creation, reset, event processing, and the frame loop; mutable state stays explicit in `state.py`.

## Tests

```bash
python -m pip install -e '.[test]'
python -m pytest
```

For headless Linux CI, the test environment uses `SDL_VIDEODRIVER=dummy` and `SDL_AUDIODRIVER=dummy`. The tests check world initialization, reset, loot placement, collision cleanup, and exit unlocking. **The 11 included tests passed against a lightweight compatibility simulation during refactoring; they have **not yet been run against the real Pygame package** in this environment. Automated smoke tests do not replace a human playthrough.**

### Manual playtest before publishing

- [ ] Install and launch the game on the target desktop OS.
- [ ] Verify movement, collision, camera, and minimap.
- [ ] Test knife, key/chest drops, shooter, health potions, and shield.
- [ ] Travel through rooms until the maze reshuffles and verify there are no invisible walls.
- [ ] Collect 10 batteries, reach the marked exit, and confirm the win state.
- [ ] Let a timer expire, then confirm `R` starts a fresh run.
- [ ] Verify `Esc` pauses gameplay **and** the countdown.
- [ ] Record a short gameplay GIF/video and add a screenshot near the top of this README.

## Development notes

See [Architecture](docs/ARCHITECTURE.md) for the game loop, procedural generation, and collision pipeline. See [Refactor Notes](docs/REFACTOR_NOTES.md) for changes from the original archive and known limitations.

**License:** This repository does not assume a license or claim rights not established by the project owner. If you own all the code and choose to make it reusable, add a license that suits your goals before publishing.
