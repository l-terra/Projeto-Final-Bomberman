#include "raylib.h"
#include "gameMap.h"
#include "bomba.h"
#include "menu.h"
#include "inimigo.h"
#include <stdio.h>

#define MAX_BOMBAS 5

const int screenWidth = 1200;
const int screenHeight = 600;
const int cellSize = 20;
const int hudHeight = 100; // Area reservada para informacoes do jogador (pontuacao, bombas, vidas...)

Bomba bombas[MAX_BOMBAS]; // Array para armazenar as bombas ativas
int bombasAtivas = 0; // Contador de bombas ativas

// Essas variáveis agora serão inicializadas pela função iniciarNovoJogo() ou no caso de continuar jogo, manterão seus valores.
int bombasDisponiveis;
int vidasJogador;
int pontuacaoJogador;

Inimigo* inimigos = NULL;
int numInimigos = 0;

PosicaoMapa playerGridPosicao;
Vector2 playerPosition;

// Variável para controlar o estado do jogo
EstadoJogo estadoAtualDoJogo = ESTADO_MENU; // O jogo começa no menu

// Função auxiliar para adicionar bombas
void addBomb(PosicaoMapa posicao, double tempoParaExplodir) {
    if (bombasAtivas < MAX_BOMBAS) {
        iniciarBomba(&bombas[bombasAtivas], posicao, tempoParaExplodir);
        bombasAtivas++;
        TraceLog(LOG_INFO, "Bomba plantada em (%d, %d). Bombas ativas: %d", posicao.coluna, posicao.linha, bombasAtivas);
    } else {
        TraceLog(LOG_WARNING, "Maximo de bombas ativas atingido (%d). Nao foi possivel plantar nova bomba.", MAX_BOMBAS);
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Bomberman");
    SetTargetFPS(60);

    // O trecho antigo que tinha aqui agora ta na função iniciarNovoJogo!!!
    char** mapa = NULL; // Inicializa o mapa como NULL

    // O loop principal agora verifica o estado do jogo
    while(!WindowShouldClose() && estadoAtualDoJogo != ESTADO_SAIR) {
        if (estadoAtualDoJogo == ESTADO_MENU) {
            // Se estiver no menu, exibe o menu e espera uma seleção
            OpcaoMenu selecaoMenu = exibirMenu(screenWidth, screenHeight);

            switch (selecaoMenu) {
                case NOVO_JOGO:
                    // Inicia um novo jogo, resetando todas as variáveis
                    iniciarNovoJogo(&mapa, &playerGridPosicao, &playerPosition,
                                    &bombasDisponiveis, &vidasJogador, &pontuacaoJogador,
                                    cellSize);
                    bombasAtivas = 0; // Garante que as bombas ativas também são zeradas para um novo jogo
                    // A limpeza de rastros de explosão não pode ser feita aqui sem modificar bomba.c

                    liberarInimigos(&inimigos, &numInimigos);          // Limpa inimigos de um jogo anterior
                    carregarInimigos(mapa, &inimigos, &numInimigos); // Carrega os novos do mapa

                    estadoAtualDoJogo = ESTADO_JOGANDO; // Muda para o estado de jogo
                    break;
                case CONTINUAR_JOGO:
                    // Se o mapa ainda não foi carregado (primeira vez que o jogo roda e escolhe continuar),
                    // inicia um novo jogo como fallback. Caso contrário, apenas retorna ao jogo atual.
                    if (mapa == NULL) {
                        TraceLog(LOG_WARNING, "Nenhum jogo em andamento. Iniciando um Novo Jogo.");
                        iniciarNovoJogo(&mapa, &playerGridPosicao, &playerPosition,
                                        &bombasDisponiveis, &vidasJogador, &pontuacaoJogador,
                                        cellSize);
                        bombasAtivas = 0; // Também zera bombas ativas se for um "novo jogo" por fallback
                    }
                    estadoAtualDoJogo = ESTADO_JOGANDO; // Apenas muda o estado para voltar ao jogo
                    TraceLog(LOG_INFO, "Retornando ao jogo atual.");
                    break;
                case SAIR_DO_JOGO:
                    estadoAtualDoJogo = ESTADO_SAIR; // Sinaliza para sair do jogo
                    break;
            }
        } else if (estadoAtualDoJogo == ESTADO_JOGANDO) {
            // Lógica do jogo (atualização e desenho)
            // Verifica se a tecla TAB foi pressionada para acessar o menu (pausar)
            if (IsKeyPressed(KEY_TAB)) {
                estadoAtualDoJogo = ESTADO_MENU; // Volta para o menu
                continue; // Pula o restante do loop para processar o menu imediatamente
            }

            int nextPlayerGridX = playerGridPosicao.coluna;
            int nextPlayerGridY = playerGridPosicao.linha;

            // Leitura de input
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) nextPlayerGridX += 1;
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) nextPlayerGridX -= 1;
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) nextPlayerGridY -= 1;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) nextPlayerGridY += 1;

            // Checa se o player pode se mover para a celula alvo
            if(nextPlayerGridX >= 0 && nextPlayerGridX < COLUNAS &&
                nextPlayerGridY >= 0 && nextPlayerGridY < LINHAS)
            {
                char celulaAlvo = mapa[nextPlayerGridY][nextPlayerGridX];
                // So pode mover se a celula for vazia ou se tiver inimigo (perde vida)
                if (celulaAlvo == VAZIO || celulaAlvo == INIMIGO) {
                    playerGridPosicao.coluna = nextPlayerGridX;
                    playerGridPosicao.linha = nextPlayerGridY;

                    playerPosition.x = (float)playerGridPosicao.coluna * cellSize;
                    playerPosition.y = (float)playerGridPosicao.linha * cellSize;
                }
            }

            // Lógica para plantar bomba
            if (IsKeyPressed(KEY_B) && bombasDisponiveis > 0) { // Tecla B: Planta bomba
                // coloca bomba na própria célula do jogador se for vazia
                PosicaoMapa posicaoBomba = playerGridPosicao;

                // Bombas só podem ser plantadas em áreas vazias
                if (mapa[posicaoBomba.linha][posicaoBomba.coluna] == VAZIO) {
                    addBomb(posicaoBomba, TEMPO_EXPLOSAO);
                    bombasDisponiveis--; // Decrementa estoque de bombas
                }
            }

            // Atualização das Bombas
            for (int i = 0; i < bombasAtivas; ) {
                // Se a bomba explodiu, a função atualizarBomba retorna true e a removemos do array
                if (atualizarBomba(&bombas[i], GetFrameTime(), mapa, &pontuacaoJogador, &vidasJogador, playerGridPosicao, &bombasDisponiveis)) {
                    // Remove a bomba do array, movendo a última para a posição atual
                    bombas[i] = bombas[bombasAtivas - 1];
                    bombasAtivas--;
                } else {
                    i++; // Apenas avança se a bomba não foi removida
                }
            }

            // Pontuação nunca fica negativa
            if (pontuacaoJogador < 0) {
                pontuacaoJogador = 0;
            }

            atualizarInimigos(inimigos, numInimigos, mapa, playerGridPosicao, &vidasJogador, &pontuacaoJogador, GetFrameTime());

            BeginDrawing();
                ClearBackground(WHITE);

                desenharMapa(mapa, screenWidth, screenHeight, cellSize);

                // Desenha todas as bombas ativas
                for (int i = 0; i < bombasAtivas; i++) {
                    desenharBomba(&bombas[i], cellSize);
                }
                // desenhar o fogo da bomba (já estava no seu main.c)
                Desenha_fogo_bomba(GetFrameTime(),mapa);

                desenharInimigos(inimigos, numInimigos, cellSize);
                // desenha o jogador
                DrawRectangle((int)playerPosition.x, (int)playerPosition.y, cellSize, cellSize, RED);

                // area reservada para as informacoes
                DrawRectangle(0, screenHeight - hudHeight, screenWidth, hudHeight, LIGHTGRAY);
                DrawText(TextFormat("Bombas: %d", bombasDisponiveis), 20, screenHeight - hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Vidas: %d", vidasJogador), 200, screenHeight - hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Pontuacao: %d", pontuacaoJogador), 400, screenHeight - hudHeight + 20, 20, BLACK);
            EndDrawing();

            // Lógica de fim de jogo (se vidas chegar a 0)
            if (vidasJogador <= 0) {
                TraceLog(LOG_INFO, "Fim de Jogo! Pontuacao Final: %d", pontuacaoJogador);
                // Você pode adicionar uma tela de "Game Over" aqui
                // Por agora, vamos voltar para o menu principal
                estadoAtualDoJogo = ESTADO_MENU;
                liberarMapa(mapa); // Libera o mapa ao "terminar" o jogo
                mapa = NULL; // Garante que um novo jogo será carregado
            }
        }
    }

    // Libera o mapa apenas se ele não for NULL (pode ser NULL se o jogo for encerrado do menu sem iniciar um jogo)
    if (mapa != NULL) {
        liberarMapa(mapa);
    }
    liberarInimigos(&inimigos, &numInimigos);
    CloseWindow();
    return 0;
}
