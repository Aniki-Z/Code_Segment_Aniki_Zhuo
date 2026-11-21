"""Application lifecycle, input handling, simulation updates, and drawing."""

from __future__ import annotations

import random
import pygame

from .camera import Camera
from .cells import Cell, LargeCell, SmallCell, get_all_cells
from .enemies import Turret
from .grid import Grid
from .hud import Infotab
from .items import DroppedItems
from .player import Player
from .projectiles import Projectile
from .settings import (
    BATTERIES_REQUIRED, CELL_HEIGHT, CELL_WIDTH, FONT, FPS, HEIGHT,
    MAZE_SIZE, TILE_WIDTH, WIDTH,
)
from .state import current_cells, glob_var, reset_runtime_state
from .tiles import Tile, Wall
from .tunnels import Tunnel


class Game:
    """Controller for a single local gameplay session.

    The underlying entity systems share one active world through `glob_var`.
    This class owns its lifecycle and keeps event handling/frame timing explicit.
    """

    def __init__(self, seed: int | None = None) -> None:
        self.seed = seed
        self.running = False
        self._paused_at: int | None = None

    @staticmethod
    def _clear_sprite_registries() -> None:
        """Release sprites and collision registrations from the previous run."""
        for group in (
            Projectile.all_projectiles, DroppedItems.all_items,
            SmallCell.all_small_cells, LargeCell.all_large_cells,
            Tunnel.all_tunnels, Tile.all_tiles, Wall.all_walls,
        ):
            for sprite in group.sprites():
                sprite.kill()

    def start(self) -> None:
        """Build a fresh maze, world state, player, and display."""
        pygame.init()
        if self.seed is not None:
            random.seed(self.seed)
        self._clear_sprite_registries()
        reset_runtime_state()
        self._paused_at = None
        pygame.display.set_caption("Randomazing")
        glob_var["font"] = pygame.font.Font(pygame.font.match_font(FONT), 20)
        glob_var["screen"] = pygame.display.set_mode((WIDTH, HEIGHT))
        glob_var["clock"] = pygame.time.Clock()
        glob_var["timer"] = pygame.time.get_ticks()
        glob_var["timer_start"] = glob_var["timer"]

        glob_var["grid"] = Grid(MAZE_SIZE, MAZE_SIZE)
        starting_cell = glob_var["grid"].cells[MAZE_SIZE // 2][MAZE_SIZE // 2]
        current_cells.append(starting_cell)
        glob_var["player"] = Player(starting_cell, 7, 100, 10)
        glob_var["infotab"] = Infotab(glob_var["font"], glob_var["player"])
        for cell in get_all_cells():
            cell.make_layout()
        glob_var["scene"] = pygame.Surface(
            (MAZE_SIZE * CELL_WIDTH, MAZE_SIZE * CELL_HEIGHT)
        )
        glob_var["camera"] = Camera(glob_var["scene"], starting_cell.pos)
        glob_var["grid"].randomize()
        self.running = True

    def _toggle_pause(self) -> None:
        now = pygame.time.get_ticks()
        if glob_var["paused"]:
            # Remove the time spent paused from the in-game countdown.
            assert self._paused_at is not None
            glob_var["timer_start"] += now - self._paused_at
            self._paused_at = None
            glob_var["paused"] = False
        else:
            glob_var["paused"] = True
            self._paused_at = now

    def handle_events(self) -> None:
        """Process controls; game-over and pause states still allow quitting."""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
                return
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE and not any(
                    glob_var[flag] for flag in ("dead", "time_up", "exited")
                ):
                    self._toggle_pause()
                    continue
                if event.key == pygame.K_r and any(
                    glob_var[flag] for flag in ("dead", "time_up", "exited")
                ):
                    self.start()
                    return
            if event.type == pygame.MOUSEBUTTONUP and event.button == pygame.BUTTON_RIGHT:
                glob_var["player"].shield = 0
            if any(glob_var[flag] for flag in ("paused", "dead", "time_up", "exited")):
                continue
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_q:
                    glob_var["player"].switch_item("l")
                elif event.key == pygame.K_e:
                    glob_var["player"].switch_item("r")
            elif event.type == pygame.MOUSEBUTTONDOWN:
                world_mouse = (
                    pygame.Vector2(glob_var["camera"].rect.topleft)
                    + pygame.Vector2(event.pos)
                    - pygame.Vector2(MAZE_SIZE * TILE_WIDTH, 0)
                )
                if event.button == pygame.BUTTON_LEFT:
                    glob_var["player"].use_item(world_mouse)
                elif event.button == pygame.BUTTON_RIGHT:
                    glob_var["player"].shield = 3

    def update(self) -> None:
        """Advance the original sprite simulation by one frame."""
        player = glob_var["player"]
        grid = glob_var["grid"]
        camera = glob_var["camera"]

        # Refresh elapsed time before applying the expiration rule.
        if not glob_var["paused"]:
            glob_var["timer"] = pygame.time.get_ticks()

        # Global status check
        if glob_var["paused"]:
            return
        if (glob_var["timer"] - glob_var["timer_start"]) // 1000 >= glob_var["timelimit_1"] and not glob_var["exitable"]:
            glob_var["time_up"] = True
        elif (glob_var["timer"] - glob_var["timer_start"]) // 1000 >= glob_var["timelimit_2"] and glob_var["exitable"]:
            glob_var["time_up"] = True
    
        if glob_var["dead"] or glob_var["time_up"] or glob_var["exited"]:
            return
        if player.batteries >= BATTERIES_REQUIRED and not glob_var["exitable"]:
            grid.set_exit()
            glob_var["exitable"] = True
            glob_var["timer_start"] = pygame.time.get_ticks()

        #if enemies in cell, close down
        if len([enemy for enemy in current_cells[-1].enemies if not isinstance(enemy, Turret)]) > 0:
            current_cells[0].close_down()
        #else, open up
        else:
            current_cells[0].open_up()
    
        #units action
        for enemy in current_cells[-1].enemies:
            enemy.action()
        player.action()
    
        #item friction
        for item in DroppedItems.all_items:
            item.acc = pygame.Vector2(0, 0)
            if item.vel != pygame.Vector2(0, 0):
                item.acc -= item.vel.normalize() * 0.8
            vel_tmp = item.vel + item.acc
            if item.vel.x * vel_tmp.x < 0:
                item.vel.x = 0
            else:
                item.vel.x = vel_tmp.x
            if item.vel.y * vel_tmp.y < 0:
                item.vel.y = 0
            else:
                item.vel.y = vel_tmp.y
    
        #don't handle collisions when in tunnel
        if isinstance(player.location, Cell):
            cms = [cell.cm for cell in current_cells]

            #first move along x
            #player
            vel_tmp = player.vel.copy()
            player.rect.left += player.vel.x
            player.vel.y = 0
            #enemies
            for enemy in current_cells[0].enemies:
                enemy.vel_tmp = enemy.vel.copy()
                enemy.rect.left += enemy.vel.x
                enemy.vel.y = 0
            #projectiles
            for projectile in Projectile.all_projectiles:
                projectile.rect.left += projectile.vel.x
            #items
            for item in DroppedItems.all_items:
                item.vel_tmp = item.vel.copy()
                item.rect.left += item.vel.x
                item.vel.y = 0
            #first collision check
            for cm in cms:
                cm.check_collisions()
            #vel recover
            player.vel.y = vel_tmp.y
            for enemy in current_cells[0].enemies:
                enemy.vel.y = enemy.vel_tmp.y
            for item in DroppedItems.all_items:
                item.vel.y = item.vel_tmp.y

            #deactivate closearea
            player.closearea.deactivate(0, 16)

            #then move along y
            #player
            player.rect.top += player.vel.y
            player.vel.x = 0
            player.closearea.move(player.rect.center)
            #enemies
            for enemy in current_cells[0].enemies:
                enemy.rect.top += enemy.vel.y
                enemy.vel.x = 0
            #projectiles
            for projectile in Projectile.all_projectiles:
                projectile.rect.top += projectile.vel.y
            #items
            for item in DroppedItems.all_items:
                item.rect.top += item.vel.y
                item.vel.x = 0
            #second collision check
            for cm in cms:
                cm.check_collisions()
            #vel recover
            player.vel.x = vel_tmp.x
            for enemy in current_cells[0].enemies:
                enemy.vel.x = enemy.vel_tmp.x
            for item in DroppedItems.all_items:
                item.vel.x = item.vel_tmp.x
    
        #don't handle collisions when in tunnel
        else:
            player.rect.center += player.vel
            for projectile in Projectile.all_projectiles:
                projectile.rect.center += projectile.vel
            for item in DroppedItems.all_items:
                item.rect.center += item.vel

        #player moving_state recover from transition
        if current_cells[-1].is_including(player.rect):
            player.moving_state = 'free'
        #current_cells update
        if player.moving_state == 'free' and len(current_cells) > 1:
            current_cells.pop(0)
    
        #camera movement
        camera.follow(player.rect.center)

    def render(self) -> None:
        """Draw world, minimap, and HUD."""
        screen = glob_var["screen"]
        grid = glob_var["grid"]
        scene = glob_var["scene"]
        player = glob_var["player"]
        camera = glob_var["camera"]
        infotab = glob_var["infotab"]

        #objects rendering
        screen.fill("black")
        scene.fill("black")
        grid.render_minimap(screen)
        infotab.render(screen)
        for cell in current_cells:
            cell.render_layout(scene)
            cell.render_content(scene)
        for projectile in Projectile.all_projectiles:
            projectile.render(scene)
        player.render(scene)
        screen.blit(scene, (MAZE_SIZE * TILE_WIDTH, 0), camera.rect)

        #flip
        pygame.display.flip()

    def run(self) -> None:
        """Run until the player closes the window."""
        self.start()
        try:
            while self.running:
                self.handle_events()
                if not self.running:
                    break
                self.update()
                self.render()
                # Tick even while paused or after death to avoid a busy loop.
                glob_var["clock"].tick(FPS)
        finally:
            pygame.quit()


def main() -> None:
    Game().run()
