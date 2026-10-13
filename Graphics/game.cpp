#include "game.h"
#include "pathfinding.h"
#include "render.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <vector>

// open cell closest to the board center that belongs to the largest connected area.
// (the exact center can be a wall, and the nearest open cell can be a tiny closed pocket)
static Pos findStartCell(const Grid& grid) {
    std::vector<char> done(MSZ * MSZ, 0), bestArea;
    int bestSize = 0;
    for (int r = 0; r < MSZ; r++)
        for (int c = 0; c < MSZ; c++) {
            if (!grid.passable(r, c) || done[r * MSZ + c]) continue;
            std::vector<char> area = ReachableFrom(grid, Pos{ r, c });
            int size = 0;
            for (int i = 0; i < MSZ * MSZ; i++)
                if (area[i]) { done[i] = 1; ++size; }
            if (size > bestSize) { bestSize = size; bestArea = area; }
        }

    const Pos center{ MSZ / 2, MSZ / 2 };
    Pos best = center;
    int bestDist = MSZ * 2;
    for (int r = 0; r < MSZ; r++)
        for (int c = 0; c < MSZ; c++)
            if (bestSize > 0 && bestArea[r * MSZ + c] && manhattan(center, Pos{ r, c }) < bestDist) {
                bestDist = manhattan(center, Pos{ r, c });
                best = Pos{ r, c };
            }
    return best;
}

Game::Game() : grid(), pac(findStartCell(grid)) {
    // only cells Pacman can actually reach matter: coins elsewhere could never be
    // collected (game could never be won), and ghosts there could never chase him
    std::vector<char> reach = ReachableFrom(grid, pac.position());
    grid.removeCoinsOutside(reach);

    std::vector<Pos> spawnCells, farCells;
    for (int r = 0; r < MSZ; r++)
        for (int c = 0; c < MSZ; c++)
            if (reach[r * MSZ + c] && Pos{ r, c } != pac.position()) {
                spawnCells.push_back(Pos{ r, c });
                if (manhattan(Pos{ r, c }, pac.position()) >= GHOST_SPAWN_DIST)
                    farCells.push_back(Pos{ r, c });
            }
    const std::vector<Pos>& pool = farCells.empty() ? spawnCells : farCells;

    // random ghosts in Pacman's area, not too close to him
    ghosts.reserve(NUM_GHOSTS);
    for (int k = 0; k < NUM_GHOSTS && !pool.empty(); k++)
        ghosts.emplace_back(pool[std::rand() % pool.size()]);

    // not starting on a coin
    grid.collectCoin(pac.position());

    coins_collected = 0;
    total_coins = grid.countCoins();
    won = (total_coins == 0);
    if (won) running = false;
}

// check if pacman meets any ghost
bool Game::checkCollision() const {
    for (const auto& g : ghosts) if (g.position() == pac.position()) return true;
    return false;
}

void Game::update() {
    if (!running || paused) return;

    ++steps;

    // ghosts
    if (steps % GHOST_STEP_PERIOD == 0) {
        for (auto& g : ghosts) g.update(grid, pac.position());

        // a ghost that stepped onto Pacman catches him before he gets to move away
        if (checkCollision()) {
            won = false; running = false;
            std::cout << "Game Over\n";
            return;
        }
    }

    // pacman
    if (steps % PAC_STEP_PERIOD == 0) {
        std::vector<Pos> gs; gs.reserve(ghosts.size());
        for (auto& g : ghosts) gs.push_back(g.position());
        pac.update(grid, gs);

        // pacman walked into a ghost
        if (checkCollision()) {
            won = false; running = false;
            std::cout << "Game Over\n";
            return;
        }

        // collect coin
        if (grid.collectCoin(pac.position())) {
            ++coins_collected;
            if (total_coins > 0) --total_coins;

            // check win
            if (total_coins <= 0 || !grid.anyCoinsLeft()) {
                won = true;
                running = false;
                std::cout << "You win! All coins collected\n";
            }
        }
    }
}

// ===== text helpers =====

// bitmap font (fixed pixel size); x, y in board units
static void drawTextBitmap(double x, double y, const char* s, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2d(x, y);
    for (const char* p = s; *p; ++p)
        glutBitmapCharacter(font, *p);
}

static double bitmapWidth(const char* s, void* font = GLUT_BITMAP_HELVETICA_18) {
    return glutBitmapLength(font, (const unsigned char*)s) / (double)CELL_PX;
}

