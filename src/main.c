#include "raylib.h"
#include "gameMap.h"
#include "bomba.h"
#include <stdio.h>

#define MAX_BOMBAS 5

const int screenWidth = 1200;
const int screenHeight = 600;
const int cellSize = 20;
const int hudHeight = 100; // Area reservada para informacoes do jogador (pontuacao, bombas, vidas...)

Bomba bombas[MAX_BOMBAS]; // Array para armazenar as bombas ativas
int bombasAtivas = 0; // Contador de bombas ativas

int bombasDisponiveis = 3; // Estoque inicial de bombas do jogador
int vidasJogador = 3; // Vidas iniciais do jogador
int pontuacaoJogador = 0; // Pontuação inicial

PosicaoMapa playerGridPosicao;
Vector2 playerPosition;

// Prototipo para uma funcao auxiliar para adicionar bombas
// Implementação da função auxiliar para adicionar bombas
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

    char** mapa = carregarMapa("mapa1.txt");
    if (mapa == NULL) {
        printf("ERRO AO CARREGAR MAPA NA MAIN!");
        CloseWindow();
        return 1;
    }

    // Encontrando posição inicial do jogador no mapa
    PosicaoMapa posicaoInicialJogador = encontrarPosicaoInicialJogador(mapa);

    if(posicaoInicialJogador.linha == -1) {
        // A mensagem de erro já é gerada dentro de 'encontrarPosicaoInicialJogador()'
        liberarMapa(mapa);
        CloseWindow();
        return 1;
    }

    playerGridPosicao = posicaoInicialJogador;
    // Define a posicao inicial do player
    playerPosition = (Vector2){(float)playerGridPosicao.coluna * cellSize, (float)playerGridPosicao.linha * cellSize};
    
    while(!WindowShouldClose()) {
        // --- Atualização da Posição do Jogador ---
        int nextPlayerGridX = playerGridPosicao.coluna;
        int nextPlayerGridY = playerGridPosicao.linha;

        /*int currentPlayerGridX = (int)playerPosition.x/cellSize;
        int currentPlayerGridY = (int)playerPosition.y/cellSize;

        int nextPlayerGridX = currentPlayerGridX;
        int nextPlayerGridY = currentPlayerGridY;*/

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
                playerPosition.x = (float)nextPlayerGridX * cellSize;
                playerPosition.y = (float)nextPlayerGridY * cellSize;
            }
        }

        // --- Lógica para plantar bomba ---
        if (IsKeyPressed(KEY_B) && bombasDisponiveis > 0) { // Tecla B: Planta bomba
            // Calcula a posição à frente do jogador 
            // Para simplificar, vamos colocar na própria célula do jogador se for vazia para este exemplo
            // Você vai precisar de uma lógica mais complexa para "uma posição à frente" dependendo da direção que o jogador está olhando.
            // Por enquanto, vamos considerar a célula atual do jogador.

            PosicaoMapa posicaoBomba = playerGridPosicao;

            // Bombas só podem ser plantadas em áreas vazias 
            if (mapa[posicaoBomba.linha][posicaoBomba.coluna] == VAZIO) {
                addBomb(posicaoBomba, TEMPO_EXPLOSAO);
                bombasDisponiveis--; // Decrementa estoque de bombas 
                // Marcar a célula do mapa como 'bomba' para que o jogador não possa andar nela imediatamente
                // Você pode definir um novo TipoCelulaMapa para BOMB_PLANTED.
                // Por agora, vamos marcar como um 'X' temporário ou um char que não seja VAZIO.
                // Ou, a bomba será tratada como um obstáculo, então o jogador não deve entrar.
                // Para este exemplo, não vamos alterar o mapa para a bomba plantada,
                // mas a lógica de colisão para a bomba (como obstáculo) precisará ser implementada.
            }
        }

        // --- Atualização das Bombas ---
        for (int i = 0; i < bombasAtivas; ) {
            // Se a bomba explodiu, a função atualizarBomba retorna true e a removemos do array
            if (atualizarBomba(&bombas[i], GetFrameTime(), mapa, &pontuacaoJogador, &vidasJogador, posicaoInicialJogador, &bombasDisponiveis)) {
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

        BeginDrawing();
            ClearBackground(WHITE);

            desenharMapa(mapa, screenWidth, screenHeight, cellSize);

            // Desenha todas as bombas ativas
            for (int i = 0; i < bombasAtivas; i++) {
                desenharBomba(&bombas[i], cellSize);
            }

            // desenha o jogador
            DrawRectangle((int)playerPosition.x, (int)playerPosition.y, cellSize, cellSize, RED);

            // area reservada para as informacoes
            DrawRectangle(0, screenHeight - hudHeight, screenWidth, hudHeight, LIGHTGRAY);
            DrawText(TextFormat("Bombas: %d", bombasDisponiveis), 20, screenHeight - hudHeight + 20, 20, BLACK); // Exemplo [cite: 31]
            DrawText(TextFormat("Vidas: %d", vidasJogador), 200, screenHeight - hudHeight + 20, 20, BLACK); // Exemplo [cite: 31]
            DrawText(TextFormat("Pontuacao: %d", pontuacaoJogador), 400, screenHeight - hudHeight + 20, 20, BLACK); // Exemplo [cite: 31]
        EndDrawing();
    }

    liberarMapa(mapa);
    CloseWindow();
    return 0;
}
