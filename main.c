#include <stdio.h>
#include "include/STACK.h"
#include "include/fila.h"
#include "include/Jogador.h"
#include "include/Embaralhar.h"
#include "include/jogo.h"
#include "include/Roda.h"
#include "include/Partida.h"
#include "include/var.h"

int main() {

    tp_listad *v;
    v = inicializa_listad();
    
    tp_fila mesa;
    char nomes[4][NOME_MAX];

    inicializar_seed();
    inicializaFila(&mesa);

    cadastrar_jogadores(nomes);
    montar_mesa(&mesa, nomes);

    jogar_partida(&mesa, v);

    Destroi_listad(v);

    return 0;
}
