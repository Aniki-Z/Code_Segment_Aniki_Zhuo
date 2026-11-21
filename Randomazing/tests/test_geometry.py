"""Fast geometry tests independent of active game state."""

import pygame

from randomazing.geometry import colliderect, collidesegmentrect, get_direction, safe_normalize


def test_cardinal_direction_to_target():
    origin = pygame.Vector2(10, 10)
    assert get_direction(origin, pygame.Vector2(11, 10)) == "r"
    assert get_direction(origin, pygame.Vector2(9, 10)) == "l"
    assert get_direction(origin, pygame.Vector2(10, 9)) == "u"
    assert get_direction(origin, pygame.Vector2(10, 11)) == "d"


def test_rect_overlap_and_separation():
    first = pygame.Rect(0, 0, 20, 20)
    assert colliderect(first, pygame.Rect(15, 15, 20, 20))
    assert not colliderect(first, pygame.Rect(20, 0, 20, 20))


def test_segment_intersects_rectangle():
    rect = pygame.Rect(10, 10, 20, 20)
    assert collidesegmentrect(pygame.Vector2(0, 20), pygame.Vector2(40, 20), rect)
    assert not collidesegmentrect(pygame.Vector2(0, 0), pygame.Vector2(5, 5), rect)


def test_zero_distance_safe_normalization():
    assert safe_normalize(pygame.Vector2(0, 0)) == pygame.Vector2(0, 0)
    assert safe_normalize(pygame.Vector2(0, 0), pygame.Vector2(1, 0)) == pygame.Vector2(1, 0)


def test_unit_vector_is_approximately_normalized():
    vector = safe_normalize(pygame.Vector2(3, 4))
    assert abs(vector.length() - 1.0) < 1e-8
