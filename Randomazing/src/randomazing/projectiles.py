"""Projectiles and hit callbacks."""

import pygame

class Projectile(pygame.sprite.Sprite):
    all_projectiles = pygame.sprite.Group()
    def __init__(self, owner, damage, pos: pygame.Vector2, vel: pygame.Vector2, cm):
        super().__init__()
        self.surface = pygame.Surface([10, 10])
        self.surface.fill("yellow")
        self.rect = pygame.Rect((0, 0), [10, 10])
        self.rect.center = pos
        self.damage = damage
        self.vel = vel
        self.owner = owner
        self.func = self.hit
        Projectile.all_projectiles.add(self)
        cm.add_dynamic(self)
    def render(self, surface: pygame.Surface):
        surface.blit(self.surface, self.rect)
    def hit(self, unit):
        # Team checks use explicit factions rather than fragile class inheritance.
        if self.owner.faction != unit.faction:
            unit.take_damage(self.damage)
            self.kill()
