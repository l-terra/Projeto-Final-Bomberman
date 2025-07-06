#include "gameLogic.h"
#include "gameTypes.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"

int main() {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Bomberman");
    SetTargetFPS(60); 

    gameState state = {0}; //inicializa a struct que serve como "painel de controle" com valores nulos.
    state.nivel_atual = 1; //começa o jogo no nível/mapa 1.
    state.chavesColetadas = 0;
    state.status = JOGANDO;

    //salva o nome do arquivo do mapa com o número correspondente ao nível atual. O nome do arquivo no diretório deve ser exatamente igual a este.
    char nomeArquivoDeMapa[32];
    sprintf(nomeArquivoDeMapa, "mapa%d.txt", state.nivel_atual);
    
    //carrega o primeiro mapa, notificando caso haja erro.
    if (!carregar_mapa(&state, nomeArquivoDeMapa)) { 
        return 1;
    }

    while (!WindowShouldClose()) {
        //---------------------------LÓGICA----------------------------------
        switch (state.status) {
            case JOGANDO: {
             //----------------------MOVIMENTAÇÃO----------------------------
            int currentGridX = (int)state.posicaoPlayer.x / TAMANHO_CELULA;
            int currentGridY = (int)state.posicaoPlayer.y / TAMANHO_CELULA;
            int nextGridX = currentGridX;
            int nextGridY = currentGridY;

            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) nextGridY--;
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) nextGridY++;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) nextGridX++;
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) nextGridX--;
            
            //verifica as condições de movimentação e atualiza a posição do jogador de acordo com os comandos
            if (nextGridX >= 0 && nextGridX < LARGURA_GRID && nextGridY >= 0 && nextGridY < ALTURA_GRID) {
                tileType nextTileType = state.mapa[nextGridY][nextGridX].tipo;
                if (nextTileType == VAZIO || nextTileType == INIMIGO || nextTileType == CHAVE) {
                    state.posicaoPlayer = (Vector2){(float)nextGridX * TAMANHO_CELULA, (float)nextGridY * TAMANHO_CELULA}; //necessário recast para float pois a struct vector não armazena inteiros
                    
                    //verifica a coleta das chaves
                    if (nextTileType == CHAVE) {
                        state.chavesColetadas++;
                        state.mapa[nextGridY][nextGridX].tipo = VAZIO; //atualiza a célula que continha a chave para torná-la um espaço vazio

                        //checando se o jogador completou o nível e se há um novo mapa a ser carregado
                        if (state.chavesColetadas == 5) {
                            char novoMapa[32];
                            sprintf(novoMapa, "mapa%d.txt", state.nivel_atual + 1);
                            FILE* novoArquivo = fopen(novoMapa, "r"); //testa a abertura do arquivo para saber se ele existe e o fecha em seguida
                            if (novoArquivo == NULL) {
                                state.status = JOGO_COMPLETO;
                            } else {
                                fclose (novoArquivo);
                                state.status = NIVEL_COMPLETO;
                            }
                        }
                    }
                    /*if (nextTileType == INIMIGO) {
                        implementar sistema de dano ao jogador, incluindo a perda de vidas e alterando status para GAME_OVER caso elas zerem
                    }*/
                }    
            }
            //----------------------FIM DA MOVIMENTAÇÃO----------------------------
            } break;

            case NIVEL_COMPLETO: {
                if (IsKeyDown(KEY_ENTER)) { //muda de fase com a tecla enter
                    state.nivel_atual++;
                    state.chavesColetadas = 0;

                    for (int lin = 0; lin < ALTURA_GRID; lin++) { //libera a memória do mapa anterior
                        free(state.mapa[lin]);
                    }
                    free(state.mapa);

                    sprintf(nomeArquivoDeMapa, "mapa%d.txt", state.nivel_atual); //atualiza a string que contém o nome do mapa com o nome do próximo
                    if (!carregar_mapa(&state, nomeArquivoDeMapa)) {
                        CloseWindow();
                        exit(1);
                    } //carrega o novo mapa dinamicamente
                    state.status = JOGANDO; //reinicia o jogo
                }
            } break;
        
            case JOGO_COMPLETO: {
                if (IsKeyDown(KEY_R)) {
                    if (state.mapa != NULL) {
                        for (int lin = 0; lin < ALTURA_GRID; lin++) {
                            free(state.mapa[lin]);
                        }
                        free(state.mapa);
                    }

                    state.nivel_atual = 1;
                    state.chavesColetadas = 0;
                    state.status = JOGANDO;

                    sprintf(nomeArquivoDeMapa, "mapa%d.txt", state.nivel_atual);
                    if (!carregar_mapa(&state, nomeArquivoDeMapa)) {
                        CloseWindow();
                        exit(1);
                    }
                } 
            } break;
        }
        //---------------------------FIM DA LÓGICA----------------------------------

        //---------------------------RENDERIZAÇÃO-----------------------------------
        BeginDrawing();
            ClearBackground(DARKGRAY);

            switch (state.status) {
                case JOGANDO: {
                    for(int lin = 0; lin < ALTURA_GRID; lin++) {
                        for (int col = 0; col < LARGURA_GRID; col++) {
                            tileType tipo = state.mapa[lin][col].tipo; 
                            if (tipo != VAZIO) {
                                Color corCelula;
                                switch (tipo) {
                                    case PAREDE_INDESTRUTIVEL: corCelula = (Color){50, 50, 50, 255}; break;
                                    case PAREDE_DESTRUTIVEL: corCelula = (Color){130, 82, 45, 255}; break;
                                    case CAIXA: corCelula = (Color){200, 150, 90, 255}; break;
                                    case INIMIGO: corCelula = PINK; break;
                                    case CHAVE: corCelula = GOLD; break;
                                    default: break;
                                }
                                DrawRectangle(col * TAMANHO_CELULA, lin * TAMANHO_CELULA, TAMANHO_CELULA, TAMANHO_CELULA, corCelula);
                                DrawRectangleLines(col * TAMANHO_CELULA, lin * TAMANHO_CELULA, TAMANHO_CELULA, TAMANHO_CELULA, BLACK);
                            }
                        }
                    }
                    DrawRectangleV(state.posicaoPlayer, (Vector2){TAMANHO_CELULA, TAMANHO_CELULA}, MAROON);
                } break;

                case NIVEL_COMPLETO: {
                    DrawText("NÍVEL CONCLUÍDO!", GetScreenWidth()/2 - MeasureText("NÍVEL CONCLUÍDO!", 50)/2, 250, 50, GOLD);
                    DrawText("Pressione [ENTER] para o proximo mapa.", GetScreenWidth()/2 - MeasureText("Pressione [ENTER] para o proximo mapa.", 20)/2, 320, 20, RAYWHITE);
                } break;

                case JOGO_COMPLETO: {
                    ClearBackground(BLACK);
                    DrawText("PARABENS!", GetScreenWidth()/2 - MeasureText("PARABENS!", 60)/2, 250, 60, LIME);
                    DrawText("Voce zerou o jogo!", GetScreenWidth()/2 - MeasureText("Voce zerou o jogo!", 40)/2, 320, 40, GREEN);
                    DrawText("Pressione ESC para sair.", GetScreenWidth()/2 - MeasureText("Pressione ESC para sair.", 20)/2, 450, 20, RAYWHITE);
                } break;

                /*case GAME_OVER: {
                    implementar tela de game over
                }*/
            }
            DrawFPS(10,10);
        EndDrawing();
        //---------------------------FIM DA RENDERIZAÇÃO-----------------------------------
    }

    if (state.mapa != NULL) {
        for (int lin = 0; lin < ALTURA_GRID; lin++) {
            free(state.mapa[lin]);
        }
        free(state.mapa);
    }

    CloseWindow();
    return 0;
}