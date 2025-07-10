#ifndef INIMIGO_H
#define INIMIGO_H

#include "raylib.h"
#include "gameMap.h" // Para PosicaoMapa e constantes do mapa
#include "bomba.h"

typedef enum {
    CIMA,
    BAIXO,
    ESQUERDA,
    DIREITA,
    NUM_DIRECOES
} Direcao;

typedef struct Inimigo{
    PosicaoMapa posicao;
    Direcao direcaoAtual;
    bool ativo;
    double tempoParaMudarDirecao;
} Inimigo;

// Protótipos das funções adaptadas
void carregarInimigos(char** mapa, Inimigo** lista_inimigos, int* num_inimigos);
void atualizarInimigos(Inimigo* lista_inimigos, int num_inimigos, char** mapa, PosicaoMapa playerPos, int* vidas, int* pontuacao, double deltaTime, Sound hitSom, const Bomba* lista_bombas, int num_bombas);
void desenharInimigos(const Inimigo* lista_inimigos, int num_inimigos, int cellSize);
void liberarInimigos(Inimigo** lista_inimigos, int* num_inimigos);

#endif
