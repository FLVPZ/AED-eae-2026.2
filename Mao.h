#ifndef MAO_H
#define MAO_H
#include <stdio.h>
#include "STACK.h"
#include "fila.h"

int ordem_valor(tp_item carta) { // retorna a força/peso do valor da carta (4: peso 1; 5: peso 2...)
    switch(carta.valor) {
        case 4:  return 1;
        case 5:  return 2;
        case 6:  return 3;
        case 7:  return 4;
        case 12: return 5; // dama (Q)
        case 11: return 6; // valete (J)
        case 13: return 7; // rei (K)
        case 1:  return 8; // A
        case 2:  return 9; // 2
        case 3:  return 10;// 3
        default: return 0;
    }
}

void ordenar_mao(tp_jogador *j, int qtd_cartas) { //ordena as cartas em ordem crescente usando o Insertion Sort
    for (int i = 1; i < qtd_cartas; i++) {
        tp_item chave = j->mao[i];
        int k = i - 1;

        while (k >= 0 && ordem_valor(j->mao[k]) > ordem_valor(chave)) {
            j->mao[k + 1] = j->mao[k];
            k--;
        }
        j->mao[k + 1] = chave;
    }
}

void exibir_mao_jogador_atual(tp_jogador j, int qtd_cartas) { //exibe apenas as cartas que o jogador ainda possui
    printf("Sua mao: ");
    for (int i = 0; i < qtd_cartas; i++) {
        printf("[%d] ", i + 1);
        
        if (j.mao[i].valor == 1)       printf("As");
        else if (j.mao[i].valor == 11) printf("J");
        else if (j.mao[i].valor == 12) printf("Q");
        else if (j.mao[i].valor == 13) printf("K");
        else                           printf("%d", j.mao[i].valor);

        switch(j.mao[i].naipe) {
            case 'E': printf(" de Espadas "); break;
            case 'C': printf(" de Copas "); break;
            case 'O': printf(" de Ouros "); break;
            case 'P': printf(" de Paus "); break;
        }
        if (i < qtd_cartas - 1) printf("| ");
    }
    printf("\n");
}

int jogar_carta(tp_jogador *j, int *qtd_cartas, int escolha_usuario, tp_item *carta_jogada) { // remove a carta da mão e ajusta o vetor
    int indice = escolha_usuario - 1;

    if (indice < 0 || indice >= *qtd_cartas) {
        return 0;
    }

    *carta_jogada = j->mao[indice]; //faz uma cópia da carta escolhida para usar na mesa

    for (int i = indice; i < *qtd_cartas - 1; i++) { //desloca as cartas que restaram
        j->mao[i] = j->mao[i + 1];
    }

    //atualiza a contagem de cartas que o jogador tem
    (*qtd_cartas)--;
    return 1;
}

#endif
