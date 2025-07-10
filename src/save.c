// salvar.c

#include "save.h"
#include <stdio.h>

int SalvarJogo(SaveState* state) {
    // Abre o arquivo "gamesave.dat" em modo de escrita binária ("wb")
    FILE* file = fopen("gamesave.dat", "wb+");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "SALVAR: Nao foi possivel abrir o arquivo gamesave.dat");
        return 0; // Falha
    }

    // Escreve a struct inteira, de uma só vez, no arquivo.
    size_t elements_written = fwrite(state, sizeof(SaveState), 1, file);
    fclose(file);

    if (elements_written > 0) {
        TraceLog(LOG_INFO, "Jogo salvo com sucesso!");
        return 1; // Sucesso
    } else {
        TraceLog(LOG_ERROR, "SALVAR: Erro ao escrever os dados no arquivo.");
        return 0; // Falha
    }
}

int CarregarJogo(SaveState* state) {
    FILE* file = fopen("gamesave.dat", "rb");
    if (file == NULL) {
        TraceLog(LOG_WARNING, "CARREGAR: Arquivo gamesave.dat nao encontrado.");
        return 0; // Falha (não é um erro, o save pode simplesmente não existir)
    }

    // Lê os dados do arquivo diretamente para a nossa struct.
    size_t elements_read = fread(state, sizeof(SaveState), 1, file);
    fclose(file);

    if (elements_read > 0) {
        TraceLog(LOG_INFO, "Jogo carregado com sucesso!");
        return 1; // Sucesso
    } else {
        TraceLog(LOG_ERROR, "CARREGAR: Erro ao ler os dados do arquivo de save.");
        return 0; // Falha
    }
}