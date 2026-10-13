#pragma once
#include "constants.h"
#include "types.h"
#include <vector>

class Grid {
public:
    Grid();
    void randomize(); // creates walls + coins
    void draw() const;

    bool inBounds(int r, int c) const;
    bool passable(int r, int c) const;
    int  get(int r, int c) const;
    void set(int r, int c, int v);

    bool hasCoin(const Pos& p) const;
    bool collectCoin(const Pos& p);
    bool anyCoinsLeft() const;
    int  countCoins() const;

    // removes coins in cells that are not marked reachable (mask index = r * MSZ + c)
    void removeCoinsOutside(const std::vector<char>& reachable);

private:
    int maze[MSZ][MSZ] = { 0 };
    void initFrameWalls();
};
