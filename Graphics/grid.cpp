#include "grid.h"
#include "render.h"
#include <cstdlib>

// the random seed is set once in main(), so every new Game gets a new map
Grid::Grid() { randomize(); }

void Grid::initFrameWalls() {
    for (int i = 0; i < MSZ; i++) {
        maze[0][i] = WALL;
        maze[MSZ - 1][i] = WALL;
        maze[i][0] = WALL;
        maze[i][MSZ - 1] = WALL;
    }
}

void Grid::randomize() {
    // initialize all cells to SPACE
    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++)
            maze[i][j] = SPACE;

    // frame walls
    initFrameWalls();

    // build random walls with different densities for even/odd rows
    for (int i = 1; i < MSZ - 1; i++) {
        for (int j = 1; j < MSZ - 1; j++) {
            int roll = std::rand() % 100;
            if (i % 2 == 0) { // even rows
                maze[i][j] = (roll < WALL_PROB_EVEN) ? WALL : SPACE;
            }
            else {          // odd rows
                maze[i][j] = (roll < WALL_PROB_ODD) ? WALL : SPACE;
            }
        }
    }

    // put coins in some of the empty spaces
    for (int i = 1; i < MSZ - 1; i++) {
        for (int j = 1; j < MSZ - 1; j++) {
            if (maze[i][j] == SPACE) {
                if ((std::rand() % 100) < COIN_DENSITY)
                    maze[i][j] = COIN;
            }
        }
    }
}

bool Grid::inBounds(int r, int c) const { return r >= 0 && r < MSZ && c >= 0 && c < MSZ; }
bool Grid::passable(int r, int c) const { return inBounds(r, c) && maze[r][c] != WALL; }
int  Grid::get(int r, int c) const { return maze[r][c]; }
void Grid::set(int r, int c, int v) { if (inBounds(r, c)) maze[r][c] = v; }

bool Grid::hasCoin(const Pos& p) const { return get(p.r, p.c) == COIN; }
bool Grid::collectCoin(const Pos& p) { if (hasCoin(p)) { set(p.r, p.c, SPACE); return true; } return false; }

bool Grid::anyCoinsLeft() const {
    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++)
            if (maze[i][j] == COIN) return true;
    return false;
}

int Grid::countCoins() const {
    int cnt = 0;
    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++)
            if (maze[i][j] == COIN) ++cnt;
    return cnt;
}

void Grid::removeCoinsOutside(const std::vector<char>& reachable) {
    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++)
            if (maze[i][j] == COIN && !reachable[i * MSZ + j])
                maze[i][j] = SPACE;
}

void Grid::draw() const {
    // floor: one quad for the whole board
    setColor(Palette::Floor);
    drawRect(0, 0, MSZ, MSZ);

    // walls
    setColor(Palette::Wall);
    glBegin(GL_QUADS);
    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++)
            if (maze[i][j] == WALL) {
                glVertex2d(j, i); glVertex2d(j + 1, i);
                glVertex2d(j + 1, i + 1); glVertex2d(j, i + 1);
            }
    glEnd();

    // coins: round gold coins with a rim, so they read as items and not as floor
    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++)
            if (maze[i][j] == COIN)
                drawCoin(j + 0.5, i + 0.5, 0.42);
}
