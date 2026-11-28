#ifndef CRIATIVO_H
#define CRIATIVO_H

#include <stdio.h>
#include "STACK.h"
#include "fila.h"
#include "Jogador.h"
#include "interface.h"

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
    ui_cartas(j->mao, qtd_cartas, valor_manilha);
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

// Os tres sinais fixos do item criativo. Usado pelo terminal e pela
// interface grafica, para o gesto nunca divergir entre as duas.
static const char *criativo_gesto_texto(int opcao) {
    switch (opcao) {
        case 1: return "pisca ;)";
        case 2: return "levanta a sobrancelha ^_^";
        case 3: return "toca na mesa: toc, toc!";
        default: return NULL;
    }
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
        ui_secao("SINAIS DO PARCEIRO");
        printf("  Para %s (Dupla %d)\n", parceiro.nome, parceiro.dupla);
        printf("  [1] Piscar  [2] Sobrancelha  [3] Tocar na mesa  [0] Voltar\n  Escolha: ");
        char op = ler_opcao();

        if (op == '0') {
            ui_aviso("Sinal cancelado. Voce continua com a vez.");
            return;
        }

        const char *gesto = (op >= '1' && op <= '3') ? criativo_gesto_texto(op - '0') : NULL;
        if (gesto == NULL) {
            printf("Opcao invalida.\n");
            continue;
        }

        printf("\n[SINAL] %s %s para %s.\n", atual->nome, gesto, parceiro.nome);
        printf("%s continua com a vez.\n", atual->nome);
        ui_aviso("Sinal: %s para %s. Sua vez continua.", gesto, parceiro.nome);
        return;
    }
}

static void criativo_apresentar(void) {
    ui_titulo("BAR DO PATO  |  TRUCO PAULISTA");
    printf("  Quatro jogadores. Duas duplas. Uma conta de 12 pontos.\n");
    printf("  [1-3] Jogar carta  [T] Truco  [S] Sinal  [V] VAR\n");
    printf("  Manilhas em destaque; a de Paus e o ZAP.\n");
    printf("  Combinem os sinais antes de jogar.\n");
}

#endif
