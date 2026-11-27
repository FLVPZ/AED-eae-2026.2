#include <stdio.h>
#include "include/STACK.h"
#include "include/fila.h"
#include "include/Jogador.h"
#include "include/Embaralhar.h"
#include "include/jogo.h"
#include "include/Roda.h"
#include "include/Partida.h"
#include "include/var.h"
#include "include/caderneta.h"
#include "include/mural.h"
#include "include/disco.h"

int main() {
    ui_iniciar();
    ui_limpar();

    tp_listad *v;
    v = inicializa_listad();
    ArvAVL *caderneta = criarAVL();
    if (v == NULL || caderneta == NULL) {
        fprintf(stderr, "Nao foi possivel iniciar o jogo: memoria insuficiente.\n");
        if (v != NULL) Destroi_listad(v);
        liberaABB(caderneta);
        return 1;
    }
    int carga = carregar_caderneta(caderneta);
    if (carga == DISCO_ERRO) {
        fprintf(stderr, "Nao foi possivel carregar caderneta.txt. O arquivo foi preservado.\n");
        fprintf(stderr, "Confira o formato: nome;partidas;maos;pedidos;corridos;recebidos.\n");
        Destroi_listad(v);
        liberaABB(caderneta);
        return 1;
    }
    
    tp_fila mesa;
    char nomes[4][NOME_MAX];

    inicializar_seed();
    inicializaFila(&mesa);

    criativo_apresentar();
    if (carga == DISCO_CARREGADO) {
        printf("  Caderneta recuperada: %d jogador(es).\n", totalNOsABB(caderneta));
    }
    ui_secao("QUEM SENTA NA MESA?");
    if (!cadastrar_jogadores(nomes)) {
        Destroi_listad(v);
        liberaABB(caderneta);
        return 0;
    }
    for (int i = 0; i < 4; i++) {
        if (acessar_jogador(caderneta, nomes[i]) == NULL) {
            fprintf(stderr, "Nao foi possivel cadastrar %s na caderneta.\n", nomes[i]);
            Destroi_listad(v);
            liberaABB(caderneta);
            return 1;
        }
    }
    montar_mesa(&mesa, nomes);

    ui_secao("CADERNETA DO BAR - ordem alfabetica");
    printf("  %-29s %4s %4s %5s %5s %5s\n", "JOGADOR", "PART", "MAOS", "PEDIU", "RECEB", "FUGIU");
    emOrd(caderneta);
    printf("\nCaderneta salva ao fim de cada mao; atualizacao das estatisticas pendente.\n");
    exibir_murais(caderneta);
    ui_pausar("Tudo pronto. Abrir a mesa");

    jogar_partida(&mesa, v, caderneta);

    exibir_murais(caderneta);
    Destroi_listad(v);
    liberaABB(caderneta);

    return 0;
}
