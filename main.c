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

int main() {

    tp_listad *v;
    v = inicializa_listad();
    ArvAVL *caderneta = criarAVL();
    if (v == NULL || caderneta == NULL) {
        fprintf(stderr, "Nao foi possivel iniciar o jogo: memoria insuficiente.\n");
        if (v != NULL) Destroi_listad(v);
        liberaABB(caderneta);
        return 1;
    }
    
    tp_fila mesa;
    char nomes[4][NOME_MAX];

    inicializar_seed();
    inicializaFila(&mesa);

    criativo_apresentar();
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

    printf("\nCADERNETA DO BAR - ordem alfabetica\n");
    emOrd(caderneta);
    printf("\nEstatisticas ainda nao sao atualizadas nem salvas nesta versao.\n");
    exibir_murais(caderneta);

    jogar_partida(&mesa, v);

    exibir_murais(caderneta);
    Destroi_listad(v);
    liberaABB(caderneta);

    return 0;
}
