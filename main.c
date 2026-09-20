#include <stdio.h>
#include "STACK.h"
#include "fila.h"
#include "Jogador.h"
#include "Embaralhar.h"
#include "jogo.h"
#include "Roda.h"

int main() {
    tp_fila mesa;
    tp_stack baralho;
    char nomes[4][NOME_MAX];
    tp_item carta_vira;
    tp_item manilhas[4];

    inicializar_seed();
    inicializaFila(&mesa);

    cadastrar_jogadores(nomes);
    montar_mesa(&mesa, nomes);

    embaralhar_baralho(&baralho);

    distribuir_cartas(&baralho, &mesa);
    carta_vira = vira(&baralho);
    manilha(carta_vira, manilhas);

    exibir_mesa(mesa);

    printf("Vira: Valor %d e Naipe %c\n", carta_vira.valor, carta_vira.naipe);
    printf("Valor Manilha: %d\n\n", manilhas[0].valor);

    exibir_mao(mesa);

    passar_mao(&mesa);
    exibir_ordem_rodada(mesa);

    return 0;
}
