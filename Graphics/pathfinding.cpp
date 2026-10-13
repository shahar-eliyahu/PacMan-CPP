#include "pathfinding.h"
#include <queue>
#include <unordered_map>
#include <climits>
#include <algorithm>

// A* algorithm: full path from start toward goal (start itself not included)
std::vector<Pos> AStarPath(const Grid& grid, const Pos& start, const Pos& goal) {
    std::vector<Pos> path;
    if (start == goal) return path;
    struct Node { Pos p; int g; int f; };
    struct Cmp { bool operator()(const Node& a, const Node& b)const { return a.f > b.f; } };
    auto key = [](const Pos& p) { return p.r * MSZ + p.c; };

    std::priority_queue<Node, std::vector<Node>, Cmp> open;
    std::unordered_map<int, int> gbest;
    std::unordered_map<int, Pos> parent;

    open.push({ start,0,manhattan(start,goal) });
    gbest[key(start)] = 0;

    while (!open.empty()) {
        Node cur = open.top(); open.pop();
        if (cur.p == goal) {
            for (Pos p = cur.p; p != start; p = parent[key(p)])
                path.push_back(p);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int k = 0; k < 4; k++) {
            Pos nx{ cur.p.r + DR[k], cur.p.c + DC[k] };
            if (!grid.passable(nx.r, nx.c)) continue;
            int ng = cur.g + 1, kk = key(nx);
            auto it = gbest.find(kk);
            if (it == gbest.end() || ng < it->second) {
                gbest[kk] = ng; parent[kk] = cur.p;
                open.push({ nx,ng, ng + manhattan(nx,goal) });
            }
        }
    }
    return path; // unreachable
}

// flood fill from start
std::vector<char> ReachableFrom(const Grid& grid, const Pos& start) {
    std::vector<char> reach(MSZ * MSZ, 0);
    if (!grid.passable(start.r, start.c)) return reach;

    std::queue<Pos> q; q.push(start); reach[start.r * MSZ + start.c] = 1;
    while (!q.empty()) {
        Pos u = q.front(); q.pop();
        for (int k = 0; k < 4; k++) {
            Pos v{ u.r + DR[k], u.c + DC[k] };
            if (!grid.passable(v.r, v.c) || reach[v.r * MSZ + v.c]) continue;
            reach[v.r * MSZ + v.c] = 1; q.push(v);
        }
    }
    return reach;
}

// BFS to find nearest coin from s; returns coin position and first step toward it
bool NearestCoinBFS(const Grid& grid, const Pos& s, Pos& coin, Pos& firstStep) {
    static bool vis[MSZ][MSZ];
    static Pos  par[MSZ][MSZ];
    for (int i = 0; i < MSZ; i++) 
        for (int j = 0; j < MSZ; j++)
             { vis[i][j] = false; par[i][j] = { -1,-1 }; }

    std::queue<Pos> q; q.push(s); vis[s.r][s.c] = true;
    while (!q.empty()) {
        Pos u = q.front(); q.pop();
        if (grid.hasCoin(u)) {
            coin = u;
            Pos p = u;
            while (p != s) 
            { Pos pr = par[p.r][p.c]; if (pr == s) { firstStep = p; return true; } p = pr; }
            firstStep = s; return true;
        }
        for (int k = 0; k < 4; k++) {
            Pos v{ u.r + DR[k], u.c + DC[k] };
            if (!grid.passable(v.r, v.c) || vis[v.r][v.c]) continue;
            vis[v.r][v.c] = true; par[v.r][v.c] = u; q.push(v);
        }
    }
    return false;
}

// Depth-limited BFS to find nearest ghost from s within depth D
bool DepthLimitedGhostScan(const Grid& grid, const Pos& s,
    const std::vector<Pos>& ghosts, int D,
    bool& seen, Pos& closestGhost, Pos& firstToward)
{
    struct Seen { Pos p; int d; Pos first; };
    static bool vis[MSZ][MSZ];
    for (int i = 0; i < MSZ; i++) for (int j = 0; j < MSZ; j++) vis[i][j] = false;

    auto isGhost = [&](const Pos& t) {
        for (const auto& g : ghosts) if (g == t) return true;
        return false;
        };

    std::queue<Seen> q; q.push({ s,0,s }); vis[s.r][s.c] = true;
    seen = false; int best = INT_MAX; firstToward = s;

    while (!q.empty()) {
        Seen cur = q.front(); q.pop();

        if (isGhost(cur.p)) {
            int d = manhattan(s, cur.p);
            if (d < best) { best = d; closestGhost = cur.p; seen = true; firstToward = cur.first; }
        }

        if (cur.d == D) continue;

        for (int k = 0; k < 4; k++) {
            Pos nx{ cur.p.r + DR[k], cur.p.c + DC[k] };
            if (!grid.passable(nx.r, nx.c) || vis[nx.r][nx.c]) continue;
            vis[nx.r][nx.c] = true;
            Pos first = (cur.p == s) ? nx : cur.first;
            q.push({ nx, cur.d + 1, first });
        }
    }
    return seen;
}

