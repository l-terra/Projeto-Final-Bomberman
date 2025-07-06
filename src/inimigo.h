#ifndef INIMIGO_H
#define INIMIGO_H

#include "raylib.h"
#include "gameMap.h" // Para PosicaoMapa e constantes do mapa

typedef enum {
    CIMA,
    BAIXO,
    ESQUERDA,
    DIREITA,
    NUM_DIRECOES
} Direcao;

typedef struct {
    PosicaoMapa posicao;
    Direcao direcaoAtual;
    bool ativo;
    double tempoParaMudarDirecao;
} Inimigo;

// Protótipos das funções adaptadas
void carregarInimigos(char** mapa, Inimigo** lista_inimigos, int* num_inimigos);
void atualizarInimigos(Inimigo* lista_inimigos, int num_inimigos, char** mapa, PosicaoMapa playerPos, int* vidas, int* pontuacao, double deltaTime);
void desenharInimigos(const Inimigo* lista_inimigos, int num_inimigos, int cellSize);
void liberarInimigos(Inimigo** lista_inimigos, int* num_inimigos);

#endif
