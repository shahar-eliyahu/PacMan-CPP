#pragma once

// ===== Board size =====
constexpr int MSZ = 100;
constexpr int HUD_H = 8;            // height of the info bar above the board (in cells)

// ===== Window =====
constexpr int CELL_PX = 7;          // pixels per cell
constexpr int WIN_W = MSZ * CELL_PX;
constexpr int WIN_H = (MSZ + HUD_H) * CELL_PX;

// ===== Cell types =====
constexpr int SPACE = 0;
constexpr int WALL = 1;
constexpr int COIN = 7;

// ===== Gameplay =====
constexpr int COIN_DENSITY = 12;    // probability (0-100) of placing a coin in a SPACE cell
constexpr int PAC_SCAN_DEPTH = 10;  // BFS search depth for Pacman
constexpr int MIN_SAFE_DIST = 2;    // Distance from ghost to consider a position unsafe
constexpr int NUM_GHOSTS = 3;
constexpr int GHOST_SPAWN_DIST = 25; // ghosts start at least this far (manhattan) from Pacman

// ===== Speeds (bigger = slower) =====
constexpr int TICK_MS = 30;          // game update interval in milliseconds (fixed, CPU independent)
constexpr int PAC_STEP_PERIOD = 2;   // pacman moves every 2 updates
constexpr int GHOST_STEP_PERIOD = 3; // ghosts move every 3 updates (a bit slower than Pacman)

// ===== Wall density controls (0-100) =====
constexpr int WALL_PROB_EVEN = 65; // Wall probability in even rows
constexpr int WALL_PROB_ODD = 15;  // Wall probability in odd rows

// ===== 4-dir movement =====
static const int DR[4] = { -1, 0, 1, 0 };
static const int DC[4] = { 0, 1, 0,-1 };
