#ifndef TRUCO_H
#define TRUCO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fila.h"
#include "Jogador.h"
#include "Roda.h"
#include "APOSTA.h"

#define TRUCO_ACEITO 0
#define TRUCO_CORRIDO 1

// lê a opção digitada
char ler_opcao() {
    char linha[64];

    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        exit(0); // teclado fechado
    }
    if (strchr(linha, '\n') == NULL) {
        limpar_buffer();
    }

    int i = 0;
    while (linha[i] == ' ' || linha[i] == '\t') {
        i++;
    }
    return (char)toupper((unsigned char)linha[i]);
}

// nome do pedido
const char *nome_pedido(int valor) {
    switch (valor) {
        case 3:  return "TRUCO";
        case 6:  return "SEIS";
        case 12: return "DOZE";
        default: return "";
    }
}

// pede truco/seis/doze e a outra dupla responde até alguém aceitar ou correr
int negociar_truco(tp_fila mesa, tp_aposta *a, tp_jogador j, int numero_mao,
                   int *vencedora, int *pontos) {
    (void)numero_mao;

    pedir_aposta(a, j.dupla);
    printf("\n%s (Dupla %d) pede %s\n", j.nome, j.dupla, nome_pedido(a->pedido));

    while (1) {
        int dupla_resp = (a->dupla_pediu == 1) ? 2 : 1;
        int novo = proximo_valor(a->pedido);
        tp_jogador resp;
        jogador_da_dupla(mesa, dupla_resp, &resp);

        printf("Vez de %s (Dupla %d) responder, %s vale %d. [A] aceitar | [C] correr",
               resp.nome, resp.dupla, nome_pedido(a->pedido), a->pedido);
        if (novo != 0) {
            printf(" | [T] pedir %s", nome_pedido(novo));
        }
        printf(": ");

        char op = ler_opcao();
        if (op == 'A') {
            aceitar_aposta(a, dupla_resp);
            printf("%s aceitou, a mao vale %d\n", resp.nome, a->valor);
            return TRUCO_ACEITO;
        }
        else if (op == 'C') {
            correr_aposta(a, dupla_resp, vencedora, pontos);
            printf("%s correu, Dupla %d leva %d ponto(s)\n", resp.nome, *vencedora, *pontos);
            return TRUCO_CORRIDO;
        }
        else if (op == 'T' && novo != 0) {
            aumentar_aposta(a, dupla_resp);
            printf("\n%s (Dupla %d) pede %s\n", resp.nome, resp.dupla, nome_pedido(a->pedido));
        }
        else {
            printf("Opcao invalida.\n");
        }
    }
}

#endif