// stroke font (scalable), horizontally centered on cx
static void drawTextStrokeCentered(double cx, double y, const char* s, double scale) {
    double w = glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*)s) * scale;
    // draw a few times with small offsets so the thin stroke font looks bold
    const double o = 0.12;
    const double offs[5][2] = { {0,0}, {o,0}, {0,o}, {o,o}, {-o,0} };
    for (const auto& d : offs) {
        glPushMatrix();
        glTranslated(cx - w / 2 + d[0], y + d[1], 0);
        glScaled(scale, scale, 1.0);
        for (const char* p = s; *p; ++p)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
        glPopMatrix();
    }
}

// ===== drawing =====

void Game::drawHud() const {
    const double top = MSZ + HUD_H;
    char buf[96];

    setColor(Palette::HudBg);
    drawRect(0, MSZ, MSZ, HUD_H);

    // coins counter + progress bar
    int total = coins_collected + total_coins;
    std::snprintf(buf, sizeof(buf), "Coins  %d / %d", coins_collected, total);
    setColor(Palette::HudText);
    drawTextBitmap(2, top - 3.4, buf);

    const double barX = 2, barY = MSZ + 1.4, barW = 42, barH = 1.3;
    setColor(Palette::Wall);
    drawRect(barX, barY, barW, barH);
    setColor(Palette::Coin);
    drawRect(barX, barY, total > 0 ? barW * coins_collected / total : barW, barH);

    // pacman mode
    const bool flee = pac.isFleeing();
    setColor(flee ? Palette::Ghost : Palette::Player);
    drawDisc(48, top - 2.9, 1.0);
    setColor(Palette::HudText);
    drawTextBitmap(50.5, top - 3.4, flee ? "Fleeing" : "Collecting");
    setColor(Palette::HudDim);
    drawTextBitmap(50.5, MSZ + 1.4, "Pacman mode", GLUT_BITMAP_HELVETICA_12);

    // controls
    const char* keys = "P  pause    R  new map    Esc  quit";
    setColor(Palette::HudDim);
    drawTextBitmap(MSZ - 2 - bitmapWidth(keys, GLUT_BITMAP_HELVETICA_12), MSZ + 1.4, keys, GLUT_BITMAP_HELVETICA_12);

    // legend: the same shapes as on the board
    const double ly = top - 2.9, ty = top - 3.4;
    void* f = GLUT_BITMAP_HELVETICA_12;
    glColor3d(1, 1, 1);
    drawDisc(70, ly, 1.25);
    setColor(Palette::Player);
    drawDisc(70, ly, 1.0);
    setColor(Palette::HudText);
    drawTextBitmap(72, ty, "Pacman", f);

    setColor(Palette::Ghost);
    drawStar(82, ly, 1.35, 0.75);
    setColor(Palette::HudText);
    drawTextBitmap(84, ty, "Ghost", f);

    drawCoin(93, ly, 0.6);
    setColor(Palette::HudText);
    drawTextBitmap(94.5, ty, "Coin", f);
}

void Game::drawEndScreen() const {
    enableBlend();
    glColor4d(0, 0, 0, 0.55);
    drawRect(0, 0, MSZ, MSZ);
    glDisable(GL_BLEND);

    setColor(Palette::HudBg);
    drawRect(MSZ / 2.0 - 36, MSZ / 2.0 - 12, 72, 24);

    glLineWidth(3.0f);
    setColor(won ? Palette::Coin : Palette::Ghost);
    drawTextStrokeCentered(MSZ / 2.0, MSZ / 2.0 + 1, won ? "YOU WIN!" : "GAME OVER", 0.075);
    glLineWidth(1.0f);

    char buf[96];
    std::snprintf(buf, sizeof(buf), "Collected %d of %d coins", coins_collected, coins_collected + total_coins);
    setColor(Palette::HudText);
    drawTextBitmap(MSZ / 2.0 - bitmapWidth(buf) / 2, MSZ / 2.0 - 4.5, buf);

    const char* hint = "Press R for a new map";
    setColor(Palette::HudDim);
    drawTextBitmap(MSZ / 2.0 - bitmapWidth(hint, GLUT_BITMAP_HELVETICA_12) / 2, MSZ / 2.0 - 8.5, hint, GLUT_BITMAP_HELVETICA_12);
}

void Game::draw() {
    grid.draw();
    for (auto& g : ghosts) g.draw(grid);
    pac.draw(grid);

    drawHud();

    if (!running) {
        drawEndScreen();
    }
    else if (paused) {
        setColor(Palette::HudBg);
        drawRect(MSZ / 2.0 - 12, MSZ / 2.0 - 4, 24, 8);
        glLineWidth(2.0f);
        setColor(Palette::HudText);
        drawTextStrokeCentered(MSZ / 2.0, MSZ / 2.0 - 2, "PAUSED", 0.045);
        glLineWidth(1.0f);
    }
}
