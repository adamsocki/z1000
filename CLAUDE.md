## Program: Vulkan-based Game Engine

### Coding Style Guidelines
- Prefer C-style programming over C++ features whenever possible.
- Avoid object-oriented programming; do not use classes.
- Use `struct` for data organization.
- Write simple, clear, and straightforward code.
- Place all `#include` directives sequentially at the top of `zayn.cpp` or `zayn.h` only.
    - New headers should not contain their own function declarations.
- Only compile `main.cpp` in CMake; do not add other source files to the build.

### Game Design Goals
- Need to define the core concept and objectives for the game.
- Suggestions for improvement:
    - Brainstorm possible game genres (e.g., puzzle, platformer, shooter).
    - Consider a minimal prototype to test engine features.
    - Focus on a small, achievable gameplay loop to start.
    - Gather feedback and iterate on the game idea as the engine develops.