#include "STACK.h"
#ifndef Jogador_h
#define Jogador_h
#include <string.h>

#define NOME_MAX 30

typedef struct{
    char nome[NOME_MAX];
    tp_item mao[3];
    int dupla;
} tp_jogador;

typedef struct{
    tp_jogador jogadores[4];
    int ordem[4];
} tp_mesa;

void montar_mesa(tp_mesa *m, char nomes[4][NOME_MAX]){
    int duplas[4] = {1, 2, 1, 2}; // parceiros sentados alternados

    for(int i=0; i<4; i++){
        strcpy(m->jogadores[i].nome, nomes[i]);
        m->jogadores[i].dupla = duplas[i];
        m->ordem[i] = i;
    }
}

int quem_e_mao(tp_mesa *m){
    return m->ordem[0];
}

// quem era mao vai pro fim, o proximo assume
void girar_ordem(tp_mesa *m){
    int primeiro = m->ordem[0];
    for(int i=0; i<3; i++){
        m->ordem[i] = m->ordem[i+1];
    }
    m->ordem[3] = primeiro;
}

#endif
