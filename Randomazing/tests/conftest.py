"""Initialize SDL in no-window mode for the game smoke tests."""

import os
os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
os.environ.setdefault("SDL_AUDIODRIVER", "dummy")

import pygame
import pytest

from randomazing.application import Game


@pytest.fixture
def game():
    instance = Game(seed=42)
    instance.start()
    yield instance
    pygame.quit()
