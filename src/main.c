#include "raylib.h"
#include "gameMap.h"
#include "bomba.h"
#include "menu.h"
#include "inimigo.h"
#include <stdio.h>
#include <stdlib.h>
#include "save.h"
#include <string.h>

const int screenWidth = 1200;
const int screenHeight = 600;
const int cellSize = 20;
const int hudHeight = 100;

Bomba bombas[MAX_BOMBAS];

int bombasAtivas = 0;
int bombasDisponiveis;
int vidasJogador;
int pontuacaoJogador;
int chavesColetadas;
int nivelAtual;
char nomeMapa[32];
Sound somExplosao;
Sound somHit;
Sound somChave;
Sound somPassarFase;
Music musicaVitoria;

bool somPassarFaseTocado = false;

Inimigo* inimigos = NULL;
int numInimigos = 0;

PosicaoMapa playerGridPosicao;
Vector2 playerPosition;
EstadoJogo estadoAtualDoJogo = ESTADO_MENU;


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

    InitAudioDevice();

    // Carrega o sons
    somPassarFase = LoadSound("assets/passarfase.mp3");
    somChave = LoadSound("assets/keys.mp3");
    somExplosao = LoadSound("assets/explosion.mp3"); 
    somHit = LoadSound("assets/hit.mp3");
    musicaVitoria = LoadMusicStream("assets/vitoria.mp3");

    // Ajusta o volume dos sons
    SetSoundVolume(somExplosao, 0.1f);
    SetSoundVolume(somHit, 1.0f);
    SetMusicVolume(musicaVitoria, 0.3f);

    char** mapa = NULL;

    while(!WindowShouldClose() && estadoAtualDoJogo != ESTADO_SAIR) {
        if (estadoAtualDoJogo == ESTADO_MENU) {
            OpcaoMenu selecaoMenu = exibirMenu(screenWidth, screenHeight);

            switch (selecaoMenu) {
                case NOVO_JOGO:
                    iniciarNovoJogo(&mapa, nomeMapa, &playerGridPosicao, &playerPosition,
                                    &bombasDisponiveis, &vidasJogador, &pontuacaoJogador, &chavesColetadas, &nivelAtual,
                                    cellSize);
                    bombasAtivas = 0;
                    liberarInimigos(&inimigos, &numInimigos);
                    carregarInimigos(mapa, &inimigos, &numInimigos);
                    estadoAtualDoJogo = ESTADO_JOGANDO;
                    break;

                case CONTINUAR_JOGO:{
                    SaveState estadoCarregado;
                    if(CarregarJogo(&estadoCarregado)){
                        if (mapa != NULL) {
                            liberarMapa(mapa);
                        }
                        if ( inimigos!= NULL){
                            liberarInimigos(&inimigos,&numInimigos);
                        }
                        nivelAtual = estadoCarregado.nivelAtual;
                        pontuacaoJogador = estadoCarregado.pontuacaoJogador;
                        vidasJogador = estadoCarregado.vidasJogador;
                        bombasDisponiveis = estadoCarregado.bombasDisponiveis;
                        chavesColetadas = estadoCarregado.chavesColetadas;

                        playerGridPosicao = estadoCarregado.playerGridPosicao;
                        playerPosition.x = (float)playerGridPosicao.coluna * cellSize;
                        playerPosition.y = (float)playerGridPosicao.linha * cellSize;

                        memcpy(bombas, estadoCarregado.bombas, sizeof(Bomba) * MAX_BOMBAS);
                        bombasAtivas = estadoCarregado.bombasAtivas;

                        // Recria o mapa a partir dos dados salvos
                        mapa = alocarMapa(); 
                        for (int i = 0; i < LINHAS; i++) {
                            for (int j = 0; j < COLUNAS; j++) {
                                mapa[i][j] = estadoCarregado.mapa[i][j];
                            }
                        }

                        // Recria os inimigos a partir dos dados salvos
                        numInimigos = estadoCarregado.numInimigos;
                        inimigos = malloc(sizeof(Inimigo) * numInimigos);
                        memcpy(inimigos, estadoCarregado.inimigos, sizeof(Inimigo) * numInimigos);

                    estadoAtualDoJogo = ESTADO_JOGANDO;
                    TraceLog(LOG_INFO, "jogo carregado,iniciando......");
                    }
                    else{
                        TraceLog(LOG_INFO, "Nenhum jogo salvo encontrado");
                    }
                    break;      
                }
                case SAIR_DO_JOGO:
                    estadoAtualDoJogo = ESTADO_SAIR;
                    break;

                case SALVAR_JOGO:
                    if(mapa != NULL) {
                        SaveState estadoAtualParaSalvar;

                        estadoAtualParaSalvar.nivelAtual = nivelAtual;
                        estadoAtualParaSalvar.pontuacaoJogador = pontuacaoJogador;
                        estadoAtualParaSalvar.vidasJogador = vidasJogador;
                        estadoAtualParaSalvar.bombasDisponiveis = bombasDisponiveis;
                        estadoAtualParaSalvar.chavesColetadas = chavesColetadas;
                        estadoAtualParaSalvar.playerGridPosicao = playerGridPosicao;
                        memcpy(estadoAtualParaSalvar.bombas, bombas, sizeof(Bomba) * MAX_BOMBAS);
                        estadoAtualParaSalvar.bombasAtivas = bombasAtivas;

                        for (int i = 0; i < LINHAS; i++) {
                            for (int j = 0; j < COLUNAS; j++) {
                                estadoAtualParaSalvar.mapa[i][j] = mapa[i][j];
                            }
                        }
                        
                        memcpy(estadoAtualParaSalvar.inimigos, inimigos, sizeof(Inimigo) * numInimigos);
                        estadoAtualParaSalvar.numInimigos = numInimigos;

                        SalvarJogo(&estadoAtualParaSalvar);
                    } else {
                        TraceLog(LOG_WARNING, "Nenhum jogo em andamento para salvar.");
                    }
                    break; 
            } 
            
        }
        else if (estadoAtualDoJogo == ESTADO_JOGANDO) {
            if (IsKeyPressed(KEY_TAB)) {
                estadoAtualDoJogo = ESTADO_MENU;
                continue;
            }

            int nextPlayerGridX = playerGridPosicao.coluna;
            int nextPlayerGridY = playerGridPosicao.linha;

            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) nextPlayerGridX += 1;
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) nextPlayerGridX -= 1;
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) nextPlayerGridY -= 1;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) nextPlayerGridY += 1;

            // ... dentro do loop do ESTADO_JOGANDO

            if(nextPlayerGridX >= 0 && nextPlayerGridX < COLUNAS && nextPlayerGridY >= 0 && nextPlayerGridY < LINHAS) {
    bool obstaculoBomba = false;
    for (int i = 0; i < bombasAtivas; i++) {
        if (bombas[i].ativa && bombas[i].posicao.linha == nextPlayerGridY && bombas[i].posicao.coluna == nextPlayerGridX) {
            obstaculoBomba = true;
            break;
        }
    }

    char celulaAlvo = mapa[nextPlayerGridY][nextPlayerGridX];
    
    // Apenas UMA verificação, combinando toda a lógica
    if (!obstaculoBomba && (celulaAlvo == VAZIO || celulaAlvo == INIMIGO || celulaAlvo == CHAVE)) {
        playerGridPosicao.coluna = nextPlayerGridX;
        playerGridPosicao.linha = nextPlayerGridY;
        playerPosition.x = (float)playerGridPosicao.coluna * cellSize;
        playerPosition.y = (float)playerGridPosicao.linha * cellSize;

        // Se a célula era a chave, coleta-a
        if (celulaAlvo == CHAVE) {
            chavesColetadas++;
            PlaySound(somChave);
            mapa[nextPlayerGridY][nextPlayerGridX] = VAZIO;

            // Lógica para verificar se o nível foi concluído
            if (chavesColetadas == 1) { // Supondo que só há 1 chave por nível
                char nomeNovoMapa[32];
                sprintf(nomeNovoMapa, "mapa%d.txt", nivelAtual + 1);
                FILE* arquivoNovoMapa = fopen(nomeNovoMapa, "r");
                if (arquivoNovoMapa == NULL) {
                    estadoAtualDoJogo = ESTADO_ZERADO;
                } else {
                    fclose(arquivoNovoMapa);
                    estadoAtualDoJogo = ESTADO_VITORIA;
                            }
                        } 
                    }
                }
            }

            if (IsKeyPressed(KEY_B) && bombasDisponiveis > 0) {
                PosicaoMapa posicaoBomba = playerGridPosicao;
                if (mapa[posicaoBomba.linha][posicaoBomba.coluna] == VAZIO) {
                    addBomb(posicaoBomba, TEMPO_EXPLOSAO);
                    bombasDisponiveis--;
                }
            }

            for (int i = 0; i < bombasAtivas; ) {
                if (atualizarBomba(&bombas[i], GetFrameTime(), mapa, &pontuacaoJogador, &vidasJogador, playerGridPosicao, &bombasDisponiveis, inimigos, numInimigos, somExplosao, somHit)) {
                    bombas[i] = bombas[bombasAtivas - 1];
                    bombasAtivas--;
                } else {
                    i++;
                }
            }

            if (pontuacaoJogador < 0) {
                pontuacaoJogador = 0;
            }

            atualizarInimigos(inimigos, numInimigos, mapa, playerGridPosicao, &vidasJogador, &pontuacaoJogador, GetFrameTime(), somHit);

            BeginDrawing();
                ClearBackground(WHITE);
                desenharMapa(mapa, screenWidth, screenHeight, cellSize);
                for (int i = 0; i < bombasAtivas; i++) {
                    desenharBomba(&bombas[i], cellSize);
                }
                Desenha_fogo_bomba(GetFrameTime(),mapa);
                desenharInimigos(inimigos, numInimigos, cellSize);
                DrawRectangle((int)playerPosition.x, (int)playerPosition.y, cellSize, cellSize, RED);
                DrawRectangle(0, screenHeight - hudHeight, screenWidth, hudHeight, LIGHTGRAY);
                DrawText(TextFormat("Bombas: %d", bombasDisponiveis), 20, screenHeight - hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Vidas: %d", vidasJogador), 200, screenHeight - hudHeight + 20, 20, BLACK);
                DrawText(TextFormat("Pontuacao: %d", pontuacaoJogador), 400, screenHeight - hudHeight + 20, 20, BLACK);
            EndDrawing();

            if (vidasJogador <= 0) {
                TraceLog(LOG_INFO, "Fim de Jogo! Pontuacao Final: %d", pontuacaoJogador);
                estadoAtualDoJogo = ESTADO_GAMEOVER;
            }
        }
        else if (estadoAtualDoJogo == ESTADO_GAMEOVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                estadoAtualDoJogo = ESTADO_MENU;
                if (mapa != NULL) {
                    liberarMapa(mapa);
                    mapa = NULL;
                }
                liberarInimigos(&inimigos, &numInimigos);
                pontuacaoJogador = 0;
                vidasJogador = 3;
                bombasDisponiveis = MAX_BOMBAS;
            }
            if (IsKeyPressed(KEY_Q)) {
                estadoAtualDoJogo = ESTADO_SAIR;
                if (mapa != NULL) {
                    liberarMapa(mapa);
                    mapa = NULL;
                }
                liberarInimigos(&inimigos, &numInimigos);
            }

            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 80) / 2, screenHeight / 4, 80, RED);
                DrawText(TextFormat("Pontuacao Final: %d", pontuacaoJogador),
                         screenWidth / 2 - MeasureText(TextFormat("Pontuacao Final: %d", pontuacaoJogador), 40) / 2,
                         screenHeight / 2, 40, WHITE);
                DrawText("Pressione ENTER para voltar ao Menu",
                         screenWidth / 2 - MeasureText("Pressione ENTER para voltar ao Menu", 20) / 2,
                         screenHeight / 2 + 80, 20, GRAY);
                DrawText("Pressione Q para Sair do Jogo",
                         screenWidth / 2 - MeasureText("Pressione Q para Sair do Jogo", 20) / 2,
                         screenHeight / 2 + 120, 20, GRAY);
            EndDrawing();
        } else if (estadoAtualDoJogo == ESTADO_VITORIA) {
            if (!somPassarFaseTocado) { //
                PlaySound(somPassarFase); //
                somPassarFaseTocado = true; //
            }

            if (IsKeyPressed(KEY_P)) {
                if (IsSoundPlaying(somPassarFase)) {
                    StopSound(somPassarFase);
                }

                nivelAtual++;
                chavesColetadas = 0;

                liberarMapa(mapa);
                liberarInimigos(&inimigos, &numInimigos);

                sprintf(nomeMapa, "mapa%d.txt", nivelAtual);
                mapa = carregarMapa(nomeMapa);
                if (mapa == NULL) {
                    TraceLog(LOG_ERROR, "Falha ao carregar o proximo nivel: %s", nomeMapa);
                    estadoAtualDoJogo = ESTADO_MENU;
                    continue;
                }

                playerGridPosicao = encontrarPosicaoInicialJogador(mapa);
                playerPosition = (Vector2){(float)playerGridPosicao.coluna * cellSize, (float)playerGridPosicao.linha * cellSize};
                carregarInimigos(mapa, &inimigos, &numInimigos);
                estadoAtualDoJogo = ESTADO_JOGANDO;
            }

            BeginDrawing();
                DrawText("NÍVEL CONCLUÍDO!", GetScreenWidth()/2 - MeasureText("NÍVEL CONCLUÍDO!", 50)/2, 250, 50, GOLD);
                DrawText("Pressione P para o proximo mapa.", GetScreenWidth()/2 - MeasureText("Pressione P para o proximo mapa.", 20)/2, 320, 20, RAYWHITE);
            EndDrawing();
        } else if (estadoAtualDoJogo == ESTADO_ZERADO) {
            // Verifica se a música já está tocando para evitar reiniciar
            if (!IsMusicStreamPlaying(musicaVitoria)) {
                PlayMusicStream(musicaVitoria);
            }
            UpdateMusicStream(musicaVitoria); // Atualiza o stream da música

            if (IsKeyPressed(KEY_ENTER)) {
                StopMusicStream(musicaVitoria);
                estadoAtualDoJogo = ESTADO_MENU;
            } else if (IsKeyPressed (KEY_Q)) {
                StopMusicStream(musicaVitoria);
                estadoAtualDoJogo = ESTADO_SAIR;
            }
            
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("PARABENS!", GetScreenWidth()/2 - MeasureText("PARABENS!", 60)/2, 250, 60, LIME);
                DrawText("Voce zerou o jogo!", GetScreenWidth()/2 - MeasureText("Voce zerou o jogo!", 40)/2, 320, 40, GREEN);
                DrawText("Pressione ENTER para voltar ao menu ou Q para sair.", GetScreenWidth()/2 - MeasureText("Pressione ENTER para voltar ao menu ou Q para sair.", 20)/2, 450, 20, RAYWHITE);
            EndDrawing();
        }
        // <-- ERRO LÓGICO CORRIGIDO: O bloco duplicado de 'Game Over' que estava aqui foi removido.
    }

    // Libera o mapa e inimigos ao fechar a janela
    if (mapa != NULL) {
        liberarMapa(mapa);
    }
    liberarInimigos(&inimigos, &numInimigos);

    UnloadSound(somChave);
    UnloadSound(somExplosao);
    UnloadSound(somHit);
    UnloadMusicStream(musicaVitoria);
    CloseAudioDevice(); // Fecha o dispositivo de áudio

    CloseWindow();
    return 0;
}
