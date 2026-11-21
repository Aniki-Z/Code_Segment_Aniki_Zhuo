"""Shared state for the original sprite-based gameplay systems.

The game world uses a single active session. State is reset when starting a new
game, including all class-level sprite registries in the application module.
"""

from .settings import TIME_LIMIT_ESCAPE_SECONDS, TIME_LIMIT_EXPLORE_SECONDS

current_cells = []

glob_var = {
    "paused": False,
    "exitable": False,
    "exited": False,
    "dead": False,
    "time_up": False,
    "has_shooter": False,
    "grid": None,
    "player": None,
    "scene": None,
    "camera": None,
    "font": None,
    "screen": None,
    "infotab": None,
    "clock": None,
    "timer_start": None,
    "timer": None,
    "timelimit_1": TIME_LIMIT_EXPLORE_SECONDS,
    "timelimit_2": TIME_LIMIT_ESCAPE_SECONDS
}

_DEFAULTS = glob_var.copy()

def reset_runtime_state() -> None:
    """Reset mutable flags in-place so imported references remain valid."""
    current_cells.clear()
    glob_var.clear()
    glob_var.update(_DEFAULTS)
