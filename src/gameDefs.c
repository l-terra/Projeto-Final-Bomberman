#include "gameDefs.h"
#include <stdio.h>

void inicializarGameState(GameState* gameState, int screenWidth, int screenHeight, int cellSize, int hudHeight) {
    gameState->screenWidth = screenWidth;
    gameState->screenHeight = screenHeight;
    gameState->cellSize = cellSize;
    gameState->hudHeight = hudHeight;

    // Inicializa áudio
    InitAudioDevice();
    gameState->somPassarFase = LoadSound("assets/passarfase.mp3");
    gameState->somChave = LoadSound("assets/keys.mp3");
    gameState->somExplosao = LoadSound("assets/explosion.mp3");
    gameState->somHit = LoadSound("assets/hit.mp3");
    gameState->musicaVitoria = LoadMusicStream("assets/vitoria.mp3");
    gameState->somMenu = LoadSound("assets/Menu.wav");

    SetSoundVolume(gameState->somExplosao, 0.1f);
    SetSoundVolume(gameState->somHit, 1.0f);
    SetMusicVolume(gameState->musicaVitoria, 0.3f);

    gameState->somPassarFaseTocado = false;

    // Estado inicial do jogo
    gameState->estadoAtualDoJogo = ESTADO_MENU;

    // Inicializa ponteiros para NULL para evitar desreferenciação inválida
    gameState->mapa = NULL;
    gameState->inimigos = NULL;
    gameState->numInimigos = 0;

    // Valores iniciais do jogador e bombas
    gameState->vidasJogador = 3;
    gameState->pontuacaoJogador = 0;
    gameState->chavesColetadas = 0;
    gameState->bombasAtivas = 0;
    gameState->bombasDisponiveis = MAX_BOMBAS;
    gameState->nivelAtual = 1; // Começa no nível 1

    gameState->playerGridPosicao = (PosicaoMapa){-1, -1};
    gameState->playerPosition = (Vector2){0, 0};
    gameState->direcaoAtualJogador = DIR_BAIXO; // Começa virado para baixo.
}

void descarregarGameState(GameState* gameState) {
    // Libera recursos de áudio
    UnloadSound(gameState->somChave);
    UnloadSound(gameState->somExplosao);
    UnloadSound(gameState->somHit);
    UnloadMusicStream(gameState->musicaVitoria);
    UnloadSound(gameState->somMenu);
    CloseAudioDevice();

    // Libera mapa se estiver alocado
    if (gameState->mapa != NULL) {
        liberarMapa(gameState->mapa);
        gameState->mapa = NULL;
    }
    // Libera inimigos se estiverem alocados
    if (gameState->inimigos != NULL) {
        liberarInimigos(&(gameState->inimigos), &(gameState->numInimigos));
    }
}