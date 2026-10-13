#pragma once
#include <cmath>

struct Pos {
    int r = 0, c = 0;
    bool operator==(const Pos& o) const { return r == o.r && c == o.c; }
    bool operator!=(const Pos& o) const { return !(*this == o); }
};

inline int manhattan(const Pos& a, const Pos& b) {
    return std::abs(a.r - b.r) + std::abs(a.c - b.c);
}
