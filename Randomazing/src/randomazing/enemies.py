"""Enemy behaviors: ranged soldiers, key guards, turrets, and elites."""

import math
import random
import pygame

from .actors import Unit
from .geometry import safe_normalize
from .items import HealthPotion, Key, ScorePoint
from .projectiles import Projectile
from .state import current_cells, glob_var

class Enemy(Unit):
    faction = "enemy"

    def _target_away_from_player(self, player, distance: float, bias: pygame.Vector2):
        """Choose a point away from the player, even when both units overlap."""
        player_pos = pygame.Vector2(player.rect.center)
        away = safe_normalize(
            pygame.Vector2(self.rect.center) - player_pos, pygame.Vector2(1, 0)
        )
        return player_pos + away * distance + bias

    def _spread_offset(self, target: pygame.Vector2, spread: float):
        """Perpendicular offset for a fan of projectiles."""
        displacement = target - pygame.Vector2(self.rect.center)
        perpendicular = pygame.Vector2(displacement.y, -displacement.x)
        return safe_normalize(perpendicular) * displacement.length() * spread

    def __init__(self, cell, pos: pygame.Vector2, speed, health, attack):
        super().__init__(cell, pos, speed, health, attack)
        self.timer = pygame.time.get_ticks()
        self.target = pygame.Vector2(0, 0)
        self.vel_tmp = self.vel.copy()
        cell.enemies.add(self)
        cell.cm.add_dynamic(self)
    def shoot_bullet(self, target_pos: pygame.Vector2, bullet_speed):
        src_pos = pygame.Vector2(self.rect.center)
        delta = target_pos - src_pos
        if delta.length() == 0:
            vel = safe_normalize(pygame.Vector2(random.uniform(-1, 1), random.uniform(-1, 1)), pygame.Vector2(1, 0)) * bullet_speed
        else:
            vel = safe_normalize(target_pos - src_pos) * bullet_speed
        Projectile(self, random.randint(self.attack, self.attack+2), self.rect.center, vel, current_cells[0].cm)
    def render(self, surface):
        super().render(surface)
    def action(self):
        pass
    def dead(self):
        pass

class NormalEnemy(Enemy):
    def __init__(self, cell, pos: pygame.Vector2):
        super().__init__(cell, pos, 5, 20, 5)
        self.state = 'idle'
        self.idle_interval = random.randint(300, 400)
        self.move_interval = random.randint(500, 750)
        self.shoot_interval = 250
    def render(self, surface):
        self.surface.fill("darkgreen")
        surface.blit(self.surface, self.rect)
        super().render(surface)
    def action(self):
        player = glob_var["player"]
        curr_time = pygame.time.get_ticks()
        match self.state:
            case 'idle':
                self.vel = pygame.Vector2(0, 0)
                if curr_time > self.timer + self.idle_interval:
                    self.state = 'move'
                    self.timer = curr_time
                    self.move_interval = random.randint(500, 750)
                    bias = pygame.Vector2(random.uniform(-100, 100), random.uniform(-100, 100))
                    self.target = self._target_away_from_player(player, 250, bias)
                    delta = self.target - pygame.Vector2(self.rect.center)
                    self.vel = safe_normalize(delta) * min(self.speed, delta.length())
            case 'move':
                delta = self.target - pygame.Vector2(self.rect.center)
                self.vel = safe_normalize(delta) * min(self.speed, delta.length())
                if curr_time > self.timer + self.move_interval or self.target == pygame.Vector2(self.rect.center):
                    self.state = 'shoot'
                    self.timer = curr_time
                    self.vel = pygame.Vector2(0, 0)
            case 'shoot':
                self.vel = pygame.Vector2(0, 0)
                if curr_time > self.timer + self.shoot_interval:
                    self.shoot_bullet(pygame.Vector2(player.rect.center), 7)
                    self.timer = curr_time
                    self.state = 'idle'
                    self.idle_interval = random.randint(300, 400)
    def dead(self):
        for _ in range(random.randint(1, 2)):
            vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
            ScorePoint(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel, None)
        self.kill()

class KeyGuard(Enemy):
    def __init__(self, cell, pos: pygame.Vector2):
        super().__init__(cell, pos, 3, 40, 5)
        self.state = 'idle'
        self.idle_interval = random.randint(300, 400)
        self.move_interval = random.randint(600, 850)
        self.shoot_interval = 250
        self.shoot_interval2 = 100
        self.shoot_cnt = 0
    def render(self, surface):
        self.surface.fill("darkblue")
        surface.blit(self.surface, self.rect)
        super().render(surface)
    def action(self):
        player = glob_var["player"]
        curr_time = pygame.time.get_ticks()
        match self.state:
            case 'idle':
                self.vel = pygame.Vector2(0, 0)
                if curr_time > self.timer + self.idle_interval:
                    self.state = 'move'
                    self.timer = curr_time
                    self.move_interval = random.randint(600, 850)
                    bias = pygame.Vector2(random.uniform(-150, 150), random.uniform(-150, 150))
                    self.target = self._target_away_from_player(player, 300, bias)
                    delta = self.target - pygame.Vector2(self.rect.center)
                    self.vel = safe_normalize(delta) * min(self.speed, delta.length())
            case 'move':
                delta = self.target - pygame.Vector2(self.rect.center)
                self.vel = safe_normalize(delta) * min(self.speed, delta.length())
                if curr_time > self.timer + self.move_interval or self.target == pygame.Vector2(self.rect.center):
                    self.state = 'shoot'
                    self.timer = curr_time
                    self.vel = pygame.Vector2(0, 0)
            case 'shoot':
                self.vel = pygame.Vector2(0, 0)
                if self.shoot_cnt == 0:
                    if curr_time > self.timer + self.shoot_interval:
                        self.target = pygame.Vector2(player.rect.center)
                        self.shoot_bullet(self.target, 9)
                        ortho = self._spread_offset(self.target, 0.87)
                        self.shoot_bullet(self.target + ortho, 7)
                        self.shoot_bullet(self.target - ortho, 7)
                        self.shoot_cnt += 1
                        self.timer = curr_time
                else:
                    if curr_time > self.timer + self.shoot_interval2:
                        self.shoot_bullet(self.target, 9)
                        ortho = self._spread_offset(self.target, 0.87)
                        self.shoot_bullet(self.target + ortho, 7)
                        self.shoot_bullet(self.target - ortho, 7)
                        self.shoot_cnt += 1
                        self.timer = curr_time
                if self.shoot_cnt >= 3:
                    self.shoot_cnt = 0
                    self.timer = curr_time
                    self.state = 'idle'
                    self.idle_interval = random.randint(300, 400)
    def dead(self):
        for _ in range(random.randint(2, 4)):
            vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
            ScorePoint(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel, None)
        vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
        Key(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel)
        self.kill()

