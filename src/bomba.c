#include "raylib.h"
#include "bomba.h"
#include "gameMap.h"
#include <stdio.h> // Para NULL
#include <stdlib.h> // para abs()
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
        Color corBomba = BLACK;
        if (bomba->tempoParaExplodir < 1.0) { // Menos de 1 segundo para explodir
            corBomba = ORANGE;
        }
        DrawCircle(bomba->posicao.coluna * cellSize + cellSize / 2,
                   bomba->posicao.linha * cellSize + cellSize / 2,
                   cellSize / 2 - 2, // Um pouco menor que a célula
                   corBomba);
    }
}


RastroExplosao rastros_ativos [MAX_RASTROS];
int num_rastros_ativos = 0;


//desenha o fogo da bomba
void desenha_rastro(PosicaoMapa bombPos, char** mapa){
    for(int l = 0; l<LINHAS ; l++){
        for(int c = 0; c<COLUNAS; c++){
            // verifica se precisamos desenhar o fogo na posicao
            bool alcance_do_fogo = false;

            if(l == bombPos.linha && abs(c - bombPos.coluna) <= CELULAS_ALCANCE_EXPLOSAO){
                alcance_do_fogo = true;
            }

            if (c == bombPos.coluna && abs(l - bombPos.linha) <= CELULAS_ALCANCE_EXPLOSAO){
                alcance_do_fogo = true;
            }

            if (alcance_do_fogo){
                DrawRectangle(c * CellSize,l * CellSize, CellSize, CellSize, ORANGE);
            }
        }
    }
}

// atualiza o rastro e desenha ele
void Desenha_fogo_bomba(double deltaTime, char** mapa) {
    // Itera sobre os rastros ativos, do primeiro ao último
    for (int i = 0; i < num_rastros_ativos; ) {
        // Decrementa o tempo restante do rastro
        rastros_ativos[i].tempoRestante -= deltaTime;

        if (rastros_ativos[i].tempoRestante <= 0) {
            // Se o tempo acabou, removemos este rastro.
            TraceLog(LOG_INFO, "Rastro em (%d, %d) expirou. Removendo...", rastros_ativos[i].posicao.coluna, rastros_ativos[i].posicao.linha);
            rastros_ativos[i] = rastros_ativos[num_rastros_ativos - 1];
            num_rastros_ativos--;
        } else {
            // Se o rastro ainda está ativo, desenhe-o
            desenha_rastro(rastros_ativos[i].posicao, mapa);
            i++;
        }
    }
}


// Função para lidar com a lógica da explosão (destruição de elementos, dano ao jogador)
void explosao(PosicaoMapa bombPos, char** mapa, int* pontos, int* vidas, PosicaoMapa playerPos, Inimigo* lista_inimigos, int num_inimigos) {
    // 1. Destruição de paredes e caixas
    for (int l = 0; l < LINHAS; l++) {
        for (int c = 0; c < COLUNAS; c++) {
            bool dentroDoAlcance = false;
            if (l == bombPos.linha && abs(c - bombPos.coluna) <= CELULAS_ALCANCE_EXPLOSAO) dentroDoAlcance = true;
            if (c == bombPos.coluna && abs(l - bombPos.linha) <= CELULAS_ALCANCE_EXPLOSAO) dentroDoAlcance = true;

            if (dentroDoAlcance) {
                char celulaAfetada = mapa[l][c];
                if (celulaAfetada == PAREDE_DESTRUTIVEL || celulaAfetada == CAIXA_COM_CHAVE || celulaAfetada == CAIXA_SEM_CHAVE) {
                    mapa[l][c] = VAZIO;
                    *pontos += 10;
                }
                // REMOVIDO: A verificação de 'INIMIGO' foi removida daqui, pois era incorreta.
            }
        }
    }

    // 2. Dano ao jogador
    bool playerInExplosionRange = false;
    if (playerPos.linha == bombPos.linha && abs(playerPos.coluna - bombPos.coluna) <= CELULAS_ALCANCE_EXPLOSAO) playerInExplosionRange = true;
    if (playerPos.coluna == bombPos.coluna && abs(playerPos.linha - bombPos.linha) <= CELULAS_ALCANCE_EXPLOSAO) playerInExplosionRange = true;

    if (playerInExplosionRange) {
        *vidas -= 1;
        *pontos -= 100;
        if (*pontos < 0) *pontos = 0;
        TraceLog(LOG_INFO, "Jogador atingido pela explosao! Vidas: %d", *vidas);
    }

    // 3. Matar inimigos
    // Itera sobre todos os inimigos para ver se foram atingidos.
    for (int i = 0; i < num_inimigos; i++) {
        // Pula o inimigo se ele já estiver inativo.
        if (!lista_inimigos[i].ativo) continue;

        PosicaoMapa inimigoPos = lista_inimigos[i].posicao;
        bool inimigoAtingido = false;

        // Verifica se a posição do inimigo está no alcance da explosão (eixo horizontal ou vertical)
        if (inimigoPos.linha == bombPos.linha && abs(inimigoPos.coluna - bombPos.coluna) <= CELULAS_ALCANCE_EXPLOSAO) {
            inimigoAtingido = true;
        }
        if (inimigoPos.coluna == bombPos.coluna && abs(inimigoPos.linha - bombPos.linha) <= CELULAS_ALCANCE_EXPLOSAO) {
            inimigoAtingido = true;
        }

        if (inimigoAtingido) {
            lista_inimigos[i].ativo = false; // "Mata" o inimigo, tornando-o inativo
            *pontos += 20; // Adiciona 20 pontos, conforme especificação do trabalho [cite: 159]
            TraceLog(LOG_INFO, "Inimigo %d foi atingido pela explosao!", i);
        }
    }
}

// Função para atualizar o estado da bomba (contagem regressiva, explosão)
// Retorna true se a bomba explodiu e deve ser removida, false caso contrário
bool atualizarBomba(Bomba* bomb, double deltaTime, char** mapa, int* pontos, int* vidas, PosicaoMapa playerPos, int* bombasDisponiveis, Inimigo* lista_inimigos, int num_inimigos) {
    if (bomb == NULL || !bomb->ativa) {
        return false;
    }

    bomb->tempoParaExplodir -= deltaTime;

    if (bomb->tempoParaExplodir <= 0) {
        // MODIFICADO: Passe a lista de inimigos para a função de explosão
        explosao(bomb->posicao, mapa, pontos, vidas, playerPos, lista_inimigos, num_inimigos);

        // Adiciona um novo rastro à lista de rastros ativos
        if (num_rastros_ativos < MAX_RASTROS) {
            rastros_ativos[num_rastros_ativos].posicao = bomb->posicao;
            rastros_ativos[num_rastros_ativos].tempoRestante = TEMPO_RASTRO_MAX;
            num_rastros_ativos++;
            TraceLog(LOG_INFO, "Novo rastro de explosao adicionado em (%d, %d). Rastros ativos: %d", bomb->posicao.coluna, bomb->posicao.linha, num_rastros_ativos);
        }
        bomb->ativa = false; // Desativa a bomba
        *bombasDisponiveis += 1; // Quando uma bomba plantada explode, o estoque é incrementado
        TraceLog(LOG_INFO, "Bomba explodiu em (%d, %d). Bombas disponiveis: %d", bomb->posicao.coluna, bomb->posicao.linha, *bombasDisponiveis);
        return true; // Indica que a bomba explodiu e pode ser removida da lista
    }
    return false;
}
