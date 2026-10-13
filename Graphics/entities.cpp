#include "constants.h"
#include "types.h"
#include "grid.h"
#include "entities.h"
#include "pathfinding.h"
#include "render.h"
#include <climits>
#include <vector>

// min distance from p to any ghost
static int minDistToGhosts(const Pos& p, const std::vector<Pos>& ghosts) {
    int m = INT_MAX;
    for (const auto& g : ghosts) {
        int d = manhattan(p, g);
        if (d < m) m = d;
    }
    return m;
}

// choose a step that maximizes the minimum distance to any ghost
static Pos chooseMaximinStep(const Grid& grid, const Pos& cur, const Pos& last,
    const std::vector<Pos>& ghosts,
    bool hasCoinTarget, const Pos& coinTarget)
{
    Pos best{ -1,-1 };
    int bestSafe = -1;
    int bestCoinD = hasCoinTarget ? manhattan(cur, coinTarget) : INT_MAX;

	// first try excluding last
    for (int k = 0; k < 4; k++) {
        Pos nx{ cur.r + DR[k], cur.c + DC[k] };
        if (!grid.passable(nx.r, nx.c)) continue;
        if (nx == last) continue;

        int s = minDistToGhosts(nx, ghosts);
        int coinD = hasCoinTarget ? manhattan(nx, coinTarget) : INT_MAX;

        if (s > bestSafe || (s == bestSafe && coinD < bestCoinD)) {
            best = nx; bestSafe = s; bestCoinD = coinD;
        }
    }
	// try including last if no other option
    if (best.r == -1) {
        for (int k = 0; k < 4; k++) {
            Pos nx{ cur.r + DR[k], cur.c + DC[k] };
            if (!grid.passable(nx.r, nx.c)) continue;
            int s = minDistToGhosts(nx, ghosts);
            int coinD = hasCoinTarget ? manhattan(nx, coinTarget) : INT_MAX;
            if (s > bestSafe || (s == bestSafe && coinD < bestCoinD)) {
                best = nx; bestSafe = s; bestCoinD = coinD;
            }
        }
    }
    return (best.r == -1 ? cur : best);
}

void Pacman::update(Grid& grid, const std::vector<Pos>& ghosts) {
    Pos cur = position();

	// info on nearby ghosts
    bool seen = false;
    Pos closest{ -1,-1 }, dirToGhost{};
    DepthLimitedGhostScan(grid, cur, ghosts, PAC_SCAN_DEPTH, seen, closest, dirToGhost);

	// info on nearest coin
    Pos coinPos{ -1,-1 }, coinStep{};
    bool hasCoinPath = NearestCoinBFS(grid, cur, coinPos, coinStep);

    Pos next = cur;
    fleeing = seen;
    hasTarget = hasCoinPath;
    target = coinPos;
    threat = seen ? closest : Pos{ -1, -1 };

    if (seen) {
        // found ghost(s), run away
        next = chooseMaximinStep(grid, cur, last, ghosts, hasCoinPath, coinPos);
    }
    else if (hasCoinPath) {
        // no ghosts nearby, found coin
        int coinSafe = minDistToGhosts(coinStep, ghosts);
        if (coinSafe >= MIN_SAFE_DIST) next = coinStep;
        else {
            fleeing = true;
            int bestD = INT_MAX;
            for (const auto& g : ghosts)
                if (manhattan(cur, g) < bestD) { bestD = manhattan(cur, g); threat = g; }
            next = chooseMaximinStep(grid, cur, last, ghosts, hasCoinPath, coinPos);
        }
    }
    else {
        // if no ghosts, no coins - just run maximin
        next = chooseMaximinStep(grid, cur, last, ghosts, false, Pos{ -1,-1 });
    }

    if (next != cur) { last = cur; facing = Pos{ next.r - cur.r, next.c - cur.c }; setPosition(next); }
}

void Pacman::draw(const Grid&) const {
    double x = pos.c + 0.5, y = pos.r + 0.5;
    enableBlend();

    // target coin: ring around the coin Pacman is heading to
    if (hasTarget && !fleeing) {
        glLineWidth(2.0f);
        setColor(Palette::Player, 0.9);
        drawRing(target.c + 0.5, target.r + 0.5, 1.0);
        glLineWidth(1.0f);
    }

    // fleeing: red halo + a line to the ghost he is running from
    if (fleeing) {
        setColor(Palette::Ghost, 0.22);
        drawDisc(x, y, 2.8);
        if (threat.r >= 0) {
            glLineWidth(2.0f);
            setColor(Palette::Ghost, 0.8);
            glBegin(GL_LINES);
            glVertex2d(x, y); glVertex2d(threat.c + 0.5, threat.r + 0.5);
            glEnd();
            glLineWidth(1.0f);
        }
    }
    glDisable(GL_BLEND);

    // body: white ring + blue disc
    glColor3d(1, 1, 1);
    drawDisc(x, y, 1.5);
    setColor(Palette::Player);
    drawDisc(x, y, 1.2);

    // white arrow showing the movement direction
    double fx = facing.c, fy = facing.r;   // unit vector (4-dir movement)
    double px = -fy, py = fx;              // perpendicular
    glColor3d(1, 1, 1);
    glBegin(GL_TRIANGLES);
    glVertex2d(x + fx * 0.9, y + fy * 0.9);
    glVertex2d(x - fx * 0.4 + px * 0.6, y - fy * 0.4 + py * 0.6);
    glVertex2d(x - fx * 0.4 - px * 0.6, y - fy * 0.4 - py * 0.6);
    glEnd();
}

void Ghost::update(const Grid& grid, const Pos& pac) {
    path = AStarPath(grid, position(), pac);
    if (!path.empty())
        setPosition(path.front());
}

void Ghost::draw(const Grid&) const {
    // remaining A* plan as faint red dots (skip the cell we are standing on)
    enableBlend();
    setColor(Palette::Ghost, 0.45);
    for (size_t i = 1; i < path.size(); i++)
        drawDisc(path[i].c + 0.5, path[i].r + 0.5, 0.24, 8);
    glDisable(GL_BLEND);

    // body: dark outline + red spiky star + white center
    double x = pos.c + 0.5, y = pos.r + 0.5;
    setColor(Palette::Wall);
    drawStar(x, y, 1.75, 1.0);
    setColor(Palette::Ghost);
    drawStar(x, y, 1.45, 0.8);
    glColor3d(1, 1, 1);
    drawDisc(x, y, 0.32, 10);
}
