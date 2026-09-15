#ifndef JOGADOR_H
#define JOGADOR_H

#include <stdio.h>
#include <string.h>

#include "STACK.h"

#define NOME_MAX 30

typedef struct {
    char nome[NOME_MAX];
    tp_item mao[3];
    int dupla;
} tp_jogador;

typedef struct {
    tp_jogador jogadores[4];
    int ordem[4];
} tp_mesa;

void montar_mesa(tp_mesa* m, char nomes[4][NOME_MAX]) {
    int duplas[4] = { 1, 2, 1, 2 }; // parceiros sentados alternados

    for (int i = 0; i < 4; i++) {
        strcpy(m->jogadores[i].nome, nomes[i]);
        m->jogadores[i].dupla = duplas[i];
        m->ordem[i] = i;
    }
}

int mao_atual(tp_mesa* m) {
    return m->ordem[0];
}

// quem era mao vai pro fim, o proximo assume
void girar_ordem(tp_mesa* m) {
    int primeiro = m->ordem[0];
    for (int i = 0; i < 3; i++) {
        m->ordem[i] = m->ordem[i + 1];
    }
    m->ordem[3] = primeiro;
}

void cadastrar_jogadores(char nomes[4][NOME_MAX]) {
    int duplas[4] = { 1, 2, 1, 2 }; // duplas alternadas

    for (int i = 0; i < 4; i++) {
        printf("Nome do Jogador %d (Dupla %d): ", i + 1, duplas[i]);
        scanf(" %29[^\n]", nomes[i]);
    }
}   

void exibir_mesa(tp_mesa* m) {
    printf("\nMESA\n");
    for (int i = 0; i < 4; i++) {
        int idx = m->ordem[i];
        printf("%do a jogar: %-15s | Dupla %d %s\n",
            i + 1,
            m->jogadores[idx].nome,
            m->jogadores[idx].dupla,
            (i == 0) ? "(Mao)" : "");
    }
}

#endif