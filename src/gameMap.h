#include "raylib.h"
#include <stdlib.h> // srand()
#include <time.h> //  seed do srand()

void initializeGameMap(int gridHeight, int gridWidth, int gameMap[40][40]) {
    srand(time(NULL));
    for(int row = 0; row < gridHeight; row++) {
        for(int col = 0; col < gridWidth; col++) {
            // Randomly set cells as free (0) or obstacle (1)
            // Adjust the likelihood by changing the modulo value (e.g., % 4 for 25% obstacles)
            gameMap[row][col] = (rand() % 5 == 0) ? 1 : 0; // Approx. 20% chance of an obstacle

            if(row == 20 && col == 20) {
                gameMap[row][col] = 0;
            }
        }
    }
}