class Turret(Enemy):
    def __init__(self, cell, pos: pygame.Vector2, countercw: bool):
        super().__init__(cell, pos, 0, 50, 5)
        self.offset = 4
        self.countercw = countercw
        self.shoot_interval = 50
        self.shoot_cnt = 0
    def take_damage(self, damage):
        self.health -= 1
        if self.health <= 0:
            self.health = 0
            self.dead()
    def render(self, surface):
        self.surface.fill("darkgrey")
        surface.blit(self.surface, self.rect)
        super().render(surface)
    def action(self):
        curr_time = pygame.time.get_ticks()
        if curr_time > self.timer + self.shoot_interval:
            self.timer = curr_time
            for i in range(4):
                self.shoot_bullet(pygame.Vector2(self.rect.center) + pygame.Vector2(math.cos(math.pi*self.offset/16+math.pi*i/2), math.sin(math.pi*self.offset/16+math.pi*i/2)), 10)
            self.shoot_cnt += 1
            if self.shoot_cnt >= 4:
                self.shoot_cnt = 0
                if self.countercw:
                    self.offset = (self.offset - 1) % 32
                else:
                    self.offset = (self.offset + 1) % 32
    def dead(self):
        for _ in range(random.randint(1, 2)):
            vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
            ScorePoint(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel, None)
        for _ in range(random.randint(0, 1)):
            vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
            HealthPotion(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel)
        self.kill()

class Elite(Enemy):
    def __init__(self, cell, pos: pygame.Vector2):
        super().__init__(cell, pos, 8, 40, 5)
        self.state = 'idle'
        self.idle_interval = random.randint(100, 300)
        self.move_interval = random.randint(200, 600)
        self.shoot_interval = 200
        self.shoot_cnt = 0
    def render(self, surface):
        self.surface.fill("crimson")
        surface.blit(self.surface, self.rect)
        super().render(surface)
    def action(self):
        player = glob_var["player"]
        curr_time = pygame.time.get_ticks()
        match self.state:
            case 'idle':
                self.vel = pygame.Vector2(0, 0)
                if curr_time > self.timer + self.idle_interval:
                    self.state = 'move'
                    self.timer = curr_time
                    self.move_interval = random.randint(200, 600)
                    bias = pygame.Vector2(random.uniform(-100, 100), random.uniform(-100, 100))
                    self.target = self._target_away_from_player(player, 250, bias)
                    delta = self.target - pygame.Vector2(self.rect.center)
                    self.vel = safe_normalize(delta) * min(self.speed, delta.length())
            case 'move':
                delta = self.target - pygame.Vector2(self.rect.center)
                self.vel = safe_normalize(delta) * min(self.speed, delta.length())
                if curr_time > self.timer + self.move_interval or self.target == pygame.Vector2(self.rect.center):
                    self.state = 'shoot'
                    self.timer = curr_time
                    self.vel = pygame.Vector2(0, 0)
            case 'shoot':
                self.vel = pygame.Vector2(0, 0)
                if self.shoot_cnt == 0:
                    if curr_time > self.timer + self.shoot_interval:
                        self.target = pygame.Vector2(player.rect.center)
                        self.shoot_bullet(self.target + pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10)), 10)
                        ortho = self._spread_offset(self.target, 0.6)
                        self.shoot_bullet(self.target + ortho + pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10)), 9)
                        self.shoot_bullet(self.target - ortho + pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10)), 9)
                        self.shoot_cnt += 1
                        self.timer = curr_time
                else:
                    if curr_time > self.timer + self.shoot_interval:
                        self.shoot_bullet(self.target + pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10)), 10)
                        ortho = self._spread_offset(self.target, 0.6)
                        self.shoot_bullet(self.target + ortho + pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10)), 9)
                        self.shoot_bullet(self.target - ortho + pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10)), 9)
                        self.shoot_cnt += 1
                        self.timer = curr_time
                if self.shoot_cnt >= 2:
                    self.shoot_cnt = 0
                    self.timer = curr_time
                    self.state = 'idle'
                    self.idle_interval = random.randint(100, 300)
    def dead(self):
        for _ in range(random.randint(1, 2)):
            vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
            ScorePoint(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel, 2)
        for _ in range(random.randint(2, 3)):
            vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
            ScorePoint(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel, None)
        for _ in range(random.randint(0, 1)):
            vel = pygame.Vector2(random.uniform(-10, 10), random.uniform(-10, 10))
            HealthPotion(self.location, pygame.Vector2(self.rect.center) - self.location.pos, vel)
        self.kill()
