#include "raylib.h"
#include "bomba.h"
#include "gameMap.h"
#include <stdio.h> // Para NULL
#include <stdlib.h>
 // teste
// Função para inicializar uma nova bomba
void iniciarBomba(Bomba* bomba, PosicaoMapa posicao, double tempoParaExplodir) {
    if (bomba != NULL) {
        bomba->posicao = posicao;
        bomba->tempoParaExplodir = tempoParaExplodir;
        bomba->ativa = true;
    }
}

// Função para desenhar a bomba na tela
void desenharBomba(const Bomba* bomba, int cellSize) {
    if (bomba != NULL && bomba->ativa) {
        // Desenha a bomba (um círculo simples, por exemplo)
        // Cor muda para indicar o tempo restante
        Color corBomba = DARKBLUE;
        if (bomba->tempoParaExplodir < 1.0) { // Menos de 1 segundo para explodir
            corBomba = BLACK;
        } else if (bomba->tempoParaExplodir < 2.0) { // Menos de 2 segundos para explodir
            corBomba = ORANGE;
        }
        DrawCircle(bomba->posicao.coluna * cellSize + cellSize / 2,
                   bomba->posicao.linha * cellSize + cellSize / 2,
                   cellSize / 2 - 2, // Um pouco menor que a célula
                   corBomba);
    }
}

// Função para lidar com a lógica da explosão (destruição de elementos, dano ao jogador)
void explosao(PosicaoMapa bombPos, char** mapa, int* pontos, int* vidas, PosicaoMapa playerPos) {
    // A explosão afeta uma região de 100x100 pixels em formato de cruz centrado na bomba 
    // 100 pixels / 20 pixels/célula = 5 células
    // Isso significa 2 células para cima, 2 para baixo, 2 para esquerda, 2 para direita, além da célula central.

    for (int l = 0; l < LINHAS; l++) {
        for (int c = 0; c < COLUNAS; c++) {
            // Verifica se a célula atual (l, c) está dentro do raio de explosão em formato de cruz
            bool dentroDoAlcance = false;

            // Verificar o eixo horizontal
            if (l == bombPos.linha && abs(c - bombPos.coluna) <= CELULAS_ALCANCE_EXPLOSAO) {
                dentroDoAlcance = true;
            }
            // Verificar o eixo vertical
            if (c == bombPos.coluna && abs(l - bombPos.linha) <= CELULAS_ALCANCE_EXPLOSAO) {
                dentroDoAlcance = true;
            }

            if (dentroDoAlcance) {
                char celulaAfetada = mapa[l][c];

                // Elementos afetados pela explosão: caixas, inimigos, paredes destrutíveis 
                // Paredes indestrutíveis não são afetadas 
                if (celulaAfetada == PAREDE_DESTRUTIVEL || celulaAfetada == CAIXA_COM_CHAVE || celulaAfetada == CAIXA_SEM_CHAVE) {
                    mapa[l][c] = VAZIO; // Destrói o elemento, tornando a célula vazia
                    *pontos += 10; // Jogador ganha 10 pontos por obstáculo destrutível explodido 
                } else if (celulaAfetada == INIMIGO) {
                    mapa[l][c] = VAZIO; // Mata o inimigo
                    *pontos += 20; // Jogador ganha 20 pontos por inimigo explodido 
                    // TODO: Em uma implementação mais completa, remover o inimigo da lista de inimigos ativos.
                }
                // Paredes indestrutíveis (PAREDE_INDESTRUTIVEL) não são alteradas
            }
        }
    }

    // Verificar se o jogador está no raio de destruição da bomba quando ela explode
    // A posição do jogador é em pixels, precisa converter para grid
    int playerGridX = playerPos.coluna; // Usando a coluna da PosicaoMapa do jogador
    int playerGridY = playerPos.linha; // Usando a linha da PosicaoMapa do jogador

    bool playerInExplosionRange = false;
    if (playerGridY == bombPos.linha && abs(playerGridX - bombPos.coluna) <= CELULAS_ALCANCE_EXPLOSAO) {
        playerInExplosionRange = true;
    }
    if (playerGridX == bombPos.coluna && abs(playerGridY - bombPos.linha) <= CELULAS_ALCANCE_EXPLOSAO) {
        playerInExplosionRange = true;
    }

    if (playerInExplosionRange) {
        *vidas -= 1; // Jogador perde vida [cite: 46]
        *pontos -= 100; // Jogador perde 100 pontos quando perde vida 
        if (*pontos < 0) *pontos = 0; // Pontuação nunca fica negativa 
        TraceLog(LOG_INFO, "Jogador atingido pela explosao! Vidas: %d", *vidas);
    }
}

// Função para atualizar o estado da bomba (contagem regressiva, explosão)
// Retorna true se a bomba explodiu e deve ser removida, false caso contrário
bool atualizarBomba(Bomba* bomb, double deltaTime, char** mapa, int* pontos, int* vidas, PosicaoMapa playerPos, int* bombasDisponiveis) {
    if (bomb == NULL || !bomb->ativa) {
        return false;
    }

    bomb->tempoParaExplodir -= deltaTime;

    if (bomb->tempoParaExplodir <= 0) {
        explosao(bomb->posicao, mapa, pontos, vidas, playerPos);
        bomb->ativa = false; // Desativa a bomba
        *bombasDisponiveis += 1; // Quando uma bomba plantada explode, o estoque é incrementado 
        TraceLog(LOG_INFO, "Bomba explodiu em (%d, %d). Bombas disponiveis: %d", bomb->posicao.coluna, bomb->posicao.linha, *bombasDisponiveis);
        return true; // Indica que a bomba explodiu e pode ser removida da lista
    }
    return false;
}