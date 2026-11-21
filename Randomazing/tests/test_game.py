"""Cross-module Pygame tests for game initialization and core progression."""

import pygame

from randomazing.cells import SmallCell
from randomazing.items import ScorePoint
from randomazing.settings import BATTERIES_REQUIRED, MAZE_SIZE
from randomazing.state import current_cells, glob_var
from randomazing.tiles import Chest
from randomazing.tunnels import Tunnel


def test_world_initializes_and_renders(game):
    assert len(SmallCell.all_small_cells) == MAZE_SIZE ** 2
    assert len(Tunnel.all_tunnels) == 2 * MAZE_SIZE * (MAZE_SIZE - 1)
    assert len(current_cells) == 1
    assert glob_var["player"].health == 100
    for _ in range(3):
        game.update()
        game.render()
    assert glob_var["dead"] is False


def test_battery_goal_spawns_exit(game):
    glob_var["player"].batteries = BATTERIES_REQUIRED
    game.update()
    assert glob_var["exitable"] is True
    assert glob_var["grid"].exit_cell is not None


def test_chest_loot_uses_world_position_once(game):
    cell = current_cells[0]
    chest = Chest(cell, pygame.Vector2(400, 300), [[(ScorePoint, 1)]])
    old_items = set(ScorePoint.all_items.sprites())
    glob_var["player"].open_chest(chest)
    new_items = set(ScorePoint.all_items.sprites()) - old_items
    assert len(new_items) == 1
    item = new_items.pop()
    assert item.rect.center == chest.rect.center


def test_clearing_room_layout_removes_collision_walls(game):
    cell = current_cells[0]
    assert len(cell.cm.static_objects) > 0
    cell.clear_layout()
    assert len(cell.cm.static_objects) == 0
    cell.make_layout()
    assert len(cell.cm.static_objects) > 0


def test_restart_resets_world_and_player(game):
    old_player = glob_var["player"]
    glob_var["player"].score = 999
    game.start()
    assert glob_var["player"] is not old_player
    assert glob_var["player"].score == 0
    assert len(SmallCell.all_small_cells) == MAZE_SIZE ** 2
    assert len(current_cells) == 1


def test_pausing_freezes_countdown(game, monkeypatch):
    base_time = pygame.time.get_ticks()
    timer_start = glob_var["timer_start"]
    displayed_time = glob_var["timer"]

    monkeypatch.setattr(pygame.time, "get_ticks", lambda: base_time + 100)
    game._toggle_pause()
    assert glob_var["paused"]

    monkeypatch.setattr(pygame.time, "get_ticks", lambda: base_time + 5100)
    game.update()
    assert glob_var["timer"] == displayed_time

    game._toggle_pause()
    assert not glob_var["paused"]
    assert glob_var["timer_start"] == timer_start + 5000
