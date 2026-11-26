#ifndef CRIATIVO_H
#define CRIATIVO_H

#include <stdio.h>
#include "STACK.h"
#include "fila.h"
#include "Jogador.h"

// Os destaques dependem da manilha já calculada pelo jogo.
// Esta função só escreve na tela: não muda a carta nem sua força.
static void criativo_destacar_manilha(tp_item carta, int valor_manilha) {
    if (valor_manilha <= 0 || carta.valor != valor_manilha) return;

    switch (carta.naipe) {
        case 'P': printf(" [MANILHA - ZAP]"); break;
        case 'C':
        case 'E':
        case 'O': printf(" [MANILHA]"); break;
    }
}

static void criativo_exibir_mao(const tp_jogador *j, int qtd_cartas,
                               int valor_manilha) {
    printf("Sua mao: ");
    for (int i = 0; i < qtd_cartas; i++) {
        tp_item carta = j->mao[i];
        printf("[%d] ", i + 1);

        switch (carta.valor) {
            case 1:  printf("As"); break;
            case 11: printf("J"); break;
            case 12: printf("Q"); break;
            case 13: printf("K"); break;
            default: printf("%d", carta.valor);
        }
        switch (carta.naipe) {
            case 'O': printf(" de Ouros"); break;
            case 'E': printf(" de Espadas"); break;
            case 'C': printf(" de Copas"); break;
            case 'P': printf(" de Paus"); break;
        }

        criativo_destacar_manilha(carta, valor_manilha);
        if (i < qtd_cartas - 1) printf(" | ");
    }
    printf("\n");
}

// A fila recebida é uma cópia. O primeiro é quem está jogando;
// o parceiro é procurado entre os demais pela dupla, mesmo após giros.
static int criativo_encontrar_parceiro(tp_fila mesa, tp_jogador *parceiro) {
    tp_jogador atual, candidato;
    if (!removeFila(&mesa, &atual)) return 0;

    while (removeFila(&mesa, &candidato)) {
        if (candidato.dupla == atual.dupla) {
            *parceiro = candidato;
            return 1;
        }
    }
    return 0;
}

// O jogador escolhe o gesto. Nenhuma carta do parceiro é consultada,
// e enviar um sinal não joga carta, não muda a aposta e não passa a vez.
static void criativo_enviar_sinal(tp_fila mesa, const tp_jogador *atual) {
    tp_jogador parceiro;
    if (!criativo_encontrar_parceiro(mesa, &parceiro)) {
        printf("Parceiro indisponivel.\n");
        return;
    }

    for (;;) {
        printf("\nSinal para %s (Dupla %d)\n", parceiro.nome, parceiro.dupla);
        printf("[1] Piscar | [2] Levantar a sobrancelha | [3] Tocar na mesa | [0] Voltar: ");
        char op = ler_opcao();
        const char *gesto;

        switch (op) {
            case '0': return;
            case '1': gesto = "pisca ;)"; break;
            case '2': gesto = "levanta a sobrancelha ^_^"; break;
            case '3': gesto = "toca na mesa: toc, toc!"; break;
            default:
                printf("Opcao invalida.\n");
                continue;
        }

        printf("\n[SINAL] %s %s para %s.\n", atual->nome, gesto, parceiro.nome);
        printf("%s continua com a vez.\n", atual->nome);
        return;
    }
}

static void criativo_apresentar(void) {
    printf("\nTRUCO NO BAR DO PATO\n");
    printf("As manilhas aparecem com [MANILHA]; a de Paus, com [MANILHA - ZAP].\n");
    printf("Na sua vez, use [S] para enviar um sinal ao parceiro.\n");
    printf("Combinem o significado dos gestos antes de jogar.\n\n");
}

#endif
