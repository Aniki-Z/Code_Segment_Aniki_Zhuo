"""Player movement, input-driven actions, inventory, and survival."""

import random
import pygame

from .actors import Unit
from .close_area import CloseArea, key_render, knife_render
from .enemies import Enemy
from .geometry import get_direction
from .items import ScorePoint
from .projectiles import Projectile
from .settings import CELL_HEIGHT, CELL_WIDTH, SAFE_MOVES_TOTAL
from .state import current_cells, glob_var
from .tiles import Chest

class Player(Unit):
    faction = "player"

    def __init__(self, cell, speed, health, attack):
        super().__init__(cell, pygame.Vector2(CELL_WIDTH / 2, CELL_HEIGHT / 2), speed, health, attack)
        self.rect.center = cell.bound.center
        self.closearea = CloseArea(self.rect.center)
        self.acc = pygame.Vector2(0, 0)
        self.facing = [1, 0]
        self.shield = 0
        self.moving_state = 'free'
        self.rand_prob = 0
        self.safe_moves = SAFE_MOVES_TOTAL
        self.moves = 0
        self.score = 0
        self.batteries = 0
        self.attack_interval = 500
        self.items = [['Knife', float('INF')]]
        self.curr_item_idx = -1
        self.switch_item('r')
        self.attack_timer = pygame.time.get_ticks()
        self.location.player_enter(self)
    def pickup_item(self, item_name, item_cnt):
        for item in self.items:
            if item[0] == item_name:
                item[1] += item_cnt
                break
        else:
            self.items.append([item_name, item_cnt])
    def switch_item(self, direction):
        match direction:
            case 'l':
                self.curr_item_idx = (self.curr_item_idx - 1) % len(self.items)
            case 'r':
                self.curr_item_idx = (self.curr_item_idx + 1) % len(self.items)
        self._equip_current_item()
    def _equip_current_item(self) -> None:
        """Select the interaction applied by the active inventory slot."""
        match self.items[self.curr_item_idx][0]:
            case 'Knife':
                self.closearea.set_func(
                    lambda obj: obj.take_damage(random.randint(self.attack - 1, self.attack + 3))
                    if isinstance(obj, Enemy) else None
                )
                self.closearea.set_render(knife_render)
            case 'Key':
                self.closearea.set_func(
                    lambda obj: self.open_chest(obj) if isinstance(obj, Chest) else None
                )
                self.closearea.set_render(key_render)
            case _:
                self.closearea.set_func(lambda _: None)
                self.closearea.set_render(lambda _: None)

    def _remove_exhausted_item(self, item) -> None:
        if item[1] > 0:
            return
        self.items.pop(self.curr_item_idx)
        # The starter knife is permanent, so there is always another slot.
        self.curr_item_idx %= len(self.items)
        self._equip_current_item()

    def use_item(self, mouse_pos: pygame.Vector2):
        curr_item = self.items[self.curr_item_idx]
        match curr_item[0]:
            case 'Shooter':
                self.shoot_bullet(mouse_pos)
            case 'Knife':
                self.knife_attack(mouse_pos)
            case 'Health Potion':
                if self.health == self.max_health:
                    return
                self.health += 20
                self.health = min(self.health, self.max_health)
            case 'Key':
                self.closearea.activate(0, 16)
                return
        curr_item[1] -= 1
        self._remove_exhausted_item(curr_item)
    def open_chest(self, chest):
        if chest.opened:
            return
        items = random.choice(chest.loot_table)
        for item in items:
            if type(item) is tuple:
                item[0](chest.cell, pygame.Vector2(chest.rect.center) - chest.cell.pos, pygame.Vector2(random.uniform(-1, 1), random.uniform(-1, 1)), item[1])
            else:
                item(chest.cell, pygame.Vector2(chest.rect.center) - chest.cell.pos, pygame.Vector2(random.uniform(-1, 1), random.uniform(-1, 1)))
        chest.opened = True
        curr_item = self.items[self.curr_item_idx]
        curr_item[1] -= 1
        self._remove_exhausted_item(curr_item)
    def knife_attack(self, mouse_pos: pygame.Vector2):
        curr_time = pygame.time.get_ticks()
        if self.moving_state != 'free' or self.shield or curr_time - self.attack_timer <= self.attack_interval:
            return
        self.attack_timer = curr_time
        src_pos = pygame.Vector2(self.rect.center)
        dest_pos = mouse_pos
        direction = get_direction(src_pos, dest_pos)
        match direction:
            case 'u':
                start = 10
            case 'ur':
                start = -4
            case 'r':
                start = -2
            case 'dr':
                start = 0
            case 'd':
                start = 2
            case 'dl':
                start = 4
            case 'l':
                start = 6
            case 'ul':
                start = 8
        self.closearea.activate(start, start + 5)
    def shoot_bullet(self, mouse_pos: pygame.Vector2):
        curr_time = pygame.time.get_ticks()
        if self.moving_state != 'free' or self.shield or curr_time - self.attack_timer <= self.attack_interval:
            return
        self.attack_timer = curr_time
        src_pos = pygame.Vector2(self.rect.center)
        dest_pos = mouse_pos
        vel = dest_pos - src_pos
        if vel == pygame.Vector2(0, 0):
            return
        vel = vel.normalize() * 10
        Projectile(self, random.randint(self.attack, self.attack+5), self.rect.center, vel, self.location.cm)
    def take_damage(self, damage):
        if self.shield > 0:
            self.shield -= 1
            return
        self.health -= damage
        if self.health <= 0:
            self.health = 0
            self.dead()
    def action(self):
        grid = glob_var["grid"]
        #player get acc and calc vel
        self.acc = pygame.Vector2(0, 0)
        max_speed = self.speed
        if self.moving_state == 'free': #if free, read keyboard inputs
            if grid.should_randomize:
                grid.randomize()
            self.location = current_cells[0]
            pressed_keys = pygame.key.get_pressed()
            facing_tmp = [0, 0]
            if pressed_keys[pygame.K_a]:
                self.acc.x -= self.speed / 2
                facing_tmp[0] -= 1
            if pressed_keys[pygame.K_d]:
                self.acc.x += self.speed / 2
                facing_tmp[0] += 1
            if pressed_keys[pygame.K_w]:
                self.acc.y -= self.speed / 2
                facing_tmp[1] -= 1
            if pressed_keys[pygame.K_s]:
                self.acc.y += self.speed / 2
                facing_tmp[1] += 1
            if facing_tmp != [0, 0]:
                self.facing = facing_tmp
            if self.shield > 0:
                max_speed = 2
        elif self.moving_state == 'transition': #if in transition, move in the tunnel direction
            self.acc.x += self.facing[0] * self.speed / 2
            self.acc.y += self.facing[1] * self.speed / 2
        else:
            assert(0)
        #friction
        if self.vel != pygame.Vector2(0, 0):
            self.acc -= self.vel.normalize() * 2
        vel_tmp = self.vel + self.acc
        if self.vel.x * vel_tmp.x < 0:
            self.vel.x = 0
        else:
            self.vel.x = vel_tmp.x
        if self.vel.y * vel_tmp.y < 0:
            self.vel.y = 0
        else:
            self.vel.y = vel_tmp.y
        #no more than max_speed
        if self.vel.length() > max_speed:
            self.vel = self.vel.normalize() * max_speed
    def dead(self):
        glob_var["dead"] = True
    def move_to(self, next_cell):
        self.moving_state = 'transition'
        for obj in Projectile.all_projectiles:
            obj.kill()
        current_cells[0].player_leave(self)
        next_cell.player_enter(self)
        self.safe_moves -= 1
        self.moves += 1
        if glob_var["exitable"]:
            if self.safe_moves <= 0:
                self.rand_prob = 50
                if random.randint(1, 100) < self.rand_prob:
                    self.rand_prob = 0
                    self.safe_moves = 2
                    glob_var["grid"].should_randomize = True
        else:
            if self.safe_moves <= 0:
                self.rand_prob = min(self.rand_prob + 5, 50)
                if random.randint(1, 100) < self.rand_prob:
                    self.rand_prob = 0
                    self.safe_moves = SAFE_MOVES_TOTAL
                    glob_var["grid"].should_randomize = True
    def render(self, surface):
        self.closearea.render(surface)
        if self.shield > 0:
            self.surface.fill("orange")
        else:
            self.surface.fill("red")
        surface.blit(self.surface, self.rect)
        super().render(surface)
