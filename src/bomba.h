#include "raylib.h"
#include "gameMap.h"

#define TEMPO_EXPLOSAO 3.0f // Tempo em segundos para a bomba explodir 
#define CELULAS_ALCANCE_EXPLOSAO 2 
// Alcance da explosão em células (100 pixels / 20 pixels/celula = 5 células no total, 2 para cada lado + centro)
// O raio de 100x100 pixels centrado na bomba significa 5 células de largura e 5 de altura.
// Então, 2 células para cada lado (cima, baixo, esquerda, direita) do centro.

// Estrutura para representar uma bomba
typedef struct {
    PosicaoMapa posicao;            // Posição da bomba no grid (linha, coluna)
    double tempoParaExplodir;      // Tempo restante para explodir (em segundos)
    bool ativa;                   // Indica se a bomba está ativa
} Bomba;

// Função para inicializar uma nova bomba
void iniciarBomba(Bomba* bomba, PosicaoMapa posicao, double tempoParaExplodir);

// Função para desenhar a bomba na tela
void desenharBomba(const Bomba* bomba, int cellSize);

// Função para atualizar o estado da bomba (contagem regressiva, explosão)
// Retorna true se a bomba explodiu, false caso contrário
bool atualizarBomba(Bomba* bomba, double deltaTime, char** mapa, int* pontos, int* vidas, PosicaoMapa posicaoPlayer, int* bombasDisponiveis);
// ponteiros para pontos e vidas para que a explosão possa afetá-los.
// playerPos é para verificar se o jogador está no raio da explosão.
// bombasDisponiveis para incrementar o estoque.

// Função para lidar com a lógica da explosão (destruição de elementos, dano ao jogador)
void explosao(PosicaoMapa posicaoBomba, char** mapa, int* pontos, int* vidas, PosicaoMapa posicaoPlayer);
