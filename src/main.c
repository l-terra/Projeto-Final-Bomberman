#include "raylib.h"
#include "gameMap.h"
#include <stdio.h>
#include <stdlib.h> // srand()
#include <time.h> //  seed do srand()

const int screenWidth = 1200;
const int screenHeight = 600;
const int cellSize = 20;
const int hudHeight = 100;

int main() {
    InitWindow(screenWidth, screenHeight, "Bomberman");
    SetTargetFPS(60);

    char** mapa = carregarMapa("mapa1.txt");
    if (mapa == NULL) {
        printf("ERRO AO CARREGAR MAPA NA MAIN!");
        CloseWindow();
        return 1;
    }

    // Encontrando posição inicial do jogador no mapa
    PosicaoMapa posicaoInicialJogador = encontrarPosicaoInicialJogador(mapa);

    if(posicaoInicialJogador.linha == -1) {
        // A mensagem de erro já é gerada dentro de 'encontrarPosicaoInicialJogador()'
        liberarMapa(mapa);
        CloseWindow();
        return 1;
    }

    Vector2 playerPosition = {(float)posicaoInicialJogador.coluna * cellSize, (float)posicaoInicialJogador.linha * cellSize};
    
    while(!WindowShouldClose()) {
        int currentPlayerGridX = (int)playerPosition.x/cellSize;
        int currentPlayerGridY = (int)playerPosition.y/cellSize;

        int nextPlayerGridX = currentPlayerGridX;
        int nextPlayerGridY = currentPlayerGridY;

        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) nextPlayerGridX += 1;
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) nextPlayerGridX -= 1;
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) nextPlayerGridY -= 1;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) nextPlayerGridY += 1;

        if(nextPlayerGridX >= 0 && nextPlayerGridX < COLUNAS &&
            nextPlayerGridY >= 0 && nextPlayerGridY < LINHAS) 
        {
            char celulaAlvo = mapa[nextPlayerGridY][nextPlayerGridX];
            if (celulaAlvo == VAZIO || celulaAlvo == INIMIGO) {
                playerPosition.x = (float)nextPlayerGridX * cellSize;
                playerPosition.y = (float)nextPlayerGridY * cellSize;
            }
        }

        BeginDrawing();
            ClearBackground(WHITE);

            desenharMapa(mapa, screenWidth, screenHeight, cellSize);

            // desenha o jogador
            DrawRectangle((int)playerPosition.x, (int)playerPosition.y, cellSize, cellSize, RED);

            // area reservada para as informacoes
            DrawRectangle(0, screenHeight - hudHeight, screenWidth, hudHeight, LIGHTGRAY);
            DrawText("Bombas: 3", 20, screenHeight - hudHeight + 20, 20, BLACK); // Exemplo [cite: 31]
            DrawText("Vidas: 3", 200, screenHeight - hudHeight + 20, 20, BLACK); // Exemplo [cite: 31]
            DrawText("Pontuacao: 100", 400, screenHeight - hudHeight + 20, 20, BLACK); // Exemplo [cite: 31]
        EndDrawing();
    }

    liberarMapa(mapa);
    CloseWindow();
    return 0;
}
