#ifndef APOSTA_H
#define APOSTA_H

#include <stdio.h>
#include "fila.h"
#include "Jogador.h"
#include "Roda.h"
#include "interface.h"
#include "estatisticas.h"
#include "var.h"
#include "disco.h"

typedef struct { //guarda as infos da aposta
    int valor; //quanto a mao vale agora
    int pedido; //valor que esta sendo pedido
    int dupla_pediu; //dupla que fez o pedido
    int ultima_dupla; //ultima dupla que pediu aumento
    int encerrada; //1 = alguem correu; 0 = continua
} tp_aposta;

#define APOSTA_CONTINUA 0
#define APOSTA_CORRIDO 1

void aposta_iniciar(tp_aposta *a) { //comeca uma nova mao com 1 ponto
    a->valor = 1;
    a->pedido = 0;
    a->dupla_pediu = 0;
    a->ultima_dupla = 0;
    a->encerrada = 0;
}

int proximo_valor(int valor) { //identifica proximo valor da aposta
    if (valor == 1)
        return 3;
    if (valor == 3)
        return 6;
    if (valor == 6)
        return 12;
    return 0;
}

int pode_pedir(tp_aposta a, int dupla) { //verifica se pode pedir aumento
    if (dupla != 1 && dupla != 2)
        return 0;
    if (a.encerrada == 1)
        return 0;
    if (a.pedido != 0)
        return 0;
    if (a.valor == 12)
        return 0;
    if (a.ultima_dupla == dupla)
        return 0;
    return 1;
}

int pedir_aposta(tp_aposta *a, int dupla) { //jogador pede truco,6 ou 12
    if (pode_pedir(*a, dupla) == 0)
        return 0;
    a->pedido = proximo_valor(a->valor);
    a->dupla_pediu = dupla;
    return 1;
}

int aceitar_aposta(tp_aposta *a) { //dupla adversaria aceita o pedido
    a->valor = a->pedido;
    a->ultima_dupla = a->dupla_pediu;
    a->pedido = 0;
    a->dupla_pediu = 0;
    return 1;
}

int aumentar_aposta(tp_aposta *a, int dupla) { //dupla adversaria aumenta o pedido
    a->valor = a->pedido;
    a->pedido = proximo_valor(a->valor); //faz o proximo pedido
    a->dupla_pediu = dupla;
    return 1;
}

int correr_aposta(tp_aposta *a, int *vencedora, int *pontos) { //uma dupla desiste da aposta
    *vencedora = a->dupla_pediu;
    *pontos = a->valor; //ganha o valor anterior ao pedido
    a->encerrada = 1;
    a->pedido = 0;
    return 1;
}

#define TRUCO_ACEITO 0
#define TRUCO_CORRIDO 1

// nome do pedido
const char *nome_pedido(int valor) {
    switch (valor) {
        case 3:  return "TRUCO";
        case 6:  return "SEIS";
        case 12: return "DOZE";
        default: return "";
    }
}

// pede truco/seis/doze e a outra dupla responde até alguém aceitar ou correr
void aposta_salvar_evento(ArvAVL *caderneta, const char *pediu, int dupla,
                          const char *respondeu, int valor, char resposta) {
    if (!salvar_evento_truco(caderneta, pediu, dupla, respondeu, valor, resposta)) {
        fprintf(stderr, "\nFalha ao salvar a negociacao:\n%s\nPasta dos dados: %s\n",
                disco_ultimo_erro, disco_diretorio);
        ui_pausar("Conferir a falha de gravacao e continuar");
    }
}

int negociar_truco(tp_fila mesa, tp_aposta *a, tp_jogador j,
                   int *vencedora, int *pontos, ArvAVL *caderneta, tp_item_VAR *registro) {

    if (!pedir_aposta(a, j.dupla)) return TRUCO_ACEITO;
    registrar_truco_pedido(caderneta, j.nome);
    char pediu_nome[NOME_MAX];
    strcpy(pediu_nome, j.nome);
    int pedido_registrado = 0;
    ui_secao("PEDIDO NA MESA");
    printf("\n%s (Dupla %d) pede %s\n", j.nome, j.dupla, nome_pedido(a->pedido));

    while (1) {
        int dupla_resp = (a->dupla_pediu == 1) ? 2 : 1;
        int novo = proximo_valor(a->pedido);
        tp_jogador resp;
        jogador_da_dupla(mesa, dupla_resp, &resp);
        if (pedido_registrado != a->pedido) {
            registrar_truco_recebido(caderneta, resp.nome);
            pedido_registrado = a->pedido;
            aposta_salvar_evento(caderneta, pediu_nome, a->dupla_pediu, resp.nome, a->pedido, 'P');
        }

        ui_limpar();
        ui_titulo("PEDIDO NA MESA");
        printf("%s (Dupla %d) pediu %s.\n", pediu_nome, a->dupla_pediu, nome_pedido(a->pedido));
        printf("  Resposta de %s (Dupla %d)\n", resp.nome, resp.dupla);
        printf("  %s: a mao passa a valer %d ponto(s).\n", nome_pedido(a->pedido), a->pedido);
        printf("  [A] Aceitar  [C] Correr");
        if (novo != 0) {
            printf("  [T] Pedir %s", nome_pedido(novo));
        }
        printf("\n  Sua escolha: ");
        fflush(stdout);

        char op = ler_opcao();
        if (op == 'A') {
            registrar_negociacao_var(registro, pediu_nome, a->dupla_pediu, resp.nome, a->pedido, op);
            aposta_salvar_evento(caderneta, pediu_nome, a->dupla_pediu, resp.nome, a->pedido, op);
            aceitar_aposta(a);
            printf("%s aceitou, a mao vale %d\n", resp.nome, a->valor);
            ui_aviso("%s aceitou. A mao vale %d ponto(s).", resp.nome, a->valor);
            return TRUCO_ACEITO;
        }
        else if (op == 'C') {
            registrar_negociacao_var(registro, pediu_nome, a->dupla_pediu, resp.nome, a->pedido, op);
            registrar_truco_corrido(caderneta, resp.nome);
            aposta_salvar_evento(caderneta, pediu_nome, a->dupla_pediu, resp.nome, a->pedido, op);
            correr_aposta(a, vencedora, pontos);
            printf("%s correu, Dupla %d leva %d ponto(s)\n", resp.nome, *vencedora, *pontos);
            ui_aviso("%s correu. Dupla %d leva %d ponto(s).", resp.nome, *vencedora, *pontos);
            return TRUCO_CORRIDO;
        }
        else if (op == 'T' && novo != 0) {
            registrar_negociacao_var(registro, pediu_nome, a->dupla_pediu, resp.nome, a->pedido, op);
            registrar_truco_pedido(caderneta, resp.nome);
            aposta_salvar_evento(caderneta, pediu_nome, a->dupla_pediu, resp.nome, a->pedido, op);
            strcpy(pediu_nome, resp.nome);
            aumentar_aposta(a, dupla_resp);
            printf("\n%s (Dupla %d) pede %s\n", resp.nome, resp.dupla, nome_pedido(a->pedido));
        }
        else {
            printf("Opcao invalida.\n");
        }
    }
}

#endif
