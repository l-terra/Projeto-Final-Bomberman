#include "raylib.h"
#include <stdlib.h>
#include <time.h>

const int screenWidth = 800;
const int screenHeight = 800;
const int cellSize = 20;

const int gridWidth = screenWidth / cellSize;
const int gridHeight = screenHeight / cellSize;

int gameMap[40][40];

void initializeGameMap() {
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

int main() {
    InitWindow(screenWidth, screenHeight, "Movimento");
    SetTargetFPS(60);

    initializeGameMap();

    Vector2 playerPosition = {400.0f, 400.0f};
    
    while(!WindowShouldClose()) {
        int currentPlayerGridX = (int)playerPosition.x/cellSize;
        int currentPlayerGridY = (int)playerPosition.y/cellSize;

        int nextPlayerGridX = currentPlayerGridX;
        int nextPlayerGridY = currentPlayerGridY;

        if (IsKeyPressed(KEY_RIGHT)) nextPlayerGridX += 1;
        if (IsKeyPressed(KEY_LEFT)) nextPlayerGridX -= 1;
        if (IsKeyPressed(KEY_UP)) nextPlayerGridY -= 1;
        if (IsKeyPressed(KEY_DOWN)) nextPlayerGridY += 1;

        if(nextPlayerGridX >= 0 && nextPlayerGridX < gridWidth &&
            nextPlayerGridY >= 0 && nextPlayerGridY < gridHeight) 
        {
            if (gameMap[nextPlayerGridY][nextPlayerGridX] == 0) {
                playerPosition.x = (float)nextPlayerGridX * cellSize;
                playerPosition.y = (float)nextPlayerGridY * cellSize;
            }
        }

        if(playerPosition.x < 0 ||
           playerPosition.x + cellSize > screenWidth || 
           playerPosition.y < 0 ||
           playerPosition.y + cellSize > screenHeight) 
        {
            break; 
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawText("MOVA O QUADRADO COM AS SETINHAS", 10, 10, 20, DARKPURPLE);

            for(int row = 0; row < gridHeight; row++) {
                for(int col = 0; col < gridWidth; col++) {
                    Color cellColor = WHITE;
                    if(gameMap[row][col] == 1) {
                        cellColor = GRAY;
                    }
                    DrawRectangle(col * cellSize, row * cellSize, cellSize, cellSize, cellColor);
                    DrawRectangleLines(col * cellSize, row * cellSize, cellSize, cellSize, LIGHTGRAY);
                }
            }

            DrawRectangle((int)playerPosition.x, (int)playerPosition.y, cellSize, cellSize, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
