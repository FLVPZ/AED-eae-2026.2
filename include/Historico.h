#ifndef HISTORICO_H
#define HISTORICO_H

#include <stdio.h>
#include "STACK.h"
#include "fila.h"

#define ARQUIVO_HISTORICO "historico.txt"

static int hist_iniciado = 0;

// a primeira chamada apaga o arquivo da partida anterior
FILE *hist_abrir() {
    FILE *f = fopen(ARQUIVO_HISTORICO, hist_iniciado ? "a" : "w");
    hist_iniciado = 1;
    return f;
}

void hist_escrever_carta(FILE *f, tp_item c) {
    switch (c.valor) {
        case 1:  fprintf(f, "As"); break;
        case 11: fprintf(f, "J");  break;
        case 12: fprintf(f, "Q");  break;
        case 13: fprintf(f, "K");  break;
        default: fprintf(f, "%d", c.valor);
    }
    switch (c.naipe) {
        case 'O': fprintf(f, " de Ouros");   break;
        case 'E': fprintf(f, " de Espadas"); break;
        case 'C': fprintf(f, " de Copas");   break;
        case 'P': fprintf(f, " de Paus");    break;
    }
}

void hist_registrar_mao(int numero_mao) {
    FILE *f = hist_abrir();
    if (f == NULL) return;
    fprintf(f, "\n=== MAO %d ===\n", numero_mao);
    fclose(f);
}

void hist_registrar_rodada(int numero_mao, int rodada, char nomes[4][NOME_MAX],
                           tp_item cartas[4], int dupla_vencedora) {
    FILE *f = hist_abrir();
    if (f == NULL) return;

    fprintf(f, "Mao %d - Rodada %d\n", numero_mao, rodada);
    for (int i = 0; i < 4; i++) {
        fprintf(f, "  %s jogou ", nomes[i]);
        hist_escrever_carta(f, cartas[i]);
        fprintf(f, "\n");
    }
    if (dupla_vencedora == 0) {
        fprintf(f, "  Rodada empatada\n");
    }
    else {
        fprintf(f, "  Dupla %d levou a rodada\n", dupla_vencedora);
    }
    fclose(f);
}

#endif
