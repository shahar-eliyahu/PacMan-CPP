#pragma once
#include "types.h"
#include <vector>
class Grid;

class Entity {
public:
    explicit Entity(Pos p) : pos(p) {}
    virtual ~Entity() = default;
    const Pos& position() const { return pos; }
    void setPosition(const Pos& p) { pos = p; }
protected:
    Pos pos;
};

class Pacman : public Entity {
public:
    using Entity::Entity;
    void update(Grid& grid, const std::vector<Pos>& ghosts);
    void draw(const Grid& grid) const;

    bool isFleeing() const { return fleeing; }
private:
    Pos last = { -1, -1 }; // initially invalid
    bool fleeing = false;   // last decision: running from a ghost
    bool hasTarget = false; // last decision: heading to a coin
    Pos target = { -1, -1 };
    Pos threat = { -1, -1 }; // closest ghost found by the scan (while fleeing)
    Pos facing = { 0, 1 };   // last movement direction, shown as an arrow
};

class Ghost : public Entity {
public:
    using Entity::Entity;
    void update(const Grid& grid, const Pos& pac);
    void draw(const Grid& grid) const;
private:
    std::vector<Pos> path;  // current A* plan, drawn as a faint trail
};
