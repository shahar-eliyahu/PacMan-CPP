#pragma once
#include "grid.h"
#include <vector>

// full A* path from start to goal (excluding start); empty if unreachable or start == goal
std::vector<Pos> AStarPath(const Grid& grid, const Pos& start, const Pos& goal);
bool NearestCoinBFS(const Grid& grid, const Pos& start, Pos& coin, Pos& firstStep);
bool DepthLimitedGhostScan(const Grid& grid, const Pos& start,
    const std::vector<Pos>& ghosts, int D,
    bool& seen, Pos& closestGhost, Pos& firstTowardGhost);
// marks every cell reachable from start (mask index = r * MSZ + c)
std::vector<char> ReachableFrom(const Grid& grid, const Pos& start);
