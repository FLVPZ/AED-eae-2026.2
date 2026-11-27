#ifndef RODA_H
#define RODA_H

#include "fila.h"

// a roda é a fila da mesa: quem está na frente joga agora
void passar_mao(tp_fila* f) {
    tp_jogador jogador;
    if (removeFila(f, &jogador)) {
        insereFila(f, jogador);
    }
}

void girar_roda(tp_fila* f, int n) {
    for (int i = 0; i < n; i++) {
        passar_mao(f);
    }
}

// primeiro jogador da dupla na ordem da roda, sem mexer na fila original
int jogador_da_dupla(tp_fila f, int dupla, tp_jogador* j_out) {
    tp_jogador j;
    while (removeFila(&f, &j)) {
        if (j.dupla == dupla) {
            *j_out = j;
            return 1;
        }
    }
    return 0;
}

#endif
