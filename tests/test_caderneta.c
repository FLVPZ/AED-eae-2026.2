#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int falhar_alocacao = 0;
static void *alocar_teste(size_t tamanho) {
    return falhar_alocacao ? NULL : malloc(tamanho);
}
#define malloc alocar_teste
#include "../include/caderneta.h"
#undef malloc

/* Confere alturas reais, balanceamento e ordem das chaves em cada operacao. */
static int verificar(struct NO *no, const char *minimo, const char *maximo) {
    if (no == NULL) return -1;
    if (minimo != NULL) assert(strcmp(no->info.nome_jogador, minimo) > 0);
    if (maximo != NULL) assert(strcmp(no->info.nome_jogador, maximo) < 0);
    int esq = verificar(no->esq, minimo, no->info.nome_jogador);
    int dir = verificar(no->dir, no->info.nome_jogador, maximo);
    assert(abs(esq - dir) <= 1);
    assert(no->alt == 1 + (esq > dir ? esq : dir));
    return no->alt;
}

static info_caderneta ficha(int numero) {
    info_caderneta info = {0};
    sprintf(info.nome_jogador, "J%04d", numero);
    info.maos_ganhas = numero;
    return info;
}

int main(void) {
    info_caderneta info = ficha(1);
    assert(inserir(NULL, info) == 0);
    assert(remover(NULL, info) == 0);
    assert(acessar_jogador(NULL, "Ana") == NULL);
    falhar_alocacao = 1;
    assert(criarAVL() == NULL);
    falhar_alocacao = 0;

    ArvAVL *raiz = criarAVL();
    assert(raiz != NULL);
    assert(acessar_jogador(raiz, NULL) == NULL);
    assert(acessar_jogador(raiz, "") == NULL);
    char longo[51];
    memset(longo, 'a', sizeof(longo) - 1);
    longo[50] = '\0';
    assert(acessar_jogador(raiz, longo) == NULL);
    assert(consultarValorAVL(raiz, NULL) == 0);
    struct NO *ana = acessar_jogador(raiz, "Ana");
    assert(ana != NULL);
    assert(ana->info.partidas_jogadas == 0 && ana->info.maos_ganhas == 0);
    assert(ana->info.trucos_pedidos == 0 && ana->info.trucos_corridos == 0);
    assert(ana->info.trucos_recebidos == 0);
    ana->info.maos_ganhas = 7;
    assert(acessar_jogador(raiz, "Ana") == ana && ana->info.maos_ganhas == 7);
    falhar_alocacao = 1;
    assert(acessar_jogador(raiz, "Bia") == NULL);
    assert(acessar_jogador(raiz, "Ana") == ana);
    falhar_alocacao = 0;
    assert(totalNOsABB(raiz) == 1);
    liberaABB(raiz);

    for (int rodada = 0; rodada < 20; rodada++) {
        int ordem[200];
        for (int i = 0; i < 200; i++) ordem[i] = i;
        srand((unsigned int)rodada);
        if (rodada >= 2) {
            for (int i = 199; i > 0; i--) {
                int j = rand() % (i + 1);
                int aux = ordem[i]; ordem[i] = ordem[j]; ordem[j] = aux;
            }
        }
        raiz = criarAVL();
        assert(raiz != NULL);
        for (int i = 0; i < 200; i++) {
            info = ficha(ordem[i]);
            assert(inserir(raiz, info) == 1);
            verificar(*raiz, NULL, NULL);
        }
        assert(totalNOsABB(raiz) == 200);
        for (int i = 0; i < 200; i++) {
            int indice = rodada % 2 ? 199 - i : i;
            info = ficha(ordem[indice]);
            assert(remover(raiz, info) == 1);
            assert(consultarValorAVL(raiz, info.nome_jogador) == 0);
            verificar(*raiz, NULL, NULL);
            assert(totalNOsABB(raiz) == 199 - i);
        }
        assert(*raiz == NULL);
        liberaABB(raiz);
    }
    puts("OK: cadastro, reuso, entradas invalidas, falta de memoria e 4000 insercoes/remocoes AVL.");
    return 0;
}
