#include "menu.h"
#include <stdio.h>   // Para printf
#include <stdlib.h>  // Para exit

OpcaoMenu exibirMenu(int screenWidth, int screenHeight) {
    OpcaoMenu selecao = NOVO_JOGO;
    bool menuAberto = true;

    while (menuAberto) {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("BOMBERMAN", screenWidth / 2 - MeasureText("BOMBERMAN", 60) / 2, screenHeight / 4, 60, BLACK);

            Color corNovoJogo = (selecao == NOVO_JOGO) ? RED : BLACK;
            Color corContinuarJogo = (selecao == CONTINUAR_JOGO) ? RED : BLACK;
            Color corSairDoJogo = (selecao == SAIR_DO_JOGO) ? RED : BLACK;
            Color corSalvarJogo = (selecao == SALVAR_JOGO) ? RED : BLACK;
            DrawText("Novo Jogo (N)", screenWidth / 2 - MeasureText("Novo Jogo (N)", 30) / 2, screenHeight / 2, 30, corNovoJogo);
            DrawText("Continuar Jogo (C)", screenWidth / 2 - MeasureText("Continuar Jogo (C)", 30) / 2, screenHeight / 2 + 40, 30, corContinuarJogo);
            DrawText("Sair do Jogo (Q)", screenWidth / 2 - MeasureText("Sair do Jogo (Q)", 30) / 2, screenHeight / 2 + 80, 30, corSairDoJogo);
            DrawText("Salvar o jogo (p)", screenWidth / 2 - MeasureText("Salvar o Jogo (p)", 30) /2, screenHeight /2 + 120,30, corSalvarJogo);
        EndDrawing();

        if (IsKeyPressed(KEY_DOWN)) {
            selecao = (OpcaoMenu)((selecao + 1) % 3);
        }
        if (IsKeyPressed(KEY_UP)) {
            selecao = (OpcaoMenu)((selecao - 1 + 3) % 3);
        }
        if (IsKeyPressed(KEY_ENTER)) {
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_N)) {
            selecao = NOVO_JOGO;
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_C)) {
            selecao = CONTINUAR_JOGO;
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_Q)) {
            selecao = SAIR_DO_JOGO;
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_P)) {
            selecao = SALVAR_JOGO;
            menuAberto = false;
        }
    }
    return selecao;
}

// Função para iniciar um novo jogo
void iniciarNovoJogo(char*** mapa, char* nomeMapa, PosicaoMapa* playerGridPosicao, Vector2* playerPosition,
                    int* bombasDisponiveis, int* vidasJogador, int* pontuacaoJogador, int* chavesColetadas, int* nivelAtual,
                    int cellSize) {
    // Libera o mapa existente se houver
    if (*mapa != NULL) {
        liberarMapa(*mapa);
        *mapa = NULL; // Garante que o ponteiro é NULL após liberar
    }

    *nivelAtual = 1;
    // Carrega um novo mapa (sempre "mapa1.txt" para um novo jogo)
    sprintf(nomeMapa, "mapa%d.txt", *nivelAtual);
    *mapa = carregarMapa(nomeMapa);
    if (*mapa == NULL) {
        TraceLog(LOG_ERROR, "ERRO: Nao foi possivel carregar o mapa para um novo jogo!");
        exit(1);
    }

    // Encontra a posicao inicial do jogador no novo mapa
    PosicaoMapa novaPosicaoInicialJogador = encontrarPosicaoInicialJogador(*mapa);
    if (novaPosicaoInicialJogador.linha == -1) {
        TraceLog(LOG_ERROR, "ERRO: Nao foi possivel encontrar a posicao inicial do jogador no novo mapa!");
        liberarMapa(*mapa);
        *mapa = NULL;
        exit(1);
    }

    // Inicializa as variáveis do jogo
    *playerGridPosicao = novaPosicaoInicialJogador;
    *playerPosition = (Vector2){(float)playerGridPosicao->coluna * cellSize, (float)playerGridPosicao->linha * cellSize};
    *bombasDisponiveis = 3;
    *vidasJogador = 3;
    *pontuacaoJogador = 0;
    *chavesColetadas = 0;

    TraceLog(LOG_INFO, "Novo Jogo Iniciado!");
}