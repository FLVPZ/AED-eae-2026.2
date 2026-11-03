#ifndef PARTIDA_H
#define PARTIDA_H

#include "var.h"
#include <stdio.h>
#include <ctype.h>
#include "STACK.h"
#include "fila.h"
#include "Jogador.h"
#include "Embaralhar.h"
#include "jogo.h"
#include "Mao.h"
#include "Roda.h"
#include "APOSTA.h"

#define PONTOS_VITORIA 12

typedef struct {
    int numero;
    tp_item vira;
    int valor_manilha;
    tp_aposta aposta;
    int giro;            // Posições que a roda girou dentro desta mão
    int resultados[3];   // Dupla que levou cada rodada (0 = empate)
    int n_rodadas;
    int dupla_vencedora;
    int pontos;
} tp_mao_atual;

// interface entre o vetor de rodadas e a função juiz do jogo.h
int vencedor_da_rodada(tp_item cartas[4], int duplas[4], int valor_manilha, int *dupla_vencedora) {
    tp_vencedor arr[4];
    tp_item manilhas[4] = {
        {valor_manilha, 'O'}, {valor_manilha, 'E'}, 
        {valor_manilha, 'C'}, {valor_manilha, 'P'}
    };

    for (int i = 0; i < 4; i++) {
        arr[i].carta = cartas[i];
        arr[i].jogador = i + 1;
    }

    int jog_venc = vencedor_rodada(arr, manilhas);
    if (jog_venc == 0) {
        *dupla_vencedora = 0;
        return -1; // Empate
    }

    int idx = jog_venc - 1;
    *dupla_vencedora = duplas[idx];
    return idx;
}

// determina a dupla vencedora da mão
int vencedor_da_mao(int resultados[3], int n_rodadas, int dupla_mao) {
    int v1 = 0, v2 = 0;
    for (int i = 0; i < n_rodadas; i++) {
        if (resultados[i] == 1) v1++;
        else if (resultados[i] == 2) v2++;
    }

    if (v1 >= 2) return 1;
    if (v2 >= 2) return 2;

    // critérios de desempate
    if (n_rodadas >= 1 && resultados[0] == 0) {
        if (n_rodadas >= 2 && resultados[1] != 0) return resultados[1];
        if (n_rodadas == 3 && resultados[2] != 0) return resultados[2];
        if (n_rodadas == 3 && resultados[1] == 0 && resultados[2] == 0) return dupla_mao;
    }

    if (n_rodadas >= 2 && resultados[1] == 0) {
        if (resultados[0] != 0) return resultados[0];
    }

    if (n_rodadas == 3 && resultados[2] == 0) {
        if (resultados[0] != 0) return resultados[0];
    }

    return 0; // Mão em andamento
}

void mostrar_valor(int valor) {
    switch (valor) {
        case 1:  printf("As"); break;
        case 11: printf("J");  break;
        case 12: printf("Q");  break;
        case 13: printf("K");  break;
        default: printf("%d", valor);
    }
}

void mostrar_carta(tp_item c) {
    mostrar_valor(c.valor);
    switch (c.naipe) {
        case 'O': printf(" de Ouros");   break;
        case 'E': printf(" de Espadas"); break;
        case 'C': printf(" de Copas");   break;
        case 'P': printf(" de Paus");    break;
    }
}

void mostrar_cabecalho(const int placar[3], tp_mao_atual *m) {
    printf("\nMao %d | Dupla 1: %d x Dupla 2: %d | vale %d | Vira: ",
           m->numero, placar[1], placar[2], m->aposta.valor);
    mostrar_carta(m->vira);
    printf(" | Manilha: ");
    mostrar_valor(m->valor_manilha);
    printf("\n");
}

void mostrar_cartas_jogadas(char nomes[4][NOME_MAX], tp_item cartas[4], int qtd) {
    for (int i = 0; i < qtd; i++) {
        printf("  %s jogou ", nomes[i]);
        mostrar_carta(cartas[i]);
        printf("\n");
    }
}

