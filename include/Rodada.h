#ifndef RODADA_H
#define RODADA_H

#include "STACK.h"
#include "jogo.h"

// força da carta: manilha ganha das comuns e desempata pelo naipe
int forca_carta(tp_item c, int valor_manilha) {
    if (c.valor == valor_manilha) {
        return 100 + forca_naipe(c.naipe);
    }
    return forca_valor(c.valor);
}

// retorna a posição de quem ganhou a rodada (-1 se empatou) e grava a dupla em *dupla_rodada
int vencedor_da_rodada(tp_item cartas[4], int duplas[4], int valor_manilha, int *dupla_rodada) {
    int melhor = 0;

    for (int i = 1; i < 4; i++) {
        if (forca_carta(cartas[i], valor_manilha) > forca_carta(cartas[melhor], valor_manilha)) {
            melhor = i;
        }
    }

    int forca_max = forca_carta(cartas[melhor], valor_manilha);
    for (int i = 0; i < 4; i++) {
        if (i != melhor && forca_carta(cartas[i], valor_manilha) == forca_max
            && duplas[i] != duplas[melhor]) {
            *dupla_rodada = 0;
            return -1;
        }
    }

    *dupla_rodada = duplas[melhor];
    return melhor;
}

// retorna a dupla que ganhou a mão ou 0 se ainda não foi decidida
int vencedor_da_mao(int resultados[3], int n_rodadas, int dupla_primeiro) {
    if (n_rodadas < 2) {
        return 0;
    }

    int vitorias[3] = { 0, 0, 0 };
    for (int i = 0; i < n_rodadas; i++) {
        vitorias[resultados[i]]++;
    }

    if (n_rodadas == 2) {
        if (resultados[0] == 0 && resultados[1] != 0) return resultados[1]; // empatou a 1ª: a 2ª decide
        if (resultados[0] != 0 && resultados[1] == 0) return resultados[0]; // empatou a 2ª: vale a 1ª
        if (resultados[0] != 0 && resultados[0] == resultados[1]) return resultados[0]; // 2 a 0
        return 0;
    }

    // 3 rodadas jogadas
    if (vitorias[1] > vitorias[2]) return 1;
    if (vitorias[2] > vitorias[1]) return 2;
    if (resultados[0] != 0) return resultados[0]; // 1 a 1: vale quem levou a 1ª
    return dupla_primeiro;                         // tudo empatado: dupla de quem abriu a mão
}

#endif
