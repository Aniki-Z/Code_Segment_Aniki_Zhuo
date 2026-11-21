# Refactor notes

## Starting point

The uploaded ZIP contained 15 Python source files, a short README, and compiled `__pycache__` output. Its main script initialized a Pygame window and ran event, update, and render calls forever. No external assets were referenced.

## What was changed

- Packaged the code as `src/randomazing/` for clean GitHub navigation and editable installation.
- Replaced `from ... import *` chains with explicit imports and removed unnecessary circular dependencies.
- Split the original ~500-line `unit.py` into `actors.py`, `player.py`, and `enemies.py`.
- Split constants, shared state, and geometry helpers into `settings.py`, `state.py`, and `geometry.py`.
- Added a `Game` controller, properly exit-able loop, restart-after-end behavior, frozen pause countdown, and constant-rate frame pacing.
- Fixed an inventory code duplication pattern; fixed chest item coordinate calculation.
- Fixed room wall teardown so the old walls disappear from the collision system as well as the draw group.
- Documented controls, dependencies, gameplay design, architecture, and a manual playtest checklist.
- Added headless tests and GitHub Actions CI configuration.
- Removed compiled cache files from the distributable ZIP.

## Validation performed at refactor time

- All source files compiled with Python `compileall`.
- Imported the entire application dependency graph using a lightweight pygame namespace stub because the actual package was not installed on the refactoring machine.
- Verified that no source module contains unresolved global names with an AST/symbol-table check.
- Simulated initialization, updates, rendering calls, exit unlocking, and restart across nine random seeds using a lightweight Pygame compatibility stand-in.
- Ran 11 bundled automated tests with the compatibility stand-in, and exercised enemy behavior over simulated frames in six seeded worlds. These are not substitutes for running the tests against actual Pygame.

**Not verified:** A real SDL/Pygame game window or full interactive playthrough. Dependency installation was unavailable in the refactoring environment. The included tests are designed to run on a developer machine or GitHub Actions once Pygame is installed. Treat the project as a runnable candidate pending that validation.

## Before making the repo public

1. Run `python -m pytest` and `python -m randomazing` locally.
2. Play through both success and failure outcomes, not just the title screen.
3. Add screenshots/GIFs recorded from the actual build; do not use mockups presented as screenshots.
4. Confirm ownership of every source file and choose a license if appropriate.
5. Update the README and portfolio description based on verified functionality.