int jogar_rodada(tp_fila *mesa, tp_mao_atual *m, const int placar[3], tp_listad *v) {
    tp_item_VAR item_var;
    inicializa_variavel_var(&item_var);

    tp_item cartas[4];
    int duplas[4];
    char nomes[4][NOME_MAX];
    int cartas_na_mao = 3 - m->n_rodadas;

    printf("\n    Rodada %d    \n", m->n_rodadas + 1);

    for (int i = 0; i < 4; i++) {
        tp_jogador j;
        posicao_do_jogador(*mesa, 0, &j);

        int escolhida = 0;
        while (escolhida == 0) {
            mostrar_cabecalho(placar, m);
            mostrar_cartas_jogadas(nomes, cartas, i);
            printf("Vez de %s (Dupla %d)\n", j.nome, j.dupla);
            
            // Requisito da Unidade: Ordenação da mão via Insertion Sort antes de exibir
            ordenar_mao(&j, cartas_na_mao);
            exibir_mao_jogador_atual(j, cartas_na_mao);

            printf("Carta [1-%d]", cartas_na_mao);
            if (pode_pedir(m->aposta, j.dupla)) {
                printf(" ou [T] pedir %s", nome_pedido(proximo_valor(m->aposta.valor)));
            }
            printf(" ou [V] ver o VAR: ");

            char op = ler_opcao();
            if (op >= '1' && op <= '0' + cartas_na_mao) {
                escolhida = op - '0';
            }
            else if (op == 'T' && pode_pedir(m->aposta, j.dupla)) {
                int r = negociar_truco(*mesa, &m->aposta, j,
                                       &m->dupla_vencedora, &m->pontos);
                if (r == APOSTA_CORRIDO) {
                    for (int k = i; k < 4; k++) {
                        cartas[k].valor = -1;
                        cartas[k].naipe = 'A';
                    }
                    mao_var(&item_var, m->numero);
                    rodada_var(&item_var, m->n_rodadas + 1);
                    dupla_vencedora_var(&item_var, m->dupla_vencedora);
                    cartas_var(&item_var, cartas, m->valor_manilha);
                    aposta_var(&item_var, m->aposta.dupla_pediu, proximo_valor(m->aposta.valor), 1);
                    adiciona_var(&item_var, v);

                    girar_roda(mesa, (4 - i) % 4);
                    return 1;
                }
            }
            else if (op == 'V') {
                ver_var(v);
            }
            else {
                printf("Opcao invalida.\n");
            }
        }

        int qtd = cartas_na_mao;
        jogar_carta(&j, &qtd, escolhida, &cartas[i]);
        strcpy(nomes[i], j.nome);
        duplas[i] = j.dupla;

        tp_jogador descarte;
        removeFila(mesa, &descarte);
        insereFila(mesa, j);
    }

    int dupla_rodada;
    int pos = vencedor_da_rodada(cartas, duplas, m->valor_manilha, &dupla_rodada);

    printf("\nCartas da rodada:\n");
    mostrar_cartas_jogadas(nomes, cartas, 4);
    if (pos < 0) {
        printf("Rodada empatada.\n");
    }
    else {
        printf("Dupla %d leva a rodada com a carta de %s.\n", dupla_rodada, nomes[pos]);
        girar_roda(mesa, pos);
        m->giro += pos;
    }

    // Registro na Lista Duplamente Encadeada do VAR
    mao_var(&item_var, m->numero);
    rodada_var(&item_var, m->n_rodadas + 1);
    dupla_vencedora_var (&item_var, dupla_rodada);
    cartas_var(&item_var, cartas, m->valor_manilha);
    if (m->aposta.valor == 1) {
        aposta_var(&item_var, -1, -1, -1);
    }
    else {
        aposta_var(&item_var, m->aposta.ultima_dupla, m->aposta.valor, 0);
    }
    adiciona_var(&item_var, v);

    m->resultados[m->n_rodadas] = dupla_rodada;
    m->n_rodadas++;
    return 0;
}

void jogar_mao(tp_fila *mesa, int placar[3], int numero_mao, tp_listad *v) {
    tp_stack baralho;
    tp_item manilhas[4];
    tp_mao_atual m;

    m.numero = numero_mao;
    m.giro = 0;
    m.n_rodadas = 0;
    m.dupla_vencedora = 0;
    m.pontos = 0;
    aposta_iniciar(&m.aposta);

    embaralhar_baralho(&baralho);
    distribuir_cartas(&baralho, mesa);
    m.vira = vira(&baralho);
    manilha(m.vira, manilhas);
    m.valor_manilha = manilhas[0].valor;

    printf("\n    MAO %d    \n", numero_mao);
    exibir_mesa(*mesa);
    exibir_ordem_rodada(*mesa);

    tp_jogador primeiro;
    posicao_do_jogador(*mesa, 0, &primeiro);

    while (m.dupla_vencedora == 0) {
        if (jogar_rodada(mesa, &m, placar, v)) break;

        int venc = vencedor_da_mao(m.resultados, m.n_rodadas, primeiro.dupla);
        if (venc != 0) {
            m.dupla_vencedora = venc;
            m.pontos = m.aposta.valor;
        }
    }

    girar_roda(mesa, (4 - (m.giro % 4)) % 4);

    placar[m.dupla_vencedora] += m.pontos;
    printf("\nDupla %d leva a mao e faz %d ponto(s).\n", m.dupla_vencedora, m.pontos);
}

void jogar_partida(tp_fila *mesa, tp_listad *v) {
    int placar[3] = { 0, 0, 0 };
    int numero_mao = 0;

    while (placar[1] < PONTOS_VITORIA && placar[2] < PONTOS_VITORIA) {
        numero_mao++;
        jogar_mao(mesa, placar, numero_mao, v);
        passar_mao(mesa);
    }

    int vencedora = (placar[1] >= PONTOS_VITORIA) ? 1 : 2;
    int pontos1 = placar[1] > PONTOS_VITORIA ? PONTOS_VITORIA : placar[1];
    int pontos2 = placar[2] > PONTOS_VITORIA ? PONTOS_VITORIA : placar[2];

    printf("\nFIM DE PARTIDA! Dupla %d venceu: ", vencedora);
    tp_fila copia = *mesa;
    tp_jogador j;
    int primeiro_nome = 1;
    while (removeFila(&copia, &j)) {
        if (j.dupla == vencedora) {
            printf("%s%s", primeiro_nome ? "" : " e ", j.nome);
            primeiro_nome = 0;
        }
    }
    printf("\nPlacar final: Dupla 1 = %d x Dupla 2 = %d\n", pontos1, pontos2);
    
    ver_var(v);
}

#endif
