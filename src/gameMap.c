#include "gameMap.h"
#include <stdio.h>   // Para operações de arquivo (fopen, fclose, etc.) e printf()
#include <stdlib.h>  // Para alocação dinâmica (malloc, free)
#include <string.h>  // Para manipulação de strings (fgets)


// NOVA FUNÇÃO
// Responsável apenas por alocar memória para o mapa
char** alocarMapa() {
    // 1. Aloca memória para as linhas (um array de ponteiros para char)
    char** mapa = (char**) malloc(LINHAS * sizeof(char*));
    if (mapa == NULL) {
        printf("FALHA AO ALOCAR MEMORIA PARA AS LINHAS DO MAPA");
        return NULL;
    }

    // 2. Para cada linha, aloca memória para as colunas
    for (int i = 0; i < LINHAS; i++) {
        mapa[i] = (char*) malloc(COLUNAS * sizeof(char));
        // Verifica se a alocacao da linha falhou
        if (mapa[i] == NULL) {
            printf("FALHA AO ALOCAR MEMORIA PARA O MAPA NA LINHA: %d.", i);
            // Se falhar, libera tudo que já foi alocado para não vazar memória
            for (int j = 0; j < i; j++) {
                free(mapa[j]);
            }
            free(mapa);
            return NULL;
        }
    }

    // Se tudo deu certo, retorna o ponteiro para o mapa alocado (e vazio)
    return mapa;
}


// Função para carregar o mapa a partir de um arquivo texto
// Em src/gameMap.c

// VERSÃO MODIFICADA
// Agora usa alocarMapa() e apenas preenche os dados do arquivo.
char** carregarMapa(const char* nomeArquivo) {
    // 1. Aloca a memória primeiro, chamando nossa nova função
    char** mapa = alocarMapa();
    if (mapa == NULL) {
        // Se a alocação falhou, não há mais nada a fazer.
        return NULL; 
    }

    // 2. Abre o arquivo para leitura
    FILE* file = fopen(nomeArquivo, "r");
    if(file == NULL) {
        printf("FALHA AO ABRIR ARQUIVO DO MAPA: %s", nomeArquivo);
        liberarMapa(mapa); // Libera a memória que foi alocada antes de sair
        return NULL;
    }

    char linha[COLUNAS + 2]; // Buffer

    // 3. Lê o arquivo e preenche o mapa que já foi alocado
    for (int i = 0; i < LINHAS; i++) {
        if(fgets(linha, sizeof(linha), file) != NULL) {
            for (int j = 0; j < COLUNAS; j++) {
                if (j < (int)strlen(linha) && linha[j] != '\n' && linha[j] != '\r') {
                    mapa[i][j] = linha[j];
                } else {
                    mapa[i][j] = VAZIO;
                }
            }
        } else {
            // Se houver um erro de leitura no meio do arquivo, preenche o resto com vazio
             for (int j = 0; j < COLUNAS; j++) {
                mapa[i][j] = VAZIO;
            }
        }
    }
    
    fclose(file);
    return mapa;
}

// apenas libera a memoria alocada pro mapa
void liberarMapa(char** mapa) {
    if(mapa != NULL) {
        for(int i = 0; i<LINHAS; i++) {
            free(mapa[i]);
        }
        free(mapa);
    }
}

void desenharMapa(char** mapa, int screenWidth, int screenHeight, int cellSize) {
    // A área visível do mapa deve ser de 25 linhas e 60 colunas
    // A janela do jogo tem 600 pixels de altura x 1200 pixels de largura
    // Cada caractere representa um bloco de 20x20 pixels
    // A parte inferior da tela (100 pixels) é reservada para informações

    int mapHeight = screenHeight - 100; // Reserva o espaco de 100px para o HUD

    int gridHeight = mapHeight / cellSize; // 500 / 20 = 25
    int gridWidth = screenWidth / cellSize; // 1200 / 20 = 60

    for(int linha = 0; linha < gridHeight; linha++) {
        for(int coluna = 0; coluna < gridWidth; coluna++) {
            if(mapa[linha][coluna] == PAREDE_INDESTRUTIVEL) {
                // Se for parede indestrutivel -> cinza escuro
                DrawRectangle(coluna * cellSize, linha * cellSize, cellSize, cellSize, DARKGRAY);
            }
            if(mapa[linha][coluna] == PAREDE_DESTRUTIVEL) {
                // Se for parede destrutivel -> cinza
                DrawRectangle(coluna * cellSize, linha * cellSize, cellSize, cellSize, GRAY);
            }
            if(mapa[linha][coluna] == CAIXA_COM_CHAVE || mapa[linha][coluna] == CAIXA_SEM_CHAVE) {
                // Se for caixa -> marrom
                DrawRectangle(coluna * cellSize, linha * cellSize, cellSize, cellSize, BROWN);
            }
            if(mapa[linha][coluna] == INIMIGO) {
                DrawRectangle(coluna * cellSize, linha * cellSize, cellSize, cellSize, PURPLE);
            }
            if (mapa[linha][coluna] == CHAVE) {
                DrawRectangle(coluna * cellSize, linha * cellSize, cellSize, cellSize, GOLD);
            }
            if(mapa[linha][coluna] == VAZIO) {
                // Fundo para áreas vazias
                DrawRectangle(coluna * cellSize, linha * cellSize, cellSize, cellSize, RAYWHITE);
            }
        }
    }
}

// Encontra a posicao inicial do jogador no mapa
// Retorna o struct PosicaoMapa, se o jogador nao for encontrado retorna {-1,-1}
PosicaoMapa encontrarPosicaoInicialJogador(char** mapa) {
    PosicaoMapa posicaoInicialJogador = {-1,-1};

    if(mapa == NULL) {
        printf("O MAPA PASSADO PARA 'encontrarPosicaoInicialJogador' É NULO");
        return posicaoInicialJogador;
    }

    for(int linha = 0; linha < LINHAS; linha++) {
        for(int coluna = 0; coluna < COLUNAS; coluna++) {
            if(mapa[linha][coluna] == INICIO_JOGADOR) {
                posicaoInicialJogador.linha = linha;
                posicaoInicialJogador.coluna = coluna;
                // CELULA DEVE SER ESVAZIADA PARA O PLAYER ENTRAR NA POSICAO
                mapa[linha][coluna] = VAZIO;
                return posicaoInicialJogador;
            }
        }
    }

    printf("NAO FOI POSSIVEL ENCONTRAR A POSICAO INICIAL DO JOGADOR NO MAPA!\n");
    return posicaoInicialJogador; // Retorna {-1,-1}

}
