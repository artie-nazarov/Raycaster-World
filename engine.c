#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "map_loader.c"

/// Window Properties
#define WIDTH 1024
#define HEIGHT 800

/// Player Properties
#define STEP_SIZE 5
#define PLAYER_HEIGHT 6
#define CROUCH_HEIGHT 3

/// Map Properties
#define MAP_FILEPATH "maps/map1.map"
#define WALL_BLOCK_SIZE 64

float px, py; // player position global variables

/// Draw a player point
void drawPlayer() {
    glColor3f(1, 1, 0);     // yellow color
    glPointSize(10);        // set the point size
    glBegin(GL_POINTS);     // specify geometricx object as POINTS (multiple objects)
        glVertex2f(px, py); // define a single point at the player coordinates
    glEnd();
}

/// Draw Map
void drawMap2D() {
    // Create a new Map object
    Map *m = initMap(MAP_FILEPATH);
    if (m == NULL) {
        return;
    }
    // Iterate over the map
    // If a wall is seen, draw a cube with dim3(WALL_BLOCK_SIZE,..,..)
    int w = m->width, h = m->height;
    int i0, j0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            // Check if there is a wall at coordinate (i, j)
            if (m->map[getIndex(m, i, j)] == '1') {
                glColor3f(1, 1, 1);
            }
            else {
                glColor3f(0, 0, 0);
            }
            i0 = i * WALL_BLOCK_SIZE, j0 = j * WALL_BLOCK_SIZE;
            glBegin(GL_QUADS);
                glVertex2i(j0 + 1, i0 + 1);
                glVertex2i(j0 -1 + WALL_BLOCK_SIZE, i0 + 1);
                glVertex2i(j0 - 1 + WALL_BLOCK_SIZE, i0 - 1 + WALL_BLOCK_SIZE);
                glVertex2i(j0 + 1, i0 - 1 + WALL_BLOCK_SIZE);
            glEnd();
        }   
    }
}

/// Player movement
/// Update coordinates upon button presses. Redraw the screen
void playerMove(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': py -= STEP_SIZE; break;
        case 's': py += STEP_SIZE; break;
        case 'a': px -= STEP_SIZE; break;
        case 'd': px += STEP_SIZE; break;
        default: break;
    }
    glutPostRedisplay();
}

/// OpenGL Display Functionality
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the color buffer (background)
    drawMap2D();
    drawPlayer();
    glutSwapBuffers();
}

// Initialization function
void init() {
    glClearColor(0.3, 0.3, 0.3, 0);  // Set background color to grey
    gluOrtho2D(0, WIDTH, HEIGHT, 0); // Define the window size
    px = 300, py = 300;
}

/// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Raycasting Engine");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(playerMove);
    glutMainLoop();
    return 0;
}