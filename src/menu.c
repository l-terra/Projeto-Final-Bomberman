#include "menu.h"
#include <stdio.h>   // Para printf
#include <stdlib.h>  // Para exit

Sound somSelecao;
OpcaoMenu exibirMenu(int screenWidth, int screenHeight) {
    OpcaoMenu selecao = NOVO_JOGO;
    bool menuAberto = true;

    while (menuAberto) {
        somSelecao = LoadSound("assets/selecao.wav");
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("BOMBERMAN", screenWidth / 2 - MeasureText("BOMBERMAN", 60) / 2, screenHeight / 4, 60, BLACK);

            Color corNovoJogo = (selecao == NOVO_JOGO) ? RED : BLACK;
            Color corContinuarJogo = (selecao == CONTINUAR_JOGO) ? RED : BLACK;
            Color corSairDoJogo = (selecao == SAIR_DO_JOGO) ? RED : BLACK;
            Color corSalvarJogo = (selecao == SALVAR_JOGO) ? RED : BLACK;
            Color corVoltarAoJogo = (selecao == VOLTAR_AO_JOGO) ? RED : BLACK;
            DrawText("Novo Jogo (N)", screenWidth / 2 - MeasureText("Novo Jogo (N)", 30) / 2, screenHeight / 2, 30, corNovoJogo);
            DrawText("Continuar Jogo (C)", screenWidth / 2 - MeasureText("Continuar Jogo (C)", 30) / 2, screenHeight / 2 + 40, 30, corContinuarJogo);
            DrawText("Sair do Jogo (Q)", screenWidth / 2 - MeasureText("Sair do Jogo (Q)", 30) / 2, screenHeight / 2 + 80, 30, corSairDoJogo);
            DrawText("Salvar o jogo (S)", screenWidth / 2 - MeasureText("Salvar o Jogo (S)", 30) /2, screenHeight /2 + 120,30, corSalvarJogo);
            DrawText("Voltar ao Jogo (V)", screenWidth / 2 - MeasureText("Voltar ao Jogo (V)", 30) /2, screenHeight /2 + 160,30, corVoltarAoJogo); 
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
            PlaySound(somSelecao);
            selecao = NOVO_JOGO;
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_C)) {
            PlaySound(somSelecao);
            selecao = CONTINUAR_JOGO;
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_Q)) {
            PlaySound(somSelecao);
            selecao = SAIR_DO_JOGO;
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_S)) {
            PlaySound(somSelecao);
            selecao = SALVAR_JOGO;
            menuAberto = false;
        }
        if (IsKeyPressed(KEY_V)) { 
            PlaySound(somSelecao);
            selecao = VOLTAR_AO_JOGO;
            menuAberto = false;
        }
    }
    return selecao;
}

// Função para iniciar um novo jogo
void iniciarNovoJogo(GameState* gameState) {
    PlaySound(somSelecao);
    // Libera o mapa existente se houver
    if (gameState->mapa != NULL) {
        liberarMapa(gameState->mapa);
        gameState->mapa = NULL; // Garante que o ponteiro é NULL após liberar
    }

    gameState->nivelAtual = 1;
    // Carrega um novo mapa (sempre "mapa1.txt" para um novo jogo)
    sprintf(gameState->nomeMapa, "mapa%d.txt", gameState->nivelAtual);
    gameState->mapa = carregarMapa(gameState->nomeMapa);
    if (gameState->mapa == NULL) {
        TraceLog(LOG_ERROR, "ERRO: Nao foi possivel carregar o mapa para um novo jogo!");
        exit(1);
    }

    // Encontra a posicao inicial do jogador no novo mapa
    PosicaoMapa novaPosicaoInicialJogador = encontrarPosicaoInicialJogador(gameState->mapa);
    if (novaPosicaoInicialJogador.linha == -1) {
        TraceLog(LOG_ERROR, "ERRO: Nao foi possivel encontrar a posicao inicial do jogador no novo mapa!");
        liberarMapa(gameState->mapa);
        gameState->mapa = NULL;
        exit(1);
    }

    // Inicializa as variáveis do jogo
    gameState->playerGridPosicao = novaPosicaoInicialJogador;
    gameState->playerPosition = (Vector2){(float)gameState->playerGridPosicao.coluna * gameState->cellSize, (float)gameState->playerGridPosicao.linha * gameState->cellSize};
    gameState->bombasDisponiveis = 3;
    gameState->vidasJogador = 3;
    gameState->pontuacaoJogador = 0;
    gameState->chavesColetadas = 0;
    UnloadSound(somSelecao);
    TraceLog(LOG_INFO, "Novo Jogo Iniciado!");
}