#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/// Map object
/**
 * All input maps should be in the format
 * 1 1 1 1 1 1 1 1 1 1
 * 1 0 0 0 0 0 0 0 0 1
 * 1 0 0 0 0 0 0 0 0 1
 * 1 1 1 1 1 0 0 1 1 1
 * 1 1 1 1 1 0 0 1 1 1
 * 1 1 0 0 0 0 0 1 1 1
 * 1 1 0 0 0 0 0 0 0 1
 * 1 1 1 0 0 0 0 0 0 1
 * 1 1 1 0 0 0 0 1 1 1
 * 1 1 1 1 1 1 1 1 1 1
*/
typedef struct map {
    int width, height;
    char* map;
} Map;

/// Functionality
Map *initMap(char *fileName);
void deleteMap(Map *m);
inline int getIndex(Map *m, int row, int col);
void printMap(Map *m);

/// Initialize the Map object
Map *initMap(char* filePath) {
    Map* M = (Map *)malloc(sizeof(Map));
    if (M == NULL) {
        printf("Error while allocating a Map object\n");
        return NULL;
    }
    // Open map file
    FILE *file;
    file = fopen(filePath, "r");
    // Check if file was opened successfully
    if (file == NULL) {
        printf("Error while opening a Map file\n");
        // Delete Map
        free(M);
        return NULL;
    }
    M->width = 0; M->height = 0;
    // We expect width and height to be greater than or equal to 2
    int width = 0, height=0;
    char c;
    int* row;
    // Get the length of the file
    fseek (file, 0, SEEK_END);
    long length = ftell (file);
    fseek (file, 0, SEEK_SET);
    // Allocate map data memory
    M->map = (char *)malloc(sizeof(char) * length);
    if (M->map == NULL) {
        free(M);
        printf("Error while allocating Map memory\n");
        return NULL;
    }
    // Extract the data from file
    char *map_ptr = M->map;
    bool new_row = true;
    do {
        c = fgetc(file);
        // Check input
        switch (c) {
            // wall or empty space
            case '1':
            case '0':
                width++;
                *(map_ptr++) = c;
                // Check if we are on a new row
                if (new_row) {
                    height++;
                    new_row = false;
                }
                break;
            case '\n':
                // Check if the number of columns is consistent
                if (M->width == 0) {
                    M->width = width;
                }
                else if(M->width != width && !new_row) {
                    printf("Error. Inconsistent width in the input Map\n");
                    free(M->map);
                    free(M);
                    return NULL;
                }
                new_row = true;
                width = 0;
            default: break;
        }
    }
    while (c != EOF);
    // Update the height
    M->height = height;
    // Close the file
    fclose(file);

    // Success!
    return M;
}

/// Get element 1d index
inline int getIndex(Map *m, int row, int col) {
    return m->width * row + col;
}

/// Print Map object
void printMap(Map *m) {
    for (int i = 0; i < m->height * m->width; i++) {
        // Print a new row on a new line
        if (i % m->width == 0 && i > 0) {
            printf("\n");
        }
        else if (i > 0) {
            printf(" ");
        }
        printf("%c", m->map[i]);
    }
    printf("\n");
}

// int main() {
//     Map *m = initMap("maps/map1.map");
//     printMap(m);
//     printf("%d\n", m->height);
//     printf("%d\n", m->width);
//     return 0;
// }