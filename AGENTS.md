# Project guidance

## Architecture

- This is a small C11 snake game. Favor straightforward game-specific data
  structures and functions over a generic framework or external libraries.
- The architecture borrows ECS ideas: keep data in plain structs and organize
  behavior into focused systems called in a clear order. `Game` owns the snake,
  food, score, timers, and game state; `Snake` owns its directions and linked
  list of `GridCell` segments.
- `src/main.c` owns the `App` lifecycle, Fenster window and input, framebuffer,
  frame pacing, and error reporting. Each frame polls events, updates the game,
  and draws it. The application seeds the C random-number generator at startup.
- `src/game.c` coordinates game-state, snake-input, and movement systems in that
  order. Movement includes collision, eating, growth, scoring, and food
  respawning because their order matters. Drawing is coordinated by `game_draw`.
- `src/snake.c` and `src/food.c` implement their respective data operations and
  drawing. `src/draw.c` and `src/font.c` provide framebuffer drawing and text.
  Keep implementation-only helpers `static` and expose shared interfaces in
  headers.
- Preserve ownership and failure handling: snake segments are dynamically
  allocated and freed; failed growth or initialization propagates an error.
  Restart initializes a replacement game before destroying the old one.
- The game uses grid coordinates, wraps at the edges, rejects direct reversal,
  and considers the current tail occupied during collision checks. Food is
  placed on a free cell; an inactive food item indicates a full board. Preserve
  pause and restart timing unless behavior changes are requested.

## Tooling and verification

- Use the existing Makefile: `make` builds `build/pixel-game`, `make run`
  launches it, and `make clean` removes build output. Override the compiler with
  `make CC=gcc-15` or another suitable compiler if needed. The default flags are
  C11, `-Wall`, `-Wextra`, `-g`, and `-O0`.
- The Makefile discovers `.c` and `.h` files in `src` automatically. Build and
  runtime requirements for macOS, Linux, and Windows are documented in
  `README.md`; VS Code tasks and debugger settings are in `.vscode/`.
- Fenster is vendored in `vendor/fenster` and supplies the window, framebuffer,
  input, and timing. Use the existing dependency rather than introducing a new
  library for these tasks. Its header contains the implementation by default, so
  include it as an implementation in only one translation unit.
- Run `make` after code changes. There is no automated test target currently;
  when relevant, manually check movement, wrapping, eating and growth,
  collision, pause/resume, restart, and rendering. Report checks that could not
  be run.
