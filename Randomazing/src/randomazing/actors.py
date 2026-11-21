"""Health, damage, and movement state shared by player and enemies."""

import pygame

from .settings import HB_HEIGHT, HB_WIDTH, UNIT_HEIGHT, UNIT_WIDTH

class Unit(pygame.sprite.Sprite):
    faction = "neutral"

    def __init__(self, cell, pos: pygame.Vector2, speed, health, attack):
        super().__init__()
        self.surface = pygame.Surface([UNIT_WIDTH, UNIT_HEIGHT])
        self.surface.set_colorkey("black")
        self.surface.fill("black")
        self.hb_surface = pygame.Surface([HB_WIDTH, HB_HEIGHT])
        self.hb_surface.set_colorkey("black")
        self.surface.fill("black")
        self.rect = pygame.Rect((0, 0), [UNIT_WIDTH, UNIT_HEIGHT])
        self.rect.center = cell.pos + pos
        self.speed = speed
        self.max_health = health
        self.health = health
        self.attack = attack
        self.vel = pygame.Vector2(0, 0)
        self.location = cell
    def render(self, surface):
        if self.health == self.max_health:
            return
        dest = pygame.Rect(self.rect.topleft, [HB_WIDTH, HB_HEIGHT])
        dest.center = pygame.Vector2(self.rect.center) - pygame.Vector2(0, UNIT_HEIGHT / 2 + 10)
        self.hb_surface.fill("black")
        pygame.draw.rect(self.hb_surface, "red", pygame.Rect((0, 0), [HB_WIDTH * self.health / self.max_health, HB_HEIGHT]))
        surface.blit(self.hb_surface, dest)
    def take_damage(self, damage):
        self.health -= damage
        if self.health <= 0:
            self.health = 0
            self.dead()
    def action(self):
        pass
    def dead(self):
        pass
