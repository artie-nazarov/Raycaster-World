#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/// Window Properties
#define WIDTH 1024
#define HEIGHT 512

/// Player Properties
#define STEP_SIZE 5
#define PLAYER_HEIGHT 6
#define CROUCH_HEIGHT 3

float px, py; // player position global variables

/// Draw a player point
void drawPlayer() {
    glColor3f(1, 1, 0);     // yellow color
    glPointSize(10);        // set the point size
    glBegin(GL_POINTS);     // specify geometricx object as POINTS (multiple objects)
        glVertex2f(px, py); // define a single point at the player coordinates
    glEnd();
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