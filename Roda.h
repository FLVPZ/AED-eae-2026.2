#ifndef RODA_H
#define RODA_H

#include <stdio.h>
#include "Jogador.h"

int mao(tp_mesa* m) { //retorna o índice do jogador que é o Mão.
    return mao_atual(m);
}

int jogador_no_turno(tp_mesa* m, int posicao_turno) { //retorna o índice de quem joga no turno, sem alterar a ordem da mesa.
    if (posicao_turno < 0 || posicao_turno > 3) return -1;
    return m->ordem[posicao_turno];
}

void proxima_mao(tp_mesa* m) {  //avança o Mão e altera a ordem da mesa.
    girar_ordem(m);
}

void exibir_ordem_rodada(tp_mesa* m) {
    printf("ORDEM DE JOGO\n");
    for (int i = 0; i < 4; i++) {
        int idx = m->ordem[i];
        printf("%dº a jogar: %-30s | Dupla %d %s\n",
            i + 1,
            m->jogadores[idx].nome,
            m->jogadores[idx].dupla,
            (i == 0) ? "<-- [MÃO DA RODADA]" : "");
    }
}

#endif