#include "raylib.h"
#include "gameMap.h"
#include "bomba.h"
#include "menu.h"
#include "gameDefs.h"
#include "inimigo.h"
#include <stdio.h>
#include <stdlib.h>
#include "save.h"
#include <string.h>


void addBomb(GameState* gameState, PosicaoMapa posicao, double tempoParaExplodir) {
    if (gameState->bombasAtivas < MAX_BOMBAS) {
        iniciarBomba(&gameState->bombas[gameState->bombasAtivas], posicao, tempoParaExplodir);
        gameState->bombasAtivas++;
        TraceLog(LOG_INFO, "Bomba plantada em (%d, %d). Bombas ativas: %d", posicao.coluna, posicao.linha, gameState->bombasAtivas);
    } else {
        TraceLog(LOG_WARNING, "Maximo de bombas ativas atingido (%d). Nao foi possivel plantar nova bomba.", MAX_BOMBAS);
    }
}

int main() {
    GameState gameState;
    inicializarGameState(&gameState, 1200, 600, 20, 100);

    InitWindow(gameState.screenWidth, gameState.screenHeight, "Bomberman");
    SetTargetFPS(60);

    while(!WindowShouldClose() && gameState.estadoAtualDoJogo != ESTADO_SAIR) {
        if (gameState.estadoAtualDoJogo == ESTADO_MENU) {
            OpcaoMenu selecaoMenu = exibirMenu(gameState.screenWidth, gameState.screenHeight);

            switch (selecaoMenu) {
                case NOVO_JOGO:
                    iniciarNovoJogo(&gameState);
                    gameState.bombasAtivas = 0;
                    liberarInimigos(&gameState.inimigos, &gameState.numInimigos);
                    carregarInimigos(gameState.mapa, &gameState.inimigos, &gameState.numInimigos);
                    gameState.estadoAtualDoJogo = ESTADO_JOGANDO;
                    break;

                case CONTINUAR_JOGO:{
                    SaveState estadoCarregado;
                    if(CarregarJogo(&estadoCarregado)){
                        if (gameState.mapa != NULL) {
                            liberarMapa(gameState.mapa);
                        }
                        if (gameState.inimigos!= NULL){
                            liberarInimigos(&gameState.inimigos,&gameState.numInimigos);
                        }
                        gameState.nivelAtual = estadoCarregado.nivelAtual;
                        gameState.pontuacaoJogador = estadoCarregado.pontuacaoJogador;
                        gameState.vidasJogador = estadoCarregado.vidasJogador;
                        gameState.bombasDisponiveis = estadoCarregado.bombasDisponiveis;
                        gameState.chavesColetadas = estadoCarregado.chavesColetadas;

                        gameState.playerGridPosicao = estadoCarregado.playerGridPosicao;
                        gameState.playerPosition.x = (float)gameState.playerGridPosicao.coluna * gameState.cellSize;
                        gameState.playerPosition.y = (float)gameState.playerGridPosicao.linha * gameState.cellSize;

                        memcpy(gameState.bombas, estadoCarregado.bombas, sizeof(Bomba) * MAX_BOMBAS);
                        gameState.bombasAtivas = estadoCarregado.bombasAtivas;

                        // Recria o mapa a partir dos dados salvos
                        gameState.mapa = alocarMapa();
                        for (int i = 0; i < LINHAS; i++) {
                            for (int j = 0; j < COLUNAS; j++) {
                                gameState.mapa[i][j] = estadoCarregado.mapa[i][j];
                            }
                        }

                        // Recria os inimigos a partir dos dados salvos
                        gameState.numInimigos = estadoCarregado.numInimigos;
                        gameState.inimigos = malloc(sizeof(Inimigo) * gameState.numInimigos);
                        memcpy(gameState.inimigos, estadoCarregado.inimigos, sizeof(Inimigo) * gameState.numInimigos);

                    gameState.estadoAtualDoJogo = ESTADO_JOGANDO;
                    TraceLog(LOG_INFO, "jogo carregado,iniciando......");
                    }
                    else{
                        TraceLog(LOG_INFO, "Nenhum jogo salvo encontrado");
                    }
                    break;
                }
                case SAIR_DO_JOGO:
                    gameState.estadoAtualDoJogo = ESTADO_SAIR;
                    break;

                case SALVAR_JOGO:
                    if(gameState.mapa != NULL) {
                        SaveState estadoAtualParaSalvar;

                        estadoAtualParaSalvar.nivelAtual = gameState.nivelAtual;
                        estadoAtualParaSalvar.pontuacaoJogador = gameState.pontuacaoJogador;
                        estadoAtualParaSalvar.vidasJogador = gameState.vidasJogador;
                        estadoAtualParaSalvar.bombasDisponiveis = gameState.bombasDisponiveis;
                        estadoAtualParaSalvar.chavesColetadas = gameState.chavesColetadas;
                        estadoAtualParaSalvar.playerGridPosicao = gameState.playerGridPosicao;
                        memcpy(estadoAtualParaSalvar.bombas, gameState.bombas, sizeof(Bomba) * MAX_BOMBAS);
                        estadoAtualParaSalvar.bombasAtivas = gameState.bombasAtivas;

                        for (int i = 0; i < LINHAS; i++) {
                            for (int j = 0; j < COLUNAS; j++) {
                                estadoAtualParaSalvar.mapa[i][j] = gameState.mapa[i][j];
                            }
                        }

                        memcpy(estadoAtualParaSalvar.inimigos, gameState.inimigos, sizeof(Inimigo) * gameState.numInimigos);
                        estadoAtualParaSalvar.numInimigos = gameState.numInimigos;

                        SalvarJogo(&estadoAtualParaSalvar);
                    } else {
                        TraceLog(LOG_WARNING, "Nenhum jogo em andamento para salvar.");
                    }
                    break;

                case VOLTAR_AO_JOGO: // Este é o novo caso que não deve afetar o salvar
                    // Se o jogo estava em andamento (mapa não NULL), retorna a ele.
                    // Caso contrário, pode ser um estado inicial sem jogo.
                    if (gameState.mapa != NULL) {
                        gameState.estadoAtualDoJogo = ESTADO_JOGANDO;
                    } else {
                        // Se não há um jogo em andamento, permanecer no menu pode ser mais lógico.
                        // Ou mudar para ESTADO_MENU explicitamente, embora já esteja lá.
                        TraceLog(LOG_WARNING, "Nenhum jogo em andamento para voltar. Permanecendo no menu.");
                        gameState.estadoAtualDoJogo = ESTADO_MENU;
                    }
                break;
            }
        }
        else if (gameState.estadoAtualDoJogo == ESTADO_JOGANDO) {
            gameState.somPassarFaseTocado = false;
            if (IsKeyPressed(KEY_TAB)) {
                PlaySound(gameState.somMenu);
                gameState.estadoAtualDoJogo = ESTADO_MENU;
                continue;
            }

            int nextPlayerGridX = gameState.playerGridPosicao.coluna;
            int nextPlayerGridY = gameState.playerGridPosicao.linha;


            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                nextPlayerGridX += 1;
                gameState.direcaoAtualJogador = DIR_DIREITA;
            }
            else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                nextPlayerGridX -= 1;
                gameState.direcaoAtualJogador = DIR_ESQUERDA;
            }
            else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                nextPlayerGridY -= 1;
                gameState.direcaoAtualJogador = DIR_CIMA;
            }
            else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                nextPlayerGridY += 1;
                gameState.direcaoAtualJogador = DIR_BAIXO;
            }
            // ... dentro do loop do ESTADO_JOGANDO

            if(nextPlayerGridX >= 0 && nextPlayerGridX < COLUNAS && nextPlayerGridY >= 0 && nextPlayerGridY < LINHAS) {
    bool obstaculoBomba = false;
    for (int i = 0; i < gameState.bombasAtivas; i++) {
        if (gameState.bombas[i].ativa && gameState.bombas[i].posicao.linha == nextPlayerGridY && gameState.bombas[i].posicao.coluna == nextPlayerGridX) {
            obstaculoBomba = true;
            break;
        }
    }

    char celulaAlvo = gameState.mapa[nextPlayerGridY][nextPlayerGridX];

    // Apenas UMA verificação, combinando toda a lógica
    if (!obstaculoBomba && (celulaAlvo == VAZIO || celulaAlvo == INIMIGO || celulaAlvo == CHAVE)) {
        gameState.playerGridPosicao.coluna = nextPlayerGridX;
        gameState.playerGridPosicao.linha = nextPlayerGridY;
        gameState.playerPosition.x = (float)gameState.playerGridPosicao.coluna * gameState.cellSize;
        gameState.playerPosition.y = (float)gameState.playerGridPosicao.linha * gameState.cellSize;

        // Se a célula era a chave, coleta-a
        if (celulaAlvo == CHAVE) {
            gameState.chavesColetadas++;
            PlaySound(gameState.somChave);
            gameState.mapa[nextPlayerGridY][nextPlayerGridX] = VAZIO;

            // Lógica para verificar se o nível foi concluído
            if (gameState.chavesColetadas == 5) { // Supondo que só há 1 chave por nível
                char nomeNovoMapa[32];
                sprintf(nomeNovoMapa, "mapa%d.txt", gameState.nivelAtual + 1);
                FILE* arquivoNovoMapa = fopen(nomeNovoMapa, "r");
                if (arquivoNovoMapa == NULL) {
                    gameState.estadoAtualDoJogo = ESTADO_ZERADO;
                } else {
                    fclose(arquivoNovoMapa);
                    gameState.estadoAtualDoJogo = ESTADO_VITORIA;
                            }
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_B) && gameState.bombasDisponiveis > 0) {
                // Começa com a posição atual do jogador como base
                PosicaoMapa posicaoBomba = gameState.playerGridPosicao;

                // Ajusta a posição da bomba com base na direção do jogador
                switch (gameState.direcaoAtualJogador) {
                    case DIR_DIREITA:   posicaoBomba.coluna++; break;
                    case DIR_ESQUERDA:  posicaoBomba.coluna--; break;
                    case DIR_CIMA:      posicaoBomba.linha--;  break;
                    case DIR_BAIXO:     posicaoBomba.linha++;  break;
                }

                // Verifica se a posição calculada está dentro dos limites do mapa
                // e se o espaço está vazio para colocar a bomba.
                if (posicaoBomba.coluna >= 0 && posicaoBomba.coluna < COLUNAS &&
                    posicaoBomba.linha >= 0 && posicaoBomba.linha < LINHAS &&
                    gameState.mapa[posicaoBomba.linha][posicaoBomba.coluna] == VAZIO)
                {
                    addBomb(&gameState, posicaoBomba, TEMPO_EXPLOSAO);
                    gameState.bombasDisponiveis--;
                }
            }

            for (int i = 0; i < gameState.bombasAtivas; ) {
                if (atualizarBomba(&gameState.bombas[i], GetFrameTime(), gameState.mapa, &gameState.pontuacaoJogador, &gameState.vidasJogador, gameState.playerGridPosicao, &gameState.bombasDisponiveis, gameState.inimigos, gameState.numInimigos, gameState.somExplosao, gameState.somHit)) {
                    gameState.bombas[i] = gameState.bombas[gameState.bombasAtivas - 1];
                    gameState.bombasAtivas--;
                } else {
                    i++;
                }
            }

            if (gameState.pontuacaoJogador < 0) {
                gameState.pontuacaoJogador = 0;
            }

            // Alteração aqui: Passando as bombas para a função de atualização dos inimigos
            atualizarInimigos(gameState.inimigos, gameState.numInimigos, gameState.mapa, gameState.playerGridPosicao, &gameState.vidasJogador, &gameState.pontuacaoJogador, GetFrameTime(), gameState.somHit, gameState.bombas, gameState.bombasAtivas);

            BeginDrawing();
                ClearBackground(WHITE);
                desenharMapa(gameState.mapa, gameState.screenWidth, gameState.screenHeight, gameState.cellSize);
                for (int i = 0; i < gameState.bombasAtivas; i++) {
                    desenharBomba(&gameState.bombas[i], gameState.cellSize);
                }
                Desenha_fogo_bomba(GetFrameTime(), gameState.mapa);
                desenharInimigos(gameState.inimigos, gameState.numInimigos, gameState.cellSize);
                DrawRectangle((int)gameState.playerPosition.x, (int)gameState.playerPosition.y, gameState.cellSize, gameState.cellSize, RED);
                DrawRectangle(0, gameState.screenHeight - gameState.hudHeight, gameState.screenWidth, gameState.hudHeight, LIGHTGRAY);
                DrawText(TextFormat("Bombas: %d", gameState.bombasDisponiveis), 20, gameState.screenHeight - gameState.hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Vidas: %d", gameState.vidasJogador), 200, gameState.screenHeight - gameState.hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Pontuacao: %d", gameState.pontuacaoJogador), 400, gameState.screenHeight - gameState.hudHeight + 20, 20, BLACK);
            EndDrawing();

            if (gameState.vidasJogador <= 0) {
                TraceLog(LOG_INFO, "Fim de Jogo! Pontuacao Final: %d", gameState.pontuacaoJogador);
                gameState.estadoAtualDoJogo = ESTADO_GAMEOVER;
            }
        }
        else if (gameState.estadoAtualDoJogo == ESTADO_GAMEOVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                gameState.estadoAtualDoJogo = ESTADO_MENU;
                if (gameState.mapa != NULL) {
                    liberarMapa(gameState.mapa);
                    gameState.mapa = NULL;
                }
                liberarInimigos(&gameState.inimigos, &gameState.numInimigos);
                gameState.pontuacaoJogador = 0;
                gameState.vidasJogador = 3;
                gameState.bombasDisponiveis = MAX_BOMBAS;
            }
            if (IsKeyPressed(KEY_Q)) {
                gameState.estadoAtualDoJogo = ESTADO_SAIR;
                if (gameState.mapa != NULL) {
                    liberarMapa(gameState.mapa);
                    gameState.mapa = NULL;
                }
                liberarInimigos(&gameState.inimigos, &gameState.numInimigos);
            }

            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER", gameState.screenWidth / 2 - MeasureText("GAME OVER", 80) / 2, gameState.screenHeight / 4, 80, RED);
                DrawText(TextFormat("Pontuacao Final: %d", gameState.pontuacaoJogador),
                         gameState.screenWidth / 2 - MeasureText(TextFormat("Pontuacao Final: %d", gameState.pontuacaoJogador), 40) / 2,
                         gameState.screenHeight / 2, 40, WHITE);
                DrawText("Pressione ENTER para voltar ao Menu",
                         gameState.screenWidth / 2 - MeasureText("Pressione ENTER para voltar ao Menu", 20) / 2,
                         gameState.screenHeight / 2 + 80, 20, GRAY);
                DrawText("Pressione Q para Sair do Jogo",
                         gameState.screenWidth / 2 - MeasureText("Pressione Q para Sair do Jogo", 20) / 2,
                         gameState.screenHeight / 2 + 120, 20, GRAY);
            EndDrawing();
        } else if (gameState.estadoAtualDoJogo == ESTADO_VITORIA) {
            if (!gameState.somPassarFaseTocado) { //
                PlaySound(gameState.somPassarFase); //
                gameState.somPassarFaseTocado = true; //
            }

            if (IsKeyPressed(KEY_P)) {
                if (IsSoundPlaying(gameState.somPassarFase)) {
                    StopSound(gameState.somPassarFase);
                }

                gameState.nivelAtual++;
                gameState.chavesColetadas = 0;

                liberarMapa(gameState.mapa);
                liberarInimigos(&gameState.inimigos, &gameState.numInimigos);

                sprintf(gameState.nomeMapa, "mapa%d.txt", gameState.nivelAtual);
                gameState.mapa = carregarMapa(gameState.nomeMapa);
                if (gameState.mapa == NULL) {
                    TraceLog(LOG_ERROR, "Falha ao carregar o proximo nivel: %s", gameState.nomeMapa);
                    gameState.estadoAtualDoJogo = ESTADO_MENU;
                    continue;
                }

                gameState.playerGridPosicao = encontrarPosicaoInicialJogador(gameState.mapa);
                gameState.playerPosition = (Vector2){(float)gameState.playerGridPosicao.coluna * gameState.cellSize, (float)gameState.playerGridPosicao.linha * gameState.cellSize};
                carregarInimigos(gameState.mapa, &gameState.inimigos, &gameState.numInimigos);
                gameState.estadoAtualDoJogo = ESTADO_JOGANDO;
            }

            BeginDrawing();
                DrawText("NÍVEL CONCLUÍDO!", GetScreenWidth()/2 - MeasureText("NÍVEL CONCLUÍDO!", 50)/2, 250, 50, GOLD);
                DrawText("Pressione P para o proximo mapa.", GetScreenWidth()/2 - MeasureText("Pressione P para o proximo mapa.", 20)/2, 320, 20, RAYWHITE);
            EndDrawing();
        } else if (gameState.estadoAtualDoJogo == ESTADO_ZERADO) {
            // Verifica se a música já está tocando para evitar reiniciar
            if (!IsMusicStreamPlaying(gameState.musicaVitoria)) {
                PlayMusicStream(gameState.musicaVitoria);
            }
            UpdateMusicStream(gameState.musicaVitoria); // Atualiza o stream da música

            if (IsKeyPressed(KEY_ENTER)) {
                StopMusicStream(gameState.musicaVitoria);
                gameState.estadoAtualDoJogo = ESTADO_MENU;
            } else if (IsKeyPressed (KEY_Q)) {
                StopMusicStream(gameState.musicaVitoria);
                gameState.estadoAtualDoJogo = ESTADO_SAIR;
            }

            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("PARABENS!", GetScreenWidth()/2 - MeasureText("PARABENS!", 60)/2, 250, 60, LIME);
                DrawText("Voce zerou o jogo!", GetScreenWidth()/2 - MeasureText("Voce zerou o jogo!", 40)/2, 320, 40, GREEN);
                DrawText("Pressione ENTER para voltar ao menu ou Q para sair.", GetScreenWidth()/2 - MeasureText("Pressione ENTER para voltar ao menu ou Q para sair.", 20)/2, 450, 20, RAYWHITE);
            EndDrawing();
        }
        // <-- ERRO LÓGICO CORRIGIDO: O bloco duplicado de 'Game Over' que estava aqui foi removido.
    }

    descarregarGameState(&gameState);

    CloseWindow();
    return 0;
}
