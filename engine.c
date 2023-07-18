#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

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

/// Other Properties
#define PI 3.1415926535
#define RAY_LEN_FACTOR 10

float px, py;       // player position global variables
float pa;           // player position angle in randians
float x_off, y_off; // x and y offsets based on player angle

/// Draw a player point
void drawPlayer() {
    glColor3f(1, 1, 0);     // yellow color
    glPointSize(10);        // set the point size
    glBegin(GL_POINTS);     // specify geometricx object as POINTS (multiple objects)
        glVertex2f(px, py); // define a single point at the player coordinates
    glEnd();
    // Draw player "vision" vector
    glLineWidth(3);
    glBegin(GL_LINES);
        glVertex2f(px, py);
        glVertex2f(px+x_off*RAY_LEN_FACTOR, py+y_off*RAY_LEN_FACTOR);
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
        case 'w': px += x_off; py += y_off; break;
        case 's': px += x_off; py += y_off; break;
        case 'a': pa -= 0.1; 
                  pa = (pa < 0) ? (2 * M_PI + pa) : pa;
                  x_off = cos(pa) * STEP_SIZE;
                  y_off = sin(pa) * STEP_SIZE;
                  break;
        case 'd': pa += 0.1; 
                  pa = (pa >= 2 * M_PI) ? (pa - 2 * M_PI) : pa;
                  x_off = cos(pa) * STEP_SIZE;
                  y_off = sin(pa) * STEP_SIZE;
                  break;
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
    pa = 0;
    x_off = cos(pa) * STEP_SIZE;
    y_off = sin(pa) * STEP_SIZE;
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