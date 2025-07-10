// salvar.h

#ifndef SALVAR_H
#define SALVAR_H

#include "raylib.h"
#include "gameMap.h"  
#include "bomba.h"    
#include "inimigo.h"  


// máximo de inimigos em qualquer fase.
#define MAX_INIMIGOS_SAVE 50

// struct restaurar o jogo exatamente como estava.
typedef struct SaveState {
    // --- DADOS GERAIS DO JOGO ---
    int nivelAtual;
    int pontuacaoJogador;
    int vidasJogador;
    int bombasDisponiveis;
    int chavesColetadas;

    // --- DADOS DO MAPA ---
    // Vamos salvar a cópia exata do mapa, com as paredes já destruídas.
    char mapa[LINHAS][COLUNAS];

    // --- DADOS DO JOGADOR ---
    PosicaoMapa playerGridPosicao;

    // --- DADOS DAS BOMBAS ---
    Bomba bombas[MAX_BOMBAS];
    int bombasAtivas;

    // --- DADOS DOS INIMIGOS ---
    Inimigo inimigos[MAX_INIMIGOS_SAVE];
    int numInimigos;

} SaveState;


// Preenche uma struct SaveState com os dados atuais do jogo e a salva em um arquivo.
int SalvarJogo(SaveState* state);

// Carrega os dados de um arquivo de save para uma struct SaveState.
int CarregarJogo(SaveState* state);


#endif 