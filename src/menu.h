#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "gameMap.h"
#include "gameDefs.h"

// Definição das opções do menu
typedef enum {
    NOVO_JOGO,
    CONTINUAR_JOGO,
    SAIR_DO_JOGO,
    SALVAR_JOGO,
    VOLTAR_AO_JOGO
} OpcaoMenu;

// Declaração da função para exibir o menu
OpcaoMenu exibirMenu(int screenWidth, int screenHeight);

// Declaração da função para iniciar um novo jogo
void iniciarNovoJogo(GameState* gameState);

#endif // MENU_H