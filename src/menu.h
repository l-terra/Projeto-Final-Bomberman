#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "gameMap.h"

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

// Definição das opções do menu
typedef enum {
    NOVO_JOGO,
    CONTINUAR_JOGO,
    SAIR_DO_JOGO,
    SALVAR_JOGO
} OpcaoMenu;

// Declaração da função para exibir o menu
OpcaoMenu exibirMenu(int screenWidth, int screenHeight);

// Declaração da função para iniciar um novo jogo
void iniciarNovoJogo(char*** mapa, char* nomeMapa, PosicaoMapa* playerGridPosicao, Vector2* playerPosition, int* bombasDisponiveis, int* vidasJogador, int* pontuacaoJogador, int* chavesColetadas, int* nivelAtual, int cellSize);

#endif // MENU_H