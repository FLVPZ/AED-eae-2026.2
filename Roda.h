#ifndef RODA_H
#define RODA_H

#include <stdio.h>
#include "Jogador.h"


void passar_mao(tp_fila* f) {
    tp_jogador jogador;
    if (removeFila(f, &jogador)) {
        insereFila(f, jogador);
    }
}

void exibir_ordem_rodada(tp_fila f) {
    printf("ORDEM DE JOGO\n");
    tp_jogador jogador;
    for (int i = 0; i < 4; i++) {
        removeFila(&f, &jogador);
        printf("%do a jogar: %-30s | Dupla %d %s\n",
            i + 1,
            jogador.nome,
            jogador.dupla,
            (i == 0) ? "<-- [MAO DA RODADA]" : "");
    }
}

#endif
