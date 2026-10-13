#pragma once
// Small drawing helpers shared by the grid, the entities and the HUD.
#include "glut.h"
#include <cmath>

struct Color { double r, g, b; };

// ===== Palette =====
namespace Palette {
    const Color Floor   = { 0.93, 0.94, 0.96 };
    const Color Wall    = { 0.18, 0.20, 0.25 };
    const Color Coin    = { 1.00, 0.80, 0.15 };
    const Color CoinRim = { 0.70, 0.48, 0.02 };
    const Color Player  = { 0.16, 0.48, 0.94 };   // blue runner
    const Color Ghost   = { 0.88, 0.11, 0.16 };   // red chasers
    const Color HudBg   = { 0.12, 0.13, 0.17 };
    const Color HudText = { 0.95, 0.95, 0.95 };
    const Color HudDim  = { 0.62, 0.65, 0.72 };
}

inline void setColor(const Color& c, double a = 1.0) { glColor4d(c.r, c.g, c.b, a); }

// filled circle centered at (x, y)
inline void drawDisc(double x, double y, double radius, int segments = 20) {
    const double PI = 3.14159265358979;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x, y);
    for (int i = 0; i <= segments; i++) {
        double a = 2.0 * PI * i / segments;
        glVertex2d(x + radius * std::cos(a), y + radius * std::sin(a));
    }
    glEnd();
}

// circle outline centered at (x, y)
inline void drawRing(double x, double y, double radius, int segments = 24) {
    const double PI = 3.14159265358979;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        double a = 2.0 * PI * i / segments;
        glVertex2d(x + radius * std::cos(a), y + radius * std::sin(a));
    }
    glEnd();
}

// filled diamond (rotated square) centered at (x, y)
inline void drawDiamond(double x, double y, double radius) {
    glBegin(GL_POLYGON);
    glVertex2d(x, y + radius);
    glVertex2d(x + radius, y);
    glVertex2d(x, y - radius);
    glVertex2d(x - radius, y);
    glEnd();
}

// filled star with n points centered at (x, y)
inline void drawStar(double x, double y, double outer, double inner, int points = 8) {
    const double PI = 3.14159265358979;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x, y);
    for (int i = 0; i <= 2 * points; i++) {
        double a = PI * i / points + PI / 2;
        double rad = (i % 2 == 0) ? outer : inner;
        glVertex2d(x + rad * std::cos(a), y + rad * std::sin(a));
    }
    glEnd();
}

// gold coin: dark rim + bright face
inline void drawCoin(double x, double y, double radius) {
    setColor(Palette::CoinRim);
    drawDisc(x, y, radius, 12);
    setColor(Palette::Coin);
    drawDisc(x, y, radius * 0.7, 12);
}

inline void drawRect(double x, double y, double w, double h) {
    glBegin(GL_POLYGON);
    glVertex2d(x, y); glVertex2d(x + w, y);
    glVertex2d(x + w, y + h); glVertex2d(x, y + h);
    glEnd();
}

inline void enableBlend() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
