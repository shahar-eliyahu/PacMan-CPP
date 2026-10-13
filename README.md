# Pacman AI: Coins & Escape

An autonomous chase game in C++ and OpenGL (GLUT). Nobody plays it: Pacman and three ghosts are AI agents on a randomly generated 100×100 maze. Pacman tries to collect every coin while staying away from the ghosts, and the ghosts hunt him down with A*.

## Demo

https://github.com/user-attachments/assets/1d49d3c9-f461-49e8-9ec7-fc2b108ecdb5

*Pacman (blue) collects coins until a ghost (red) comes within range, then switches to fleeing. The red dots show each ghost's planned A\* path.*

## Screenshots

<table>
  <tr>
    <th align="center">Collecting coins</th>
    <th align="center">Fleeing from a ghost</th>
  </tr>
  <tr>
    <td><img width="420" alt="Pacman heading to the nearest coin" src="https://github.com/user-attachments/assets/48dd1043-2de6-49e1-afb6-b6aef5cac0ea" /></td>
    <td><img width="420" alt="Pacman fleeing from the nearest ghost" src="https://github.com/user-attachments/assets/369a7673-92e7-41e1-a3d0-8cf378a78781" /></td>
  </tr>
</table>

## How the AI works

**Ghosts (red stars)** run A* toward Pacman on every move, using Manhattan distance as the heuristic, and take the first step of the path. The rest of each ghost's planned path is drawn as red dots.

**Pacman (blue disc, the arrow shows his direction)** decides every move in three layers:

1. **Threat scan:** a depth-limited BFS (10 steps) checks whether any ghost is reachable nearby.
2. **Coin search:** a BFS finds the nearest reachable coin and the first step toward it.
3. **Decision:**
   - No ghost in range: step toward the coin (a ring marks the target coin), unless that step brings him within 2 cells of a ghost.
   - Ghost in range: flee using a **maximin** rule, choosing the neighboring cell that maximizes the distance to the closest ghost. Ties go to the cell closer to the target coin, and he avoids stepping straight back to avoid oscillating. While fleeing he turns red, and a line points to the ghost he is running from.

**Map generation:** even rows get walls with 65% probability and odd rows with 15%, which creates horizontal corridors with random openings. Pacman starts in the largest connected area, coins he could never reach are removed (so every game is winnable), and ghosts spawn at least 25 cells away from him.

## Controls

| Key | Action |
|---|---|
| `P` / `Space` | Pause |
| `R` | New random map |
| `Esc` | Quit |

## Build and run

Requirements: Windows and Visual Studio 2022 (C++ desktop workload).

1. Open `Graphics.sln`.
2. Build and run (F5). The bundled freeglut and GLEW libraries are 32-bit, so the solution always builds the x86 version (selecting x64 maps to it as well). A post-build step copies the DLLs next to the executable.

## Project structure

| File | Purpose |
|---|---|
| `main.cpp` | GLUT window, fixed-rate game loop, keyboard |
| `game.cpp/.h` | Game rules, collisions, win/lose, info bar and end screen |
| `entities.cpp/.h` | Pacman and ghost behavior and drawing |
| `pathfinding.cpp/.h` | A*, BFS to nearest coin, depth-limited ghost scan, flood fill |
| `grid.cpp/.h` | Maze generation, coins, drawing the board |
| `render.h` | Color palette and shape-drawing helpers |
| `constants.h` | All tunable parameters: map size, wall density, speeds, scan depth |

Want to experiment? Everything is tunable in `constants.h`, for example `GHOST_STEP_PERIOD` (ghost speed), `PAC_SCAN_DEPTH` (how far Pacman looks for threats) or `COIN_DENSITY`.

## Background

Built as a course assignment (Assignment 3) on search algorithms and game AI.
