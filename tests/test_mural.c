#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../include/caderneta.h"

/* Simula falta de memoria somente nas alocacoes dos vetores do mural. */
static int falhar_alocacao = 0;
static void *alocar_mural(size_t tamanho) {
    return falhar_alocacao ? NULL : malloc(tamanho);
}
#define malloc alocar_mural
#include "../include/mural.h"
#undef malloc

static void conferir_ordem(struct NO **vetor, int total,
                           const char *esperados[4]) {
    assert(vetor != NULL);
    assert(total == 4);
    for (int i = 0; i < total; i++) {
        assert(strcmp(vetor[i]->info.nome_jogador, esperados[i]) == 0);
    }
}

int main(void) {
    info_caderneta ficha = {0};
    assert(taxa_correu(ficha) == 0.0);
    ficha.trucos_recebidos = 3;
    ficha.trucos_corridos = 1;
    assert(fabs(taxa_correu(ficha) - 100.0 / 3.0) < 0.000001);
    ficha.trucos_pedidos = 999;
    assert(fabs(taxa_correu(ficha) - 100.0 / 3.0) < 0.000001);
    ficha.trucos_corridos = 3;
    assert(taxa_correu(ficha) == 100.0);
    ficha.trucos_corridos = 0;
    assert(taxa_correu(ficha) == 0.0);

    ArvAVL *raiz = criarAVL();
    assert(raiz != NULL);
    int total = 99;
    assert(mural(raiz, &total) == NULL && total == 0);
    total = 99;
    assert(mural_vergonha(raiz, &total) == NULL && total == 0);
    assert(mural(NULL, &total) == NULL && total == 0);
    assert(mural_vergonha(NULL, &total) == NULL && total == 0);

    char nomes[4][50] = {"Ana", "Bia", "Caio", "Davi"};
    int ganhos[4] = {5, 1, 1, 3};
    int recebidos[4] = {4, 2, 0, 8};
    int corridos[4] = {1, 2, 0, 2};
    struct NO *jogadores[4];
    struct NO originais[4];
    for (int i = 0; i < 4; i++) {
        jogadores[i] = acessar_jogador(raiz, nomes[i]);
        assert(jogadores[i] != NULL);
        assert(jogadores[i]->info.trucos_recebidos == 0);
        jogadores[i]->info.maos_ganhas = ganhos[i];
        jogadores[i]->info.trucos_recebidos = recebidos[i];
        jogadores[i]->info.trucos_corridos = corridos[i];
    }
    for (int i = 0; i < 4; i++) originais[i] = *jogadores[i];
    struct NO *raiz_original = *raiz;

    const char *ordem_ganhos[4] = {"Bia", "Caio", "Davi", "Ana"};
    struct NO **ranking = mural(raiz, &total);
    conferir_ordem(ranking, total, ordem_ganhos);
    free(ranking); /* Os nos continuam pertencendo a AVL. */

    const char *ordem_fuga[4] = {"Bia", "Ana", "Davi", "Caio"};
    ranking = mural_vergonha(raiz, &total);
    conferir_ordem(ranking, total, ordem_fuga);
    free(ranking);

    assert(mural(raiz, NULL) == NULL);
    assert(mural_vergonha(raiz, NULL) == NULL);
    falhar_alocacao = 1;
    total = 99;
    assert(mural(raiz, &total) == NULL && total == 0);
    total = 99;
    assert(mural_vergonha(raiz, &total) == NULL && total == 0);
    falhar_alocacao = 0;

    assert(*raiz == raiz_original);
    for (int i = 0; i < 4; i++) {
        assert(acessar_jogador(raiz, nomes[i]) == jogadores[i]);
        assert(memcmp(&jogadores[i]->info, &originais[i].info,
                      sizeof(info_caderneta)) == 0);
        assert(jogadores[i]->esq == originais[i].esq);
        assert(jogadores[i]->dir == originais[i].dir);
        assert(jogadores[i]->alt == originais[i].alt);
    }
    liberaABB(raiz);
    puts("OK: taxa, rankings, empates, arvore vazia, falta de memoria e preservacao da AVL.");
    return 0;
}
