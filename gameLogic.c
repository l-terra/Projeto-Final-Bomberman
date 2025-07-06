#include "gameLogic.h"
#include <stdio.h>
#include <stdlib.h>

bool carregar_mapa(gameState *state, const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        return false;
    }

    state->mapa = (mapTile **)malloc(ALTURA_GRID * sizeof(mapTile *));
    if (state->mapa == NULL) {
        fclose(file);
        return false;
    }
    
    for (int lin = 0; lin < ALTURA_GRID; lin++) {
        state->mapa[lin] = (mapTile *)malloc(LARGURA_GRID * sizeof(mapTile));
        if (state->mapa[lin] == NULL) {
            fclose(file);
            return false;
        }
    }

    for (int lin = 0; lin < ALTURA_GRID; lin++) {
        for (int col = 0; col < LARGURA_GRID; col++) {
            int c = fgetc(file);
            while (c == '\n' || c == '\r') {
                c = fgetc(file);
            }
            if (c == EOF) {
                break;
            }

            state->mapa[lin][col].temChave = false;
            switch (c) {
                case 'W': state->mapa[lin][col].tipo = PAREDE_INDESTRUTIVEL; break;
                case 'D': state->mapa[lin][col].tipo = PAREDE_DESTRUTIVEL; break;
                case 'B': state->mapa[lin][col].tipo = CAIXA; break;
                case 'K':
                    state->mapa[lin][col].tipo = CAIXA;
                    state->mapa[lin][col].temChave = true;
                    break;
                case 'J':
                    state->mapa[lin][col].tipo = VAZIO;
                    state->posicaoPlayer = (Vector2){(float)col * TAMANHO_CELULA, (float)lin * TAMANHO_CELULA};
                    break;
                case 'E': state->mapa[lin][col].tipo = INIMIGO; break;
                default: state->mapa[lin][col].tipo = VAZIO; break;
            }
        }
        if (feof(file)) break;
    }
    fclose(file);
    return true;
}