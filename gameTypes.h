#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "raylib.h"
#include <stdbool.h>

#define LARGURA_TELA 1200
#define ALTURA_TELA 600
#define TAMANHO_CELULA 20
#define LARGURA_GRID 60
#define ALTURA_GRID 25

typedef enum { 
    JOGANDO,
    PAUSADO,
    NIVEL_COMPLETO,
    JOGO_COMPLETO,
    GAME_OVER
} gameStatus;

typedef enum {
    VAZIO,
    PAREDE_INDESTRUTIVEL,
    PAREDE_DESTRUTIVEL,
    CAIXA,
    CAIXA_CHAVE,
    CHAVE,
    JOGADOR,
    INIMIGO
} tileType;

typedef struct {
    tileType tipo;
    bool temChave;
} mapTile;

typedef struct {
    mapTile **mapa;
    Vector2 posicaoPlayer;
    int chavesColetadas;
    int nivel_atual;
    int vidas;
    int bombas;
    gameStatus status;
} gameState;

typedef struct {
    Vector2 posicao;
} enemy;

#endif