#pragma once
#include "grid.h"
#include "entities.h"
#include <vector>

class Game {
public:
    Game();
    void update();
    void draw();

    bool isRunning() const { return running; }
    bool isPaused() const { return paused; }
    void togglePause() { if (running) paused = !paused; }

    // counters for collected and total coins
    int coinsCollected() const { return coins_collected; }
    int totalCoins()     const { return total_coins; }

    // win state - all coins collected
    bool isWon() const { return won; }

private:
    Grid grid;
    Pacman pac;
    std::vector<Ghost> ghosts;

    bool running = true;
    bool paused = false;
    bool won = false;
    int  coins_collected = 0;   // counting how many collected
    int  total_coins = 0;       // coins still on the board
    int  steps = 0;             // update counter (per game, resets on new map)

    bool checkCollision() const; // check if pacman meets any ghost
    void drawHud() const;
    void drawEndScreen() const;
};
