#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "raylib.h"

// Tipos possíveis de células que teremos nos mapas
typedef enum {
    VAZIO = ' ',
    INICIO_JOGADOR = 'J',
    PAREDE_INDESTRUTIVEL = 'W',
    PAREDE_DESTRUTIVEL = 'D',
    CAIXA_COM_CHAVE = 'K',
    CAIXA_SEM_CHAVE = 'B',
    CHAVE = 'C',
    INIMIGO = 'E'
} TipoCelulaMapa;

// Estrutura que será utilizada para representar posição
// de um elemento no mapa
typedef struct {
    int linha;
    int coluna;
} PosicaoMapa;

// Definir tamanho padrão do mapa
#define LINHAS 25
#define COLUNAS 60
#define CellSize 20

// Função para carregar o mapa a partir de um arquivo de texto
// Retorna um ponteiro para a matriz do mapa alocada dinamicamente
char** carregarMapa(const char* nomeArquivo);

// Função para liberar a memória alocada para o mapa
void liberarMapa(char** gameMap);

// Função responsável por desenhar o mapa na tela
void desenharMapa(char** gameMap, int screenWidth, int screenHeight, int cellSize);

// Encontra a posicao inicial do jogador no mapa
// Retorna o struct PosicaoMapa, se o jogador nao for encontrado retorna {-1,-1}
PosicaoMapa encontrarPosicaoInicialJogador(char** mapa);
#endif