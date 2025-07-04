#include "gameMap.h"
#include <stdio.h>   // Para operações de arquivo (fopen, fclose, etc.) e printf()
#include <stdlib.h>  // Para alocação dinâmica (malloc, free)
#include <string.h>  // Para manipulação de strings (fgets)

// Função para carregar o mapa a partir de um arquivo texto
char** carregarMapa(const char* nomeArquivo) {
    // Abertura do arquivo no modo de leitura
    FILE* file = fopen(nomeArquivo, "r");
    // Verifica se a abertura do arquivo falhou
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

    // array de char que vai servir como buffer para ler cada linha do arquivo.
    char linha[COLUNAS + 2]; // +1 para o '\n' e +1 para o '\0'

    // Loop principal para ler cada linha do mapa.
    // Itera de 0 até LINHAS-1 para processar todas as linhas do mapa.
    for (int i = 0; i < LINHAS; i++) {
        // Para cada linha, aloca dinamicamente memória para os caracteres dessa linha.
        // COLUNAS = 60 = numero de caracteres em cada linha do mapa
        mapa[i] = (char*)malloc(COLUNAS * sizeof(char));
        // Verifica se a alocacao da linha falhou
        if (mapa[i] == NULL) {
            printf("FALHA AO ALOCAR MEMORIA PARA O MAPA NA LINHA: %d.", i);
            for (int j = 0; j < i; j++) {
                free(mapa[j]);
            }
            free(mapa);
            fclose(file);
            return NULL;
        }

        // Tenta ler uma linha do arquivo para o buffer 'linha'.
        // fgets lê até 'sizeof(linha) - 1' caracteres ou até encontrar um '\n' ou EOF.
        if(fgets(linha, sizeof(linha), file) != NULL) {
            // Se a leitura foi bem-sucedida, copia os caracteres do buffer para a linha do mapa.
            // Este loop copia apenas os 'COLUNAS' primeiros caracteres.
            for (int j = 0; j < COLUNAS; j++) {
                // Verifica se o índice 'j' está dentro do comprimento da linha lida
                // e se o caractere não é uma nova linha ('\n').
                if (j < strlen(linha) && linha[j] != '\n') {
                    mapa[i][j] = linha[j]; // Copia o caractere para a matriz do mapa.
                } else {
                    // Se a linha lida for mais curta que 'COLUNAS' ou se um '\n' for encontrado,
                    // preenche o restante da linha do mapa com o caractere VAZIO.
                    mapa[i][j] = VAZIO;
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