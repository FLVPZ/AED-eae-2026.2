#ifndef MURAL_H
#define MURAL_H
#include <stdlib.h>
#include "caderneta.h"

// Percentual de pedidos recebidos que o jogador respondeu correndo.
// Cada proposta (truco/seis/doze) conta para quem efetivamente a respondeu.
// A integracao deve registrar o recebimento antes de aceitar, aumentar ou correr;
// trucos_corridos deve ser incrementado para o mesmo jogador quando ele corre.
double taxa_correu(info_caderneta info) {
    if (info.trucos_recebidos <= 0) return 0.0;
    return 100.0 * info.trucos_corridos / info.trucos_recebidos;
}

void no_avl(struct NO* no, struct NO** vetor, int* indice) { //usa mesma lógica de andar por arvore avl, mas guarda os endereços dos nós no vetor de ponteiros (o índice é por ponteiro tb pra permitir a recursividade)
    if (no == NULL) return;
    no_avl(no->esq, vetor, indice);
    vetor[*indice] = no;
    (*indice)++;
    no_avl(no->dir, vetor, indice);
} //percorre Em Ordem (como as chaves da AVL são os nomes, já vai estar em ordem alfabética)

// Os dois murais devolvem um vetor de ponteiros: o chamador libera somente
// o vetor com free(), nunca os nos da AVL. Falha ou vazio: NULL e total zero.
// O percurso custa O(n); o insertion sort, O(n^2) no pior caso.
// Empates mantem a ordem alfabetica produzida pelo percurso em ordem.
struct NO** mural(ArvAVL* raiz, int* total_out) { //estrutura o vetor de maos ganhas
    if (total_out == NULL) return NULL;
    *total_out = totalNOsABB(raiz);
    if (*total_out <= 0) return NULL;

    struct NO** vetor = (struct NO**)malloc((*total_out) * sizeof(struct NO*));
    if (vetor == NULL) {
        *total_out = 0;
        return NULL;
    }

    int idx = 0;
    no_avl(*raiz, vetor, &idx);

    for (int i = 1; i < *total_out; i++) { // insertion sort para ordenar os ponteiros
        struct NO* chave = vetor[i];
        int j = i - 1;
        while (j >= 0 && vetor[j]->info.maos_ganhas > chave->info.maos_ganhas) {
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = chave;
    }

    return vetor;
}

struct NO** mural_vergonha(ArvAVL* raiz, int* total_out) { //exatamente a mesma lógica do mural padrão, mas ordena o vetor de fuga
    if (total_out == NULL) return NULL;
    *total_out = totalNOsABB(raiz);
    if (*total_out <= 0) return NULL;

    struct NO** vetor = (struct NO**)malloc((*total_out) * sizeof(struct NO*));
    if (vetor == NULL) {
        *total_out = 0;
        return NULL;
    }

    int idx = 0;
    no_avl(*raiz, vetor, &idx);
  
    for (int i = 1; i < *total_out; i++) { // insertion sort decrescente no while pra maior taxa de corrida ficar no topo
        struct NO* chave = vetor[i];
        int j = i - 1;
        while (j >= 0 && taxa_correu(vetor[j]->info) < taxa_correu(chave->info)) {
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = chave;
    }

    return vetor;
}

// A exibicao aproveita os vetores de Diego e libera somente esses vetores.
void exibir_murais(ArvAVL* raiz) {
    if (estah_vaziaABB(raiz)) {
        printf("\nCaderneta vazia: nenhum jogador para exibir nos murais.\n");
        return;
    }

    int total = 0;
    struct NO** ranking = mural(raiz, &total);
    printf("\nMURAL DO BAR - maos ganhas em ordem crescente\n");
    if (ranking == NULL) {
        printf("Nao foi possivel montar o mural: memoria insuficiente.\n");
    } else {
        for (int i = 0; i < total; i++) {
            printf("%s: %d mao(s) ganha(s)\n", ranking[i]->info.nome_jogador,
                   ranking[i]->info.maos_ganhas);
        }
        free(ranking);
    }

    ranking = mural_vergonha(raiz, &total);
    printf("\nMURAL DA VERGONHA - taxa de fuga em ordem decrescente\n");
    if (ranking == NULL) {
        printf("Nao foi possivel montar o mural: memoria insuficiente.\n");
    } else {
        for (int i = 0; i < total; i++) {
            info_caderneta info = ranking[i]->info;
            if (info.trucos_recebidos == 0) {
                printf("%s: sem pedidos recebidos\n", info.nome_jogador);
            } else {
                printf("%s: %.2f%% (%d fuga(s) / %d pedido(s) recebido(s))\n",
                       info.nome_jogador, taxa_correu(info), info.trucos_corridos,
                       info.trucos_recebidos);
            }
        }
        free(ranking);
    }
}

#endif
