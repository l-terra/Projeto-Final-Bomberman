#include "inimigo.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h> // Para abs()

#define INTERVALO_MOVIMENTO_INIMIGO 0.9 // Velocidade de movimentação

// Função auxiliar para verificar se uma posição é válida para se mover
static bool podeMoverPara(PosicaoMapa pos, char** mapa, const Inimigo* todosInimigos, int numInimigos, int inimigoAtualId) {
    // 1. Checa limites do mapa
    if (pos.linha < 0 || pos.linha >= LINHAS || pos.coluna < 0 || pos.coluna >= COLUNAS) {
        return false;
    }
    // 2. Checa paredes e caixas
    char celulaAlvo = mapa[pos.linha][pos.coluna];
    if (celulaAlvo != VAZIO) {
        return false;
    }
    // 3. Checa outros inimigos
    for (int i = 0; i < numInimigos; i++) {
        if (i == inimigoAtualId) continue;
        if (todosInimigos[i].ativo && todosInimigos[i].posicao.linha == pos.linha && todosInimigos[i].posicao.coluna == pos.coluna) {
            return false;
        }
    }
    return true;
}


void atualizarInimigos(Inimigo* lista_inimigos, int num_inimigos, char** mapa, PosicaoMapa playerPos, int* vidas, int* pontuacao, double deltaTime, Sound hitSom) {
    static double tempoAcumulado = 0;
    tempoAcumulado += deltaTime;

    if (tempoAcumulado < INTERVALO_MOVIMENTO_INIMIGO) return;
    tempoAcumulado = 0;

    for (int i = 0; i < num_inimigos; i++) {
        Inimigo* inimigo = &lista_inimigos[i];
        if (!inimigo->ativo) continue;

        // --- LÓGICA DE MOVIMENTAÇÃO INTELIGENTE ---
        int deltaX = playerPos.coluna - inimigo->posicao.coluna;
        int deltaY = playerPos.linha - inimigo->posicao.linha;

        Direcao direcaoPrincipal, direcaoOposta, lado1, lado2;

        // Determina as direções prioritárias
        if (abs(deltaX) > abs(deltaY)) {
            direcaoPrincipal = (deltaX > 0) ? DIREITA : ESQUERDA;
            direcaoOposta = (deltaX > 0) ? ESQUERDA : DIREITA;
            lado1 = CIMA;
            lado2 = BAIXO;
        } else {
            direcaoPrincipal = (deltaY > 0) ? BAIXO : CIMA;
            direcaoOposta = (deltaY > 0) ? CIMA : BAIXO;
            lado1 = ESQUERDA;
            lado2 = DIREITA;
        }

        // Tenta mover na melhor direção
        PosicaoMapa proximaPosicao = inimigo->posicao;

        // Tenta mover na direção principal
        proximaPosicao.linha = inimigo->posicao.linha + ((direcaoPrincipal == BAIXO) - (direcaoPrincipal == CIMA));
        proximaPosicao.coluna = inimigo->posicao.coluna + ((direcaoPrincipal == DIREITA) - (direcaoPrincipal == ESQUERDA));
        if (podeMoverPara(proximaPosicao, mapa, lista_inimigos, num_inimigos, i)) {
            inimigo->posicao = proximaPosicao;
        }
        // Se não puder, tenta mover para o primeiro lado
        else {
            proximaPosicao.linha = inimigo->posicao.linha + ((lado1 == BAIXO) - (lado1 == CIMA));
            proximaPosicao.coluna = inimigo->posicao.coluna + ((lado1 == DIREITA) - (lado1 == ESQUERDA));
            if (podeMoverPara(proximaPosicao, mapa, lista_inimigos, num_inimigos, i)) {
                inimigo->posicao = proximaPosicao;
            }
            // Se também não puder, tenta o segundo lado
            else {
                 proximaPosicao.linha = inimigo->posicao.linha + ((lado2 == BAIXO) - (lado2 == CIMA));
                 proximaPosicao.coluna = inimigo->posicao.coluna + ((lado2 == DIREITA) - (lado2 == ESQUERDA));
                 if (podeMoverPara(proximaPosicao, mapa, lista_inimigos, num_inimigos, i)) {
                    inimigo->posicao = proximaPosicao;
                 }
                 // Em último caso, tenta a direção oposta (para desempacar)
                 else {
                    proximaPosicao.linha = inimigo->posicao.linha + ((direcaoOposta == BAIXO) - (direcaoOposta == CIMA));
                    proximaPosicao.coluna = inimigo->posicao.coluna + ((direcaoOposta == DIREITA) - (direcaoOposta == ESQUERDA));
                    if (podeMoverPara(proximaPosicao, mapa, lista_inimigos, num_inimigos, i)) {
                        inimigo->posicao = proximaPosicao;
                    }
                 }
            }
        }

        // Colisão com o jogador
        if (inimigo->ativo && inimigo->posicao.linha == playerPos.linha && inimigo->posicao.coluna == playerPos.coluna) {
            (*vidas)--;
            (*pontuacao) -= 100;
            if (*pontuacao < 0) *pontuacao = 0;
            PlaySound(hitSom);
        }
    }
}

void carregarInimigos(char** mapa, Inimigo** lista_inimigos, int* num_inimigos) {
    int contador = 0;
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (mapa[i][j] == INIMIGO) contador++;
        }
    }
    if (contador == 0) return;
    *lista_inimigos = (Inimigo*) malloc(contador * sizeof(Inimigo));
    if (*lista_inimigos == NULL) exit(1);
    *num_inimigos = contador;
    int inimigoAtual = 0;
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (mapa[i][j] == INIMIGO) {
                (*lista_inimigos)[inimigoAtual].posicao.linha = i;
                (*lista_inimigos)[inimigoAtual].posicao.coluna = j;
                (*lista_inimigos)[inimigoAtual].ativo = true;
                mapa[i][j] = VAZIO;
                inimigoAtual++;
            }
        }
    }
}

void desenharInimigos(const Inimigo* lista_inimigos, int num_inimigos, int cellSize) {
    if (lista_inimigos == NULL) return;
    for (int i = 0; i < num_inimigos; i++) {
        if (lista_inimigos[i].ativo) {
            DrawRectangle(lista_inimigos[i].posicao.coluna * cellSize, lista_inimigos[i].posicao.linha * cellSize, cellSize, cellSize, PURPLE);
        }
    }
}

void liberarInimigos(Inimigo** lista_inimigos, int* num_inimigos) {
    if (*lista_inimigos != NULL) {
        free(*lista_inimigos);
        *lista_inimigos = NULL;
        *num_inimigos = 0;
    }
}
