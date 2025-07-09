#include "raylib.h"
#include "bomba.h"
#include "gameMap.h"
#include <stdio.h> // Para NULL
#include <stdlib.h> // para abs()

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
void desenha_rastro(PosicaoMapa bombPos, char** mapa) {
    // Desenha o centro da explosão
    DrawRectangle(bombPos.coluna * CellSize, bombPos.linha * CellSize, CellSize, CellSize, RED);

    // Expande a explosão em 4 direções (direita, esquerda, baixo, cima)
    for (int dir = 0; dir < 4; dir++) {
        for (int i = 1; i <= CELULAS_ALCANCE_EXPLOSAO; i++) {
            int l = bombPos.linha;
            int c = bombPos.coluna;

            if (dir == 0) c += i; // Direita
            if (dir == 1) c -= i; // Esquerda
            if (dir == 2) l += i; // Baixo
            if (dir == 3) l -= i; // Cima

            // Verifica se está dentro dos limites do mapa
            if (l < 0 || l >= LINHAS || c < 0 || c >= COLUNAS) {
                break; // Para a expansão nesta direção se sair do mapa
            }

            // Desenha o fogo na célula atual
            DrawRectangle(c * CellSize, l * CellSize, CellSize, CellSize, ORANGE);

            // Se a célula atual contém uma parede (destrutível ou não), a explosão para aqui.
            char celulaAtual = mapa[l][c];
            if (celulaAtual == PAREDE_INDESTRUTIVEL|| celulaAtual == PAREDE_DESTRUTIVEL ||
                celulaAtual == CAIXA_COM_CHAVE || celulaAtual == CAIXA_SEM_CHAVE) {
                break; // Para a expansão nesta direção
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

bool processaCelula(PosicaoMapa bombPos, char** mapa, int* pontos, int* vidas, PosicaoMapa playerPos, Inimigo* lista_inimigos, int num_inimigos, int l, int c, Sound somHit){
        bool jogadorAtingido = false;
        // Verifica se a célula está fora do mapa
        if (l < 0 || l >= LINHAS || c < 0 || c >= COLUNAS) {
            return true; // Para a explosão
        }

        // 1. Dano ao Jogador
        if (!jogadorAtingido && playerPos.linha == l && playerPos.coluna == c) {
            *vidas -= 1;
            *pontos -= 100;
            if (*pontos < 0) *pontos = 0;
            PlaySound(somHit);
            TraceLog(LOG_INFO, "Jogador atingido pela explosao! Vidas: %d", *vidas);
            jogadorAtingido = true; // Evita dano múltiplo pela mesma bomba
        }

        // 2. Dano aos Inimigos
        for (int i = 0; i < num_inimigos; i++) {
            if (lista_inimigos[i].ativo && lista_inimigos[i].posicao.linha == l && lista_inimigos[i].posicao.coluna == c) {
                lista_inimigos[i].ativo = false;
                *pontos += 20;
                TraceLog(LOG_INFO, "Inimigo %d foi atingido pela explosao!", i);
            }
        }

        // 3. Interação com o Mapa (Paredes)
        char celulaAfetada = mapa[l][c];
        if (celulaAfetada == CAIXA_COM_CHAVE) { 
            mapa[l][c] = CHAVE;                
            *pontos += 10;
            return true; // destrói a caixa e para a explosão
        } else if (celulaAfetada == PAREDE_DESTRUTIVEL || celulaAfetada == CAIXA_SEM_CHAVE) { // Caso contrário, verifica se é outra parede destrutível
            mapa[l][c] = VAZIO;                
            *pontos += 10;
            return true; // Igualmente, destrói a parede e para a explosão                       
        }

        return false; // Explosão continua
    }


// Função para lidar com a lógica da explosão (destruição de elementos, dano ao jogador)
void explosao(PosicaoMapa bombPos, char** mapa, int* pontos, int* vidas, PosicaoMapa playerPos, Inimigo* lista_inimigos, int num_inimigos, Sound somExplosao, Sound somHit) {
    PlaySound(somExplosao);

    // Processa a própria célula da bomba primeiro
    processaCelula (bombPos ,  mapa,  pontos, vidas,  playerPos,  lista_inimigos,  num_inimigos, bombPos.linha,bombPos.coluna, somHit);

    // Expande a explosão em 4 direções
    for (int dir = 0; dir < 4; dir++) {
        for (int i = 1; i <= CELULAS_ALCANCE_EXPLOSAO; i++) {
            int l = bombPos.linha;
            int c = bombPos.coluna;

            if (dir == 0) c += i; // Direita
            if (dir == 1) c -= i; // Esquerda
            if (dir == 2) l += i; // Baixo
            if (dir == 3) l -= i; // Cima

            if (processaCelula (bombPos ,  mapa,  pontos, vidas,  playerPos,  lista_inimigos,  num_inimigos, l,  c, somHit)) {
                break; // Se processaCelula retornou true, a explosão para nesta direção
            }
        }
    }
}

// Função para atualizar o estado da bomba (contagem regressiva, explosão)
// Retorna true se a bomba explodiu e deve ser removida, false caso contrário
bool atualizarBomba(Bomba* bomb, double deltaTime, char** mapa, int* pontos, int* vidas, PosicaoMapa playerPos, int* bombasDisponiveis, Inimigo* lista_inimigos, int num_inimigos, Sound somExplosao, Sound somHit) {
    if (bomb == NULL || !bomb->ativa) {
        return false;
    }

    bomb->tempoParaExplodir -= deltaTime;

    if (bomb->tempoParaExplodir <= 0) {
        // MODIFICADO: Passe a lista de inimigos para a função de explosão
        explosao(bomb->posicao, mapa, pontos, vidas, playerPos, lista_inimigos, num_inimigos, somExplosao, somHit);

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
