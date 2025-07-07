#include "raylib.h"
#include "gameMap.h"
#include "bomba.h"
#include "menu.h"
#include "inimigo.h"
#include <stdio.h>
#include <stdlib.h> // Para exit, se usado em funções auxiliares

#define MAX_BOMBAS 5

const int screenWidth = 1200;
const int screenHeight = 600;
const int cellSize = 20;
const int hudHeight = 100;

Bomba bombas[MAX_BOMBAS];
int bombasAtivas = 0;

int bombasDisponiveis;
int vidasJogador;
int pontuacaoJogador;
int chavesColetadas;
int nivelAtual;
char nomeMapa[32];

// <--- Adicione estas declarações de variáveis globais para inimigos
Inimigo* inimigos = NULL;
int numInimigos = 0;

PosicaoMapa playerGridPosicao;
Vector2 playerPosition;

EstadoJogo estadoAtualDoJogo = ESTADO_MENU;

void addBomb(PosicaoMapa posicao, double tempoParaExplodir) {
    if (bombasAtivas < MAX_BOMBAS) {
        iniciarBomba(&bombas[bombasAtivas], posicao, tempoParaExplodir);
        bombasAtivas++;
        TraceLog(LOG_INFO, "Bomba plantada em (%d, %d). Bombas ativas: %d", posicao.coluna, posicao.linha, bombasAtivas);
    } else {
        TraceLog(LOG_WARNING, "Maximo de bombas ativas atingido (%d). Nao foi possivel plantar nova bomba.", MAX_BOMBAS);
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Bomberman");
    SetTargetFPS(60);

    char** mapa = NULL;

    while(!WindowShouldClose() && estadoAtualDoJogo != ESTADO_SAIR) {
        if (estadoAtualDoJogo == ESTADO_MENU) {
            OpcaoMenu selecaoMenu = exibirMenu(screenWidth, screenHeight);

            switch (selecaoMenu) {
                case NOVO_JOGO:
                    iniciarNovoJogo(&mapa, nomeMapa, &playerGridPosicao, &playerPosition,
                                    &bombasDisponiveis, &vidasJogador, &pontuacaoJogador, &chavesColetadas, &nivelAtual,
                                    cellSize);
                    bombasAtivas = 0;
                    liberarInimigos(&inimigos, &numInimigos); // <--- Garante que inimigos antigos são liberados
                    carregarInimigos(mapa, &inimigos, &numInimigos); // <--- Carrega novos inimigos
                    estadoAtualDoJogo = ESTADO_JOGANDO;
                    break;
                case CONTINUAR_JOGO:
                    if (mapa == NULL) {
                        TraceLog(LOG_WARNING, "Nenhum jogo em andamento. Iniciando um Novo Jogo.");
                        iniciarNovoJogo(&mapa, nomeMapa, &playerGridPosicao, &playerPosition,
                                        &bombasDisponiveis, &vidasJogador, &pontuacaoJogador, &chavesColetadas, &nivelAtual,
                                        cellSize);
                        bombasAtivas = 0;
                        // inimigos seriam carregados aqui se fosse um fallback, mas a lógica de um "continue"
                        // implica que eles já estariam lá.
                    }
                    estadoAtualDoJogo = ESTADO_JOGANDO;
                    TraceLog(LOG_INFO, "Retornando ao jogo atual.");
                    break;
                case SAIR_DO_JOGO:
                    estadoAtualDoJogo = ESTADO_SAIR;
                    break;
            }
        } else if (estadoAtualDoJogo == ESTADO_JOGANDO) {
            if (IsKeyPressed(KEY_TAB)) {
                estadoAtualDoJogo = ESTADO_MENU;
                continue;
            }

            int nextPlayerGridX = playerGridPosicao.coluna;
            int nextPlayerGridY = playerGridPosicao.linha;

            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) nextPlayerGridX += 1;
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) nextPlayerGridX -= 1;
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) nextPlayerGridY -= 1;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) nextPlayerGridY += 1;

            if(nextPlayerGridX >= 0 && nextPlayerGridX < COLUNAS &&
                nextPlayerGridY >= 0 && nextPlayerGridY < LINHAS) {
                char celulaAlvo = mapa[nextPlayerGridY][nextPlayerGridX];
                if (celulaAlvo == VAZIO || celulaAlvo == INIMIGO || celulaAlvo == CHAVE) {
                    playerGridPosicao.coluna = nextPlayerGridX;
                    playerGridPosicao.linha = nextPlayerGridY;

                    playerPosition.x = (float)playerGridPosicao.coluna * cellSize;
                    playerPosition.y = (float)playerGridPosicao.linha * cellSize;

                    if (celulaAlvo == CHAVE) {
                        chavesColetadas++;
                        celulaAlvo == VAZIO;

                        if (chavesColetadas == 5) {
                            char nomeNovoMapa[32];
                            sprintf(nomeNovoMapa, "mapa%d.txt", nivelAtual + 1);
                            FILE* arquivoNovoMapa = fopen(nomeNovoMapa, "r");
                            if (arquivoNovoMapa == NULL) {
                                estadoAtualDoJogo = ESTADO_ZERADO;
                            } else {
                                fclose(arquivoNovoMapa);
                                estadoAtualDoJogo = ESTADO_VITORIA;
                            }
                        } 
                    }
                }
            }

            if (IsKeyPressed(KEY_B) && bombasDisponiveis > 0) {
                PosicaoMapa posicaoBomba = playerGridPosicao;
                if (mapa[posicaoBomba.linha][posicaoBomba.coluna] == VAZIO) {
                    addBomb(posicaoBomba, TEMPO_EXPLOSAO);
                    bombasDisponiveis--;
                }
            }

            for (int i = 0; i < bombasAtivas; ) {
                if (atualizarBomba(&bombas[i], GetFrameTime(), mapa, &pontuacaoJogador, &vidasJogador, playerGridPosicao, &bombasDisponiveis, inimigos, numInimigos)) {
                    bombas[i] = bombas[bombasAtivas - 1];
                    bombasAtivas--;
                } else {
                    i++;
                }
            }

            if (pontuacaoJogador < 0) {
                pontuacaoJogador = 0;
            }

            atualizarInimigos(inimigos, numInimigos, mapa, playerGridPosicao, &vidasJogador, &pontuacaoJogador, GetFrameTime()); // <--- Atualiza inimigos

            BeginDrawing();
                ClearBackground(WHITE);
                desenharMapa(mapa, screenWidth, screenHeight, cellSize);
                for (int i = 0; i < bombasAtivas; i++) {
                    desenharBomba(&bombas[i], cellSize);
                }
                Desenha_fogo_bomba(GetFrameTime(),mapa);
                desenharInimigos(inimigos, numInimigos, cellSize); // <--- Desenha inimigos
                DrawRectangle((int)playerPosition.x, (int)playerPosition.y, cellSize, cellSize, RED);

                DrawRectangle(0, screenHeight - hudHeight, screenWidth, hudHeight, LIGHTGRAY);
                DrawText(TextFormat("Bombas: %d", bombasDisponiveis), 20, screenHeight - hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Vidas: %d", vidasJogador), 200, screenHeight - hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Pontuacao: %d", pontuacaoJogador), 400, screenHeight - hudHeight + 20, 20, BLACK);
            EndDrawing();

            if (vidasJogador <= 0) {
                TraceLog(LOG_INFO, "Fim de Jogo! Pontuacao Final: %d", pontuacaoJogador);
                estadoAtualDoJogo = ESTADO_GAMEOVER; // Mude para ESTADO_GAMEOVER
                // Não libera o mapa ou inimigos aqui, isso será feito no ESTADO_GAMEOVER
            }
        }
        else if (estadoAtualDoJogo == ESTADO_GAMEOVER) {
            // Lidar com a entrada do usuário na tela de Game Over
            // ESTA LÓGICA DEVE VIR ANTES DO BEGIN/END DRAWING PARA REAGIR IMEDIATAMENTE
            if (IsKeyPressed(KEY_ENTER)) {
                estadoAtualDoJogo = ESTADO_MENU; // Volta para o menu
                // Libera recursos para um novo jogo
                if (mapa != NULL) {
                    liberarMapa(mapa);
                    mapa = NULL;
                }
                liberarInimigos(&inimigos, &numInimigos); // Libera os inimigos também
                // IMPORTANTE: ZERAR PONTUAÇÃO E VIDAS AQUI PARA UM NOVO JOGO
                pontuacaoJogador = 0;
                vidasJogador = 3; // Ou o valor inicial de vidas
                bombasDisponiveis = MAX_BOMBAS; // Zera as bombas também
            }
            if (IsKeyPressed(KEY_Q)) {
                estadoAtualDoJogo = ESTADO_SAIR; // Sinaliza para sair do jogo
                // Libera recursos antes de sair
                if (mapa != NULL) {
                    liberarMapa(mapa);
                    mapa = NULL;
                }
                liberarInimigos(&inimigos, &numInimigos);
            }

            BeginDrawing(); // Agora o BeginDrawing vem depois da lógica de input
                ClearBackground(BLACK); // Fundo preto para a tela de Game Over

                // Desenha o mapa atrás da tela de Game Over (opcional)
                // if (mapa != NULL) {
                //     desenharMapa(mapa, screenWidth, screenHeight, cellSize);
                // }

                DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 80) / 2, screenHeight / 4, 80, RED);
                DrawText(TextFormat("Pontuacao Final: %d", pontuacaoJogador),
                         screenWidth / 2 - MeasureText(TextFormat("Pontuacao Final: %d", pontuacaoJogador), 40) / 2,
                         screenHeight / 2, 40, WHITE);

                DrawText("Pressione ENTER para voltar ao Menu",
                         screenWidth / 2 - MeasureText("Pressione ENTER para voltar ao Menu", 20) / 2,
                         screenHeight / 2 + 80, 20, GRAY);
                DrawText("Pressione Q para Sair do Jogo",
                         screenWidth / 2 - MeasureText("Pressione Q para Sair do Jogo", 20) / 2,
                         screenHeight / 2 + 120, 20, GRAY);

            EndDrawing();
        } else if (estadoAtualDoJogo == ESTADO_VITORIA) {
            if (IsKeyPressed(KEY_P)) {
                nivelAtual++;
                chavesColetadas = 0;

                liberarMapa(mapa);
                liberarInimigos(&inimigos, &numInimigos);

                sprintf(nomeMapa, "mapa%d.txt", nivelAtual);
                carregarMapa(nomeMapa);
                carregarInimigos(mapa, &inimigos, &numInimigos);

                estadoAtualDoJogo = ESTADO_JOGANDO;
            }

            BeginDrawing();
                DrawText("NÍVEL CONCLUÍDO!", GetScreenWidth()/2 - MeasureText("NÍVEL CONCLUÍDO!", 50)/2, 250, 50, GOLD);
                DrawText("Pressione P para o proximo mapa.", GetScreenWidth()/2 - MeasureText("Pressione P para o proximo mapa.", 20)/2, 320, 20, RAYWHITE);
            EndDrawing();
        } else if (estadoAtualDoJogo == ESTADO_ZERADO) {
            if (IsKeyPressed(KEY_ENTER)) {
                estadoAtualDoJogo = ESTADO_MENU;
            } else if (IsKeyPressed (KEY_Q)) {
                estadoAtualDoJogo = ESTADO_SAIR;
            }
            
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("PARABENS!", GetScreenWidth()/2 - MeasureText("PARABENS!", 60)/2, 250, 60, LIME);
                DrawText("Voce zerou o jogo!", GetScreenWidth()/2 - MeasureText("Voce zerou o jogo!", 40)/2, 320, 40, GREEN);
                DrawText("Pressione ENTER para voltar ao menu ou Q para sair.", GetScreenWidth()/2 - MeasureText("Pressione ENTER para voltar ao menu ou Q para sair.", 20)/2, 450, 20, RAYWHITE);
            EndDrawing();
        }

            // Lidar com a entrada do usuário na tela de Game Over
            if (IsKeyPressed(KEY_ENTER)) {
                estadoAtualDoJogo = ESTADO_MENU; // Volta para o menu
                // Libera recursos para um novo jogo
                if (mapa != NULL) {
                    liberarMapa(mapa);
                    mapa = NULL;
                }
                liberarInimigos(&inimigos, &numInimigos); // Libera os inimigos também
            }
            if (IsKeyPressed(KEY_Q)) {
                estadoAtualDoJogo = ESTADO_SAIR; // Sinaliza para sair do jogo
                // Libera recursos antes de sair
                if (mapa != NULL) {
                    liberarMapa(mapa);
                    mapa = NULL;
                }
                liberarInimigos(&inimigos, &numInimigos);
            }
        }

    // Libera o mapa e inimigos ao fechar a janela
    if (mapa != NULL) {
        liberarMapa(mapa);
    }
    liberarInimigos(&inimigos, &numInimigos); // <--- Libera os inimigos ao sair do jogo
    CloseWindow();
    return 0;
} //game over corrigido
