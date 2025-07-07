#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "gameMap.h" // Se as funções do menu usarem PosicaoMapa, por exemplo.

// Definição dos estados do jogo
typedef enum {
    ESTADO_MENU,
    ESTADO_JOGANDO,
    ESTADO_PAUSADO,
    ESTADO_GAMEOVER,
    ESTADO_VITORIA,
    ESTADO_ZERADO,
    ESTADO_SAIR
} EstadoJogo;

// Definição das opções do menu
typedef enum {
    NOVO_JOGO,
    CONTINUAR_JOGO, // Se você tiver uma opção de continuar jogo
    SAIR_DO_JOGO
} OpcaoMenu;

// Declaração da função para exibir o menu
OpcaoMenu exibirMenu(int screenWidth, int screenHeight);

// Declaração da função para iniciar um novo jogo
void iniciarNovoJogo(char*** mapa, char* nomeMapa, PosicaoMapa* playerGridPosicao, Vector2* playerPosition, int* bombasDisponiveis, int* vidasJogador, int* pontuacaoJogador, int* chavesColetadas, int* nivelAtual, int cellSize);

// Se houver alguma função para desenhar fogo da bomba no menu ou main
void Desenha_fogo_bomba(double deltaTime, char** mapa); // Esta função aparece no main.c, deve ser declarada em bomba.h ou menu.h se for usada aqui. Sugiro bomba.h.

#endif // MENU_H