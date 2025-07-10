#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include "raylib.h"
#include "gameMap.h"
#include "bomba.h"
#include "inimigo.h"
#include "save.h" // para SaveState ter acesso a GameState para salvar/carregar

// Definição dos estados do jogo
typedef enum {
    ESTADO_MENU,
    ESTADO_JOGANDO,
    ESTADO_PAUSADO,
    ESTADO_GAMEOVER,
    ESTADO_VITORIA,
    ESTADO_ZERADO,
    ESTADO_SAIR,
    ESTADO_SALVAR
} EstadoJogo;

typedef enum {
    DIR_DIREITA,
    DIR_ESQUERDA,
    DIR_CIMA,
    DIR_BAIXO
} DirecaoJogador;

// Estrutura principal para o estado do jogo
typedef struct {
    // Dimensões da tela e célula (constantes, mas podem ser membros se configuráveis)
    int screenWidth;
    int screenHeight;
    int cellSize;
    int hudHeight;

    // Recursos de áudio
    Sound somExplosao;
    Sound somHit;
    Sound somChave;
    Sound somPassarFase;
    Music musicaVitoria;
    Sound somMenu;
    bool somPassarFaseTocado;

    // Estado do jogo
    EstadoJogo estadoAtualDoJogo;

    // Dados do mapa
    char** mapa;
    char nomeMapa[32]; // Para o nome do arquivo do mapa atual

    // Dados do jogador
    PosicaoMapa playerGridPosicao;
    Vector2 playerPosition;
    DirecaoJogador direcaoAtualJogador;
    int vidasJogador;
    int pontuacaoJogador;
    int chavesColetadas;

    // Dados das bombas
    Bomba bombas[MAX_BOMBAS];
    int bombasAtivas;
    int bombasDisponiveis;

    // Dados dos inimigos
    Inimigo* inimigos;
    int numInimigos;

    // Nível atual
    int nivelAtual;

} GameState;

// Função para inicializar o GameState
void inicializarGameState(GameState* gameState, int screenWidth, int screenHeight, int cellSize, int hudHeight);
// Função para descarregar recursos do GameState
void descarregarGameState(GameState* gameState);

#endif // GAMEDEFS_H