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
#define WALL_BLOCK_POWER 6

/// Other Properties
#define PI 3.1415926535
#define RAY_LEN_FACTOR 10
#define EPS 1e-5

int px, py;         // player position global variables
float pa;           // player position angle in randians
float x_off, y_off; // x and y offsets based on player angle
Map *m;             // Map object

/// Raycasting
void raycast() {
    float ra;               // ray angle
    int hdx, hdy, hrx, hry; // horizontal ray intercept points and offests
    int vdx, vdy, vrx, vry; // vertical ray intercept points and offests
    int dop;                // depth of perception value in terms of number of blocks (how far a player can see)
    // set intial ray angle to player's view angle
    ra = pa;
    // define depth of perception max ray length
    dop = 10;

    /// Based on the direction of a ray compute the first Horizonatal and Vertical line intercepts
    /// Vertical intercept
    float tanra = tan(ra);
    // ray facing right
    if (ra < M_PI_2 || ra > 3*M_PI_2) {
        // x-intercept
        // The nearest right block edge x coordinate
        vrx = ((px + 64) >> WALL_BLOCK_POWER) << WALL_BLOCK_POWER;
        // y-offset
        vry = (vrx-px) * tanra + py;
        // delta offsets
        vdx = WALL_BLOCK_SIZE;
        vdy = vdx * tanra;
    }
    // ray facing left
    else if (ra > M_PI_2 && ra < 3*M_PI_2) {
        // tangent value should be flipped for 3 and 4 quadrants
        tanra *= -1;
        // x-intercept
        // The nearest left block edge x coordinate
        vrx = ((px >> WALL_BLOCK_POWER) << WALL_BLOCK_POWER) - 0.0001;
        // y-offset
        vry = (px-vrx) * tanra + py;
        // delta offsets
        vdx = -WALL_BLOCK_SIZE;
        vdy = -vdx * tanra;
    }
    // ray facing upward or downward
    else {
        vrx = px;
        vry = py;
        dop = 0;
    }
    /// Horizontal intercept
    float recip_tan = -(1 / tan(ra));  // reciprocal of tan(ra)
    // ray facing sideways (not up or down)
    bool ra_iszero = ((ra <= EPS && ra >= 0) || (ra >= -EPS && ra <= 0));
    if ( ra_iszero || ((ra >= M_PI - EPS && ra <= M_PI + EPS))) {
        hrx = px;
        hry = py;
        hdx = WALL_BLOCK_SIZE;
        hdy=0;
    }
    // ray facing upward
    else if (ra > M_PI) {
        // y-intercept
        // The nearest upper block edge y coordinate
        hry = ((py >> WALL_BLOCK_POWER) << WALL_BLOCK_POWER) - 0.00001;
        // x-offset
        hrx = (py - hry) * recip_tan + px;
        hdy = -WALL_BLOCK_SIZE;
        hdx = -hdy * recip_tan;
    }
    // ray facing downward
    else if (ra < M_PI) {
        // y-intercept
        // The nearest lower block edge y coordinate
        hry = ((py + 64) >> WALL_BLOCK_POWER) << WALL_BLOCK_POWER;
        // x-offset
        hrx = (py - hry) * recip_tan + px;
        hdy = WALL_BLOCK_SIZE;
        hdx = -hdy * recip_tan;
    }
    
    // Cast a ray until it hits a wall
    int ix, iy;                  // indices into the map
    bool vertical_hit = false;   // check whether we hit a vertical edge
    bool horizontal_hit = false; // check whether we hit a horizontal edge
    while (dop) {
        // check horizontal wall
        ix = hrx >> WALL_BLOCK_POWER;
        iy = hry >> WALL_BLOCK_POWER;
        // Check map boundaries and whether we already found a horizontal hit
        if ((ix >= 0 && ix < m->width) && (iy >= 0 && iy < m->height) && !horizontal_hit) {
            // we hit a wall
            //printf("HORIZONTAL (%d, %d) -> (%d, %d) -> %c\n", hrx, hry, iy, ix, m->map[getIndex(m, iy, ix)]);
            if (m->map[getIndex(m, iy, ix)] == '1') { horizontal_hit = true;}
            // otherwise keep casting
            else {
                hrx += hdx;
                hry += hdy;
            }
        }
        // check vertical wall
        ix = vrx >> WALL_BLOCK_POWER;
        iy = vry >> WALL_BLOCK_POWER;
        // Check map boundaries and whether we already found a vertical hit
        if ((ix >= 0 && ix < m->width) && (iy >= 0 && iy < m->height) && !vertical_hit) {
            //printf("VERTICAL (%d, %d) -> (%d, %d) -> %c\n", vrx, vry, iy, ix, m->map[getIndex(m, iy, ix)]);
            if (m->map[getIndex(m, iy, ix)] == '1') { vertical_hit = true;}
            // otherwise keep casting
            else {
                vrx += vdx;
                vry += vdy;
            }
        }
        dop--;
    }

    // Select the smaller distance to a wall (horizontal vs vertical)
    int rx, ry; // ray end point coordinates
    // if ray hit both edges, pick one with the shortest distance from player
    if (horizontal_hit && vertical_hit) {
        //printf("BOTH\n");
        float v_distance = sqrt((px - vrx) * (px - vrx) + (py - vry) * (py - vry));
        float h_distance = sqrt((px - hrx) * (px - hrx) + (py - hry) * (py - hry));
        bool vertical = (v_distance < h_distance);
        rx = vertical ? vrx : hrx;
        ry = vertical ? vry : hry;
    }
    // Horizontal hit occurred
    else if (horizontal_hit) {
        //printf("HOR\n");
        rx = hrx;
        ry = hry;
    }
    // vertical hit occurred
    else {
        //printf("VERT\n");
        rx = vrx;
        ry = vry;
    }
    
    //printf("player_pos: (%d, %d) ray_pos: (%d, %d)\n", px, py, rx, ry);
    // Draw resulting ray
    glColor3f(0, 1, 0);
    glLineWidth(1);
    glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
    glEnd();

}

/// Draw a player point
void drawPlayer() {
    glColor3f(1, 1, 0);     // yellow color
    glPointSize(10);        // set the point size
    glBegin(GL_POINTS);     // specify geometricx object as POINTS (multiple objects)
        glVertex2i(px, py); // define a single point at the player coordinates
    glEnd();
    // Draw player "vision" vector
    glLineWidth(3);
    glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(px+x_off*RAY_LEN_FACTOR, py+y_off*RAY_LEN_FACTOR);
    glEnd();
}

/// Draw Map
void drawMap2D() {
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
        case 's': px -= x_off; py -= y_off; break;
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
    printf("rad=%f cos(t)=%f sin(t)=%f tan(t)=%f\n", pa, cos(pa), sin(pa), tan(pa));

    glutPostRedisplay();
}

/// OpenGL Display Functionality
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the color buffer (background)
    drawMap2D();
    drawPlayer();
    raycast();
    glutSwapBuffers();
}

// Initialization function
void init() {
    glClearColor(0.3, 0.3, 0.3, 0);  // Set background color to grey
    gluOrtho2D(0, WIDTH, HEIGHT, 0); // Define the window size
    // Create a new Map object
    m = initMap(MAP_FILEPATH);
    if (m == NULL) {
        return;
    }
    px = 350, py = 300;              // player initial coorindates
    pa = 0.1;                        // player initial view angle
    x_off = cos(pa) * STEP_SIZE;     // player view x_offset
    y_off = sin(pa) * STEP_SIZE;     // player view y_offset
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