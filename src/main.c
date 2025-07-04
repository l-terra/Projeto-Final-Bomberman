#include "raylib.h"
#include "gameMap.h"
#include <stdlib.h> // srand()
#include <time.h> //  seed do srand()

const int screenWidth = 800;
const int screenHeight = 800;
const int cellSize = 20;

const int gridWidth = screenWidth / cellSize;
const int gridHeight = screenHeight / cellSize;

int gameMap[40][40];

int main() {
    InitWindow(screenWidth, screenHeight, "Movimento");
    SetTargetFPS(60);

    initializeGameMap(gridHeight, gridWidth, gameMap);

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

        BeginDrawing();
            ClearBackground(WHITE);
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
