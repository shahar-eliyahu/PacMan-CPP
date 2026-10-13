#include <ctime>
#include <cstdlib>
#include "glut.h"
#include "game.h"
#include "constants.h"

static Game* g_game = nullptr;

void initGL() {
    glClearColor(0.12, 0.13, 0.17, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, MSZ, 0, MSZ + HUD_H, -1, 1); // board + info bar on top
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (g_game) g_game->draw();
    glutSwapBuffers();
}

// fixed-rate game loop: same speed on every computer
void timer(int) {
    if (g_game) g_game->update();
    glutPostRedisplay();
    glutTimerFunc(TICK_MS, timer, 0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 'r': case 'R':          // new random map
        delete g_game;
        g_game = new Game();
        break;
    case 'p': case 'P': case ' ':
        if (g_game) g_game->togglePause();
        break;
    case 27:                     // Esc
        delete g_game;
        std::exit(0);
    }
    glutPostRedisplay();
}

// keep the board square-cell when the window is resized
void reshape(int w, int h) {
    double aspect = (double)WIN_W / WIN_H;
    int vw = w, vh = (int)(w / aspect);
    if (vh > h) { vh = h; vw = (int)(h * aspect); }
    glViewport((w - vw) / 2, (h - vh) / 2, vw, vh);
}

int main(int argc, char** argv) {
    std::srand((unsigned)std::time(nullptr));
    g_game = new Game();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(400, 50);
    glutCreateWindow("Pacman: Coins & Escape");

    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TICK_MS, timer, 0);

    glutMainLoop();
    return 0;
}
