#ifndef MURAL_H
#define MURAL_H
#include <stdlib.h>
#include "caderneta.h"

double taxa_correu(info_caderneta info) { //calcula a taxa de fugas do truco de um fregues (usei a passagem por cópia das fichas dos jogadores)
    int total_decisoes = info.trucos_pedidos + info.trucos_corridos;
    if (total_decisoes == 0) return 0.0; //impedir divisao por 0
    return ((double)info.trucos_corridos / total_decisoes) * 100.0; //encontrei pesquisando na internet, se chama "type cast" pra forçar a divisão de int em decimal (ex: 1.0/3 = 33.33%)
}

void no_avl(struct NO* no, struct NO** vetor, int* indice) { //usa mesma lógica de andar por arvore avl, mas guarda os endereços dos nós no vetor de ponteiros (o índice é por ponteiro tb pra permitir a recursividade)
    if (no == NULL) return;
    no_avl(no->esq, vetor, indice);
    vetor[*indice] = no;
    (*indice)++;
    no_avl(no->dir, vetor, indice);
} //percorre Em Ordem (como as chaves da AVL são os nomes, já vai estar em ordem alfabética)

struct NO** mural(ArvAVL* raiz, int* total_out) { //estrutura o vetor de maos ganhas
    *total_out = totalNOsABB(raiz);
    if (*total_out <= 0) return NULL;

    struct NO** vetor = (struct NO**)malloc((*total_out) * sizeof(struct NO*));
    if (vetor == NULL) return NULL;

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
    *total_out = totalNOsABB(raiz);
    if (*total_out <= 0) return NULL;

    struct NO** vetor = (struct NO**)malloc((*total_out) * sizeof(struct NO*));
    if (vetor == NULL) return NULL;

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

#endif
