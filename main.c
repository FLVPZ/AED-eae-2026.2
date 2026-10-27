#include <stdio.h>
#include "STACK.h"
#include "fila.h"
#include "Jogador.h"
#include "Embaralhar.h"
#include "jogo.h"
#include "Roda.h"
#include "Partida.h"

int main() {
    tp_fila mesa;
    char nomes[4][NOME_MAX];

    inicializar_seed();
    inicializaFila(&mesa);

    cadastrar_jogadores(nomes);
    montar_mesa(&mesa, nomes);

    jogar_partida(&mesa);

    return 0;
}
