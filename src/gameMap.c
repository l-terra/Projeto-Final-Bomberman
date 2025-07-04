#include "gameMap.h"
#include <stdio.h>   // Para operações de arquivo (fopen, fclose, etc.) e printf()
#include <stdlib.h>  // Para alocação dinâmica (malloc, free)
#include <string.h>  // Para manipulação de strings (fgets)

// Função para carregar o mapa a partir de um arquivo texto
char** carregarMapa(const char* nomeArquivo) {
    FILE* file = fopen(nomeArquivo, "r");
    if(file == NULL) {
        printf("FALHA AO ABRIR ARQUIVO DO MAPA: %s", nomeArquivo);
        return NULL;
    }

    // Alocação dinâmica para o mapa (array de ponteiros para char)
    char** mapa = (char**) malloc(LINHAS * sizeof(char*));
    if(mapa == NULL) {
        printf("FALHA AO ALOCAR MEMORIA PARA AS LINHAS DO MAPA");
        fclose(file);
        return NULL;
    }

    char linha[COLUNAS + 2]; // +1 para o '\n' e +1 para o '\0'

    for (int i = 0; i < LINHAS; i++) {
        mapa[i] = (char*)malloc(COLUNAS * sizeof(char));
        if (mapa[i] == NULL) {
            printf("FALHA AO ALOCAR MEMORIA PARA O MAPA NA LINHA: %d.", i);
            // Liberar memoria ja alocada antes de retornar
            for (int j = 0; j < i; j++) {
                free(mapa[j]);
            }
            free(mapa);
            fclose(file);
            return NULL;
        }

        if(fgets(linha, sizeof(linha), file) != NULL) {
        // Copia apenas os 60 (NUMERO DE COLUNAS) primeiros caracteres, ignorando '\n' se houver
            for (int j = 0; j < COLUNAS; j++) {
                if (j < strlen(linha) && linha[j] != '\n') {
                    mapa[i][j] = linha[j];
                } else {
                    mapa[i][j] = VAZIO; // Preenche com vazio se a linha for mais curta
                }
            }
        }
        else {
            printf("ERRO DURANTE A LEITURA DO ARQUIVO!");
            for(int j = 0; j<COLUNAS; j++) {
                mapa[i][j] = VAZIO;
            }
        }
    }
    fclose(file);
    return mapa;
}

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
            if(mapa[linha][coluna] == VAZIO) {
                // Fundo para áreas vazias
                DrawRectangle(coluna * cellSize, linha * cellSize, cellSize, cellSize, RAYWHITE);
            }
        }
    }
}

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