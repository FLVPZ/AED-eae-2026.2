/* motor_web.c
 * --------------------------------------------------------------------------
 * Adaptador para a interface grafica do navegador (WebAssembly).
 *
 * Este arquivo NAO reimplementa as regras do Truco. Ele inclui e chama
 * exatamente as mesmas estruturas e funcoes do motor em C usadas pela
 * versao de terminal (main.c / include/Partida.h): a pilha do baralho
 * (STACK.h/baralho.h/Embaralhar.h), a fila dos jogadores (fila.h/Roda.h),
 * a logica de rodada e mao (jogo.h), a negociacao de aposta (APOSTA.h),
 * a arvore AVL da caderneta (caderneta.h/estatisticas.h) e a lista
 * duplamente encadeada do VAR (listade.h/var.h).
 *
 * A diferenca e apenas de "frente": em vez de um loop bloqueante lendo
 * teclado e escrevendo com printf (como em Partida.h), este arquivo expoe
 * um pequeno conjunto de funcoes que a pagina HTML chama a cada clique, e
 * devolve o estado atual do jogo como texto JSON para a pagina desenhar.
 *
 * O desenho (arquitetura "motor separado da apresentacao, dirigido por
 * eventos") e o mesmo adotado no PR #8 (gui.c/motor_grafico.h, revertido
 * no PR #9 só por ficar visualmente simples demais); aqui o motor e o
 * mesmo, so a saida muda de uma janela nativa do Windows (GDI) para o
 * WebAssembly + HTML/CSS/JS, o que permite rodar em qualquer sistema sem
 * instalar compilador.
 *
 * Persistencia em disco (caderneta.txt / historico.txt, include/disco.h)
 * continua sendo uma funcionalidade exclusiva da versao de terminal: um
 * modulo WebAssembly rodando no navegador nao tem acesso ao sistema de
 * arquivos do jogador, entao esta build NAO chama fopen/salvar_caderneta.
 * As estatisticas da caderneta, aqui, vivem em memoria durante a sessao
 * do navegador (mesma AVL, sem gravacao em arquivo).
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/STACK.h"
#include "../include/baralho.h"
#include "../include/Embaralhar.h"
#include "../include/fila.h"
#include "../include/Jogador.h"
#include "../include/Mao.h"
#include "../include/Roda.h"
#include "../include/jogo.h"
#include "../include/listade.h"
#include "../include/var.h"
#include "../include/caderneta.h"
#include "../include/estatisticas.h"
#include "../include/mural.h"
#include "../include/APOSTA.h"
#include "../include/criativo.h"

/* ---- Fases do motor (equivalentes as etapas do antigo motor_grafico.h) -- */
enum {
    FASE_CADASTRO = 0,
    FASE_VEZ,            /* jogador da vez pode jogar carta / pedir / sinal / VAR */
    FASE_RESPOSTA,        /* outra dupla responde truco/seis/doze */
    FASE_RODADA,          /* rodada concluida, aguardando "continuar" */
    FASE_MAO,             /* mao concluida, aguardando "continuar" */
    FASE_FIM,              /* partida encerrada */
    FASE_ERRO
};

#define BUF_GRANDE 16384
#define BUF_VAR_MAX 131072

typedef struct {
    ArvAVL *caderneta;
    tp_listad *var;
    tp_fila mesa;

    char assentos[4][NOME_MAX]; /* nome de quem sentou em cada assento 0..3, na ordem de cadastro */

    tp_item vira;
    tp_item cartas[4];
    tp_aposta aposta;
    tp_item_VAR registro;
    tp_jogador respondente;
    char pediu[NOME_MAX];

    int etapa;
    int visivel;          /* 0 = mao do jogador da vez escondida; 1 = revelada apos confirmar */
    int placar[3];
    int numero;
    int rodada;            /* 0..2: rodada atual dentro da mao */
    int qtd_mesa;           /* cartas ja jogadas nesta rodada (0..4) */
    int manilha;
    int giro;
    int primeiro_dupla;
    int resultados[3];
    int dupla_rodada_atual; /* resultado da ultima rodada concluida (para a tela FASE_RODADA) */
    int dupla_mao_atual;    /* resultado da ultima mao concluida (para a tela FASE_MAO) */
    int pontos_mao_atual;
    int maior, numero_maior;
    int erros_disco;        /* mantido por compatibilidade conceitual; nunca incrementado aqui */
    char aviso[256];
    int iniciado;
} tp_motor;

static tp_motor M;
static char BUF_ESTADO[BUF_GRANDE];
static char BUF_VAR[BUF_VAR_MAX];
static char BUF_STATS[BUF_GRANDE];

/* Area de memoria que a pagina usa para escrever os nomes antes de
 * chamar motor_iniciar(). Evita qualquer dependencia de malloc vindo do
 * lado do JS: a propria pagina escreve bytes UTF-8 aqui dentro. */
#define SCRATCH_TAM (4 * NOME_MAX)
static char SCRATCH[SCRATCH_TAM];

__attribute__((export_name("motor_scratch_ptr")))
int motor_scratch_ptr(void) { return (int)(intptr_t)SCRATCH; }

__attribute__((export_name("motor_scratch_tam")))
int motor_scratch_tam(void) { return SCRATCH_TAM; }

__attribute__((export_name("motor_definir_semente")))
void motor_definir_semente(int semente) {
    /* Em vez de time(NULL) (Embaralhar.h usa inicializar_seed()), a pagina
     * fornece a semente (Date.now() do navegador). Mesmo gerador de
     * numeros pseudoaleatorios do embaralhar_baralho(); so a fonte da
     * semente muda, para nao depender de chamadas de relogio do sistema
     * dentro do sandbox do WebAssembly. */
    srand((unsigned int)semente);
}

/* ------------------------------- JSON ----------------------------------- */

static void json_escapar(char *dst, size_t dst_tam, const char *src) {
    size_t i = 0;
    if (dst_tam == 0) return;
    for (; *src && i + 2 < dst_tam; src++) {
        if (*src == '"' || *src == '\\') { dst[i++] = '\\'; }
        if (i + 1 >= dst_tam) break;
        dst[i++] = *src;
    }
    dst[i] = '\0';
}

static const char *json_valor_str(int valor) {
    switch (valor) {
        case 1:  return "A";
        case 2:  return "2"; case 3: return "3"; case 4: return "4";
        case 5:  return "5"; case 6: return "6"; case 7: return "7";
        case 11: return "J"; case 12: return "Q"; case 13: return "K";
        default: return "?";
    }
}

static const char *json_naipe_str(char naipe) {
    switch (naipe) {
        case 'O': return "ouros"; case 'E': return "espadas";
        case 'C': return "copas"; case 'P': return "paus";
        default:  return "?";
    }
}

static int pode_pedir_(int dupla); /* ver definicao mais abaixo */

static int eh_manilha_valor(tp_item carta) {
    return M.manilha > 0 && carta.valor == M.manilha;
}

/* Encontra o indice do assento (0..3) a partir do nome do jogador. */
static int assento_do_nome(const char *nome) {
    for (int i = 0; i < 4; i++) {
        if (strcmp(M.assentos[i], nome) == 0) return i;
    }
    return -1;
}

static int escrever_carta(char *buf, size_t tam, tp_item carta) {
    if (carta.valor < 1) return snprintf(buf, tam, "null");
    return snprintf(buf, tam, "{\"valor\":%d,\"naipe\":\"%s\",\"rotulo\":\"%s\",\"manilha\":%s,\"zap\":%s}",
                     carta.valor, json_naipe_str(carta.naipe), json_valor_str(carta.valor),
                     eh_manilha_valor(carta) ? "true" : "false",
                     (eh_manilha_valor(carta) && carta.naipe == 'P') ? "true" : "false");
}

/* Quantidade de cartas que o assento ainda tem na mao (sem revelar o valor). */
static int cartas_restantes_do_assento(int seat_idx) {
    tp_jogador j;
    tp_fila copia = M.mesa;
    while (removeFila(&copia, &j)) {
        if (strcmp(j.nome, M.assentos[seat_idx]) == 0) return 3 - M.rodada;
    }
    return 0;
}

__attribute__((export_name("motor_estado_ptr")))
int motor_estado_ptr(void) { return (int)(intptr_t)BUF_ESTADO; }

__attribute__((export_name("motor_estado_json")))
int motor_estado_json(void) {
    char *b = BUF_ESTADO;
    size_t tam = BUF_GRANDE;
    int n = 0;

    const char *fase_txt =
        M.etapa == FASE_VEZ ? "jogando" :
        M.etapa == FASE_RESPOSTA ? "negociacao" :
        M.etapa == FASE_RODADA ? "rodada_concluida" :
        M.etapa == FASE_MAO ? "mao_concluida" :
        M.etapa == FASE_FIM ? "fim_partida" : "erro";

    tp_jogador atual;
    posicao_do_jogador(M.mesa, 0, &atual);
    int seat_atual = assento_do_nome(atual.nome);

    char aviso_escapado[320];
    json_escapar(aviso_escapado, sizeof(aviso_escapado), M.aviso);

    n += snprintf(b + n, tam - n,
        "{\"fase\":\"%s\",\"placar1\":%d,\"placar2\":%d,\"mao\":%d,\"rodada\":%d,"
        "\"valor_mao\":%d,\"vira\":", fase_txt, M.placar[1], M.placar[2], M.numero,
        M.rodada + 1, M.aposta.valor);
    n += escrever_carta(b + n, tam - n, M.vira);
    n += snprintf(b + n, tam - n, ",\"manilha_valor\":\"%s\",\"visivel\":%s,\"aviso\":\"%s\",",
                  M.manilha > 0 ? json_valor_str(M.manilha) : "?", M.visivel ? "true" : "false",
                  aviso_escapado);

    /* assentos (posicao fixa na mesa, independente da rotacao da fila) */
    n += snprintf(b + n, tam - n, "\"jogadores\":[");
    for (int s = 0; s < 4; s++) {
        tp_jogador j;
        tp_fila copia = M.mesa;
        int dupla = 0;
        while (removeFila(&copia, &j)) {
            if (strcmp(j.nome, M.assentos[s]) == 0) { dupla = j.dupla; break; }
        }
        char nome_escapado[NOME_MAX * 2];
        json_escapar(nome_escapado, sizeof(nome_escapado), M.assentos[s]);
        n += snprintf(b + n, tam - n,
            "%s{\"assento\":%d,\"nome\":\"%s\",\"dupla\":%d,\"cartas_na_mao\":%d,\"vez\":%s",
            s ? "," : "", s, nome_escapado, dupla, cartas_restantes_do_assento(s),
            (s == seat_atual && M.etapa == FASE_VEZ) ? "true" : "false");
        if (s == seat_atual && M.etapa == FASE_VEZ && M.visivel) {
            int qtd = 3 - M.rodada;
            tp_jogador jcopia = atual;
            ordenar_mao(&jcopia, qtd);
            n += snprintf(b + n, tam - n, ",\"mao\":[");
            for (int c = 0; c < qtd; c++) {
                if (c) n += snprintf(b + n, tam - n, ",");
                n += escrever_carta(b + n, tam - n, jcopia.mao[c]);
            }
            n += snprintf(b + n, tam - n, "]");
        }
        n += snprintf(b + n, tam - n, "}");
    }
    n += snprintf(b + n, tam - n, "],");

    /* cartas jogadas nesta rodada */
    n += snprintf(b + n, tam - n, "\"mesa\":[");
    for (int i = 0; i < M.qtd_mesa; i++) {
        n += snprintf(b + n, tam - n, "%s{\"assento\":%d,\"carta\":", i ? "," : "",
                      assento_do_nome(M.registro.jogadores[i]));
        n += escrever_carta(b + n, tam - n, M.cartas[i]);
        n += snprintf(b + n, tam - n, "}");
    }
    n += snprintf(b + n, tam - n, "],");

    /* acoes validas nesta fase */
    int pode_jogar = (M.etapa == FASE_VEZ && M.visivel);
    int pode_pedir = (M.etapa == FASE_VEZ && M.visivel && pode_pedir_(atual.dupla));
    n += snprintf(b + n, tam - n,
        "\"acoes\":{\"pode_jogar\":%s,\"cartas_jogaveis\":%d,\"pode_pedir\":%s,"
        "\"pedido_possivel\":%d,\"pode_sinal\":%s,\"pode_ver_var\":%s,\"pode_ver_stats\":true,"
        "\"precisa_confirmar\":%s},",
        pode_jogar ? "true" : "false", 3 - M.rodada,
        pode_pedir ? "true" : "false",
        pode_pedir ? proximo_valor(M.aposta.valor) : 0,
        (M.etapa == FASE_VEZ && M.visivel) ? "true" : "false",
        "true",
        (!M.visivel && (M.etapa == FASE_VEZ || M.etapa == FASE_RESPOSTA)) ? "true" : "false");

    if (M.etapa == FASE_RESPOSTA) {
        char pediu_escapado[NOME_MAX * 2], resp_escapado[NOME_MAX * 2];
        json_escapar(pediu_escapado, sizeof(pediu_escapado), M.pediu);
        json_escapar(resp_escapado, sizeof(resp_escapado), M.respondente.nome);
        int novo = proximo_valor(M.aposta.pedido);
        n += snprintf(b + n, tam - n,
            "\"negociacao\":{\"pediu\":\"%s\",\"dupla_pediu\":%d,\"respondente\":\"%s\","
            "\"respondente_dupla\":%d,\"valor_pedido\":%d,\"nome_pedido\":\"%s\","
            "\"pode_aumentar\":%s,\"proximo_valor\":%d,\"visivel\":%s},",
            pediu_escapado, M.aposta.dupla_pediu, resp_escapado, M.respondente.dupla,
            M.aposta.pedido, nome_pedido(M.aposta.pedido), novo != 0 ? "true" : "false", novo,
            M.visivel ? "true" : "false");
    } else {
        n += snprintf(b + n, tam - n, "\"negociacao\":null,");
    }

    if (M.etapa == FASE_RODADA) {
        n += snprintf(b + n, tam - n, "\"resultado_rodada\":{\"dupla\":%d},", M.dupla_rodada_atual);
    } else {
        n += snprintf(b + n, tam - n, "\"resultado_rodada\":null,");
    }

    if (M.etapa == FASE_MAO) {
        n += snprintf(b + n, tam - n, "\"resultado_mao\":{\"dupla\":%d,\"pontos\":%d},",
                      M.dupla_mao_atual, M.pontos_mao_atual);
    } else {
        n += snprintf(b + n, tam - n, "\"resultado_mao\":null,");
    }

    if (M.etapa == FASE_FIM) {
        int vencedora = M.placar[1] >= 12 ? 1 : 2;
        int p1 = M.placar[1] > 12 ? 12 : M.placar[1];
        int p2 = M.placar[2] > 12 ? 12 : M.placar[2];
        n += snprintf(b + n, tam - n,
            "\"fim\":{\"dupla_vencedora\":%d,\"placar1_final\":%d,\"placar2_final\":%d,"
            "\"maior_mao\":%d,\"numero_maior_mao\":%d}",
            vencedora, p1, p2, M.maior, M.numero_maior);
    } else {
        n += snprintf(b + n, tam - n, "\"fim\":null");
    }

    n += snprintf(b + n, tam - n, "}");
    return n;
}

/* pode_pedir() de APOSTA.h usa (tp_aposta, int) por valor; nome diferente
 * aqui so para nao colidir com macro/include futuro. */
static int pode_pedir_(int dupla) { return pode_pedir(M.aposta, dupla); }

/* ----------------------------- VAR (consulta) ---------------------------- */

static void escrever_rodada_var_json(char *b, size_t tam, int *n, const tp_item_VAR *v) {
    *n += snprintf(b + *n, tam - *n,
        "{\"mao\":%d,\"rodada\":%d,\"manilha\":\"%s\",\"pontos_em_jogo\":%d,"
        "\"em_andamento\":%s,\"encerrada_por_fuga\":%s,\"dupla_vencedora\":%d,\"jogadores\":[",
        v->mao, v->rodada, v->manilha > 0 ? json_valor_str(v->manilha) : "?",
        v->pontos_em_jogo, v->em_andamento ? "true" : "false",
        v->encerrada_por_fuga ? "true" : "false", v->dupla_vencedora);
    for (int i = 0; i < 4; i++) {
        char nome_escapado[NOME_MAX * 2];
        json_escapar(nome_escapado, sizeof(nome_escapado), v->jogadores[i][0] ? v->jogadores[i] : "?");
        *n += snprintf(b + *n, tam - *n, "%s{\"nome\":\"%s\",\"carta\":", i ? "," : "", nome_escapado);
        *n += escrever_carta(b + *n, tam - *n, v->cartas_jogadas[i]);
        *n += snprintf(b + *n, tam - *n, "}");
    }
    *n += snprintf(b + *n, tam - *n, "],\"negociacoes\":[");
    for (int i = 0; i < v->qtd_negociacoes; i++) {
        const tp_negociacao_VAR *ng = &v->negociacoes[i];
        char pediu_escapado[NOME_MAX * 2], resp_escapado[NOME_MAX * 2];
        json_escapar(pediu_escapado, sizeof(pediu_escapado), ng->pediu);
        json_escapar(resp_escapado, sizeof(resp_escapado), ng->respondeu);
        *n += snprintf(b + *n, tam - *n,
            "%s{\"pediu\":\"%s\",\"dupla_pediu\":%d,\"respondeu\":\"%s\",\"valor\":%d,\"resposta\":\"%c\"}",
            i ? "," : "", pediu_escapado, ng->dupla_pediu, resp_escapado, ng->valor, ng->resposta);
    }
    *n += snprintf(b + *n, tam - *n, "]}");
}

__attribute__((export_name("motor_var_ptr")))
int motor_var_ptr(void) { return (int)(intptr_t)BUF_VAR; }

__attribute__((export_name("motor_var_json")))
int motor_var_json(void) {
    char *b = BUF_VAR;
    size_t tam = BUF_VAR_MAX;
    int n = 0;
    n += snprintf(b + n, tam - n, "[");
    int primeiro = 1;
    if (M.var != NULL) {
        for (tp_no_de *atu = M.var->ini; atu != NULL; atu = atu->prox) {
            if (!primeiro) n += snprintf(b + n, tam - n, ",");
            escrever_rodada_var_json(b, tam, &n, &atu->info);
            primeiro = 0;
        }
    }
    /* Rodada em andamento: so as cartas ja jogadas, igual ao VAR do
     * terminal (ver_var_com_rodada em var.h), mas aqui sempre disponivel
     * (nao somente na primeira rodada da partida). */
    if (M.etapa == FASE_VEZ || M.etapa == FASE_RESPOSTA) {
        tp_item_VAR parcial = M.registro;
        parcial.pontos_em_jogo = M.aposta.valor;
        if (!primeiro) n += snprintf(b + n, tam - n, ",");
        escrever_rodada_var_json(b, tam, &n, &parcial);
    }
    n += snprintf(b + n, tam - n, "]");
    return n;
}

/* --------------------------- Estatisticas / caderneta --------------------- */

static void coletar_stats(struct NO *no, char *b, size_t tam, int *n, int *primeiro) {
    if (no == NULL) return;
    coletar_stats(no->esq, b, tam, n, primeiro);
    info_caderneta info = no->info;
    char nome_escapado[100];
    json_escapar(nome_escapado, sizeof(nome_escapado), info.nome_jogador);
    double taxa = taxa_correu(info);
    *n += snprintf(b + *n, tam - *n,
        "%s{\"nome\":\"%s\",\"partidas\":%d,\"maos_ganhas\":%d,\"pedidos\":%d,"
        "\"recebidos\":%d,\"corridos\":%d,\"taxa_fuga\":%.2f,\"tem_pedidos\":%s}",
        *primeiro ? "" : ",", nome_escapado, info.partidas_jogadas, info.maos_ganhas,
        info.trucos_pedidos, info.trucos_recebidos, info.trucos_corridos, taxa,
        info.trucos_recebidos > 0 ? "true" : "false");
    *primeiro = 0;
    coletar_stats(no->dir, b, tam, n, primeiro);
}

__attribute__((export_name("motor_estatisticas_ptr")))
int motor_estatisticas_ptr(void) { return (int)(intptr_t)BUF_STATS; }

__attribute__((export_name("motor_estatisticas_json")))
int motor_estatisticas_json(void) {
    char *b = BUF_STATS;
    size_t tam = BUF_GRANDE;
    int n = 0, primeiro = 1;
    n += snprintf(b + n, tam - n, "[");
    if (M.caderneta != NULL) coletar_stats(*M.caderneta, b, tam, &n, &primeiro);
    n += snprintf(b + n, tam - n, "]");
    return n;
}

/* -------------------------------- Ciclo do jogo --------------------------- */

static void nova_rodada(void) {
    M.qtd_mesa = 0;
    inicializa_variavel_var(&M.registro);
    M.registro.mao = M.numero;
    M.registro.rodada = M.rodada + 1;
    M.registro.manilha = M.manilha;
    M.registro.pontos_em_jogo = M.aposta.valor;
    for (int i = 0; i < 4; i++) {
        tp_jogador j;
        posicao_do_jogador(M.mesa, i, &j);
        strcpy(M.registro.jogadores[i], j.nome);
        M.cartas[i].valor = -1;
        M.cartas[i].naipe = 'A';
    }
    M.visivel = 0;
    M.etapa = FASE_VEZ;
    tp_jogador atual;
    posicao_do_jogador(M.mesa, 0, &atual);
    snprintf(M.aviso, sizeof(M.aviso), "Rodada %d. Passe o dispositivo para %s.", M.rodada + 1, atual.nome);
}

static void nova_mao(void) {
    tp_stack baralho;
    tp_item manilhas[4];
    tp_jogador primeiro;
    M.numero++;
    M.rodada = 0;
    M.giro = 0;
    memset(M.resultados, 0, sizeof(M.resultados));
    aposta_iniciar(&M.aposta);
    embaralhar_baralho(&baralho);
    distribuir_cartas(&baralho, &M.mesa);
    M.vira = vira(&baralho);
    manilha(M.vira, manilhas);
    M.manilha = manilhas[0].valor;
    posicao_do_jogador(M.mesa, 0, &primeiro);
    M.primeiro_dupla = primeiro.dupla;
    nova_rodada();
    snprintf(M.aviso, sizeof(M.aviso), "Mao %d. Vira: %s de %s. Passe para %s.",
             M.numero, json_valor_str(M.vira.valor), json_naipe_str(M.vira.naipe), primeiro.nome);
}

__attribute__((export_name("motor_iniciar")))
int motor_iniciar(void) {
    char nomes[4][NOME_MAX];
    for (int i = 0; i < 4; i++) {
        snprintf(nomes[i], NOME_MAX, "%s", SCRATCH + i * NOME_MAX);
        if (!disco_nome_valido(nomes[i])) {
            snprintf(M.aviso, sizeof(M.aviso), "Preencha os quatro nomes, sem ponto e virgula.");
            return 0;
        }
        for (int k = 0; k < i; k++) {
            if (strcmp(nomes[i], nomes[k]) == 0) {
                snprintf(M.aviso, sizeof(M.aviso), "Use um nome diferente para cada jogador.");
                return 0;
            }
        }
    }
    if (M.caderneta == NULL) M.caderneta = criarAVL();
    if (M.caderneta == NULL) {
        snprintf(M.aviso, sizeof(M.aviso), "Memoria insuficiente para iniciar a caderneta.");
        return 0;
    }
    for (int i = 0; i < 4; i++) {
        if (acessar_jogador(M.caderneta, nomes[i]) == NULL) {
            snprintf(M.aviso, sizeof(M.aviso), "Nao foi possivel cadastrar %s.", nomes[i]);
            return 0;
        }
        strcpy(M.assentos[i], nomes[i]);
    }
    tp_listad *novo_var = inicializa_listad();
    if (novo_var == NULL) {
        snprintf(M.aviso, sizeof(M.aviso), "Memoria insuficiente para iniciar o VAR.");
        return 0;
    }
    if (M.var != NULL) Destroi_listad(M.var);
    M.var = novo_var;

    M.placar[1] = M.placar[2] = 0;
    M.numero = M.maior = M.numero_maior = M.erros_disco = 0;
    inicializaFila(&M.mesa);
    montar_mesa(&M.mesa, nomes);
    M.iniciado = 1;
    nova_mao();
    return 1;
}

static void registrar_rodada_atual(int dupla_vencedora, int corrido) {
    mao_var(&M.registro, M.numero);
    rodada_var(&M.registro, M.rodada + 1);
    dupla_vencedora_var(&M.registro, dupla_vencedora);
    cartas_var(&M.registro, M.cartas, M.manilha);
    if (corrido) {
        aposta_var(&M.registro, M.aposta.dupla_pediu, proximo_valor(M.aposta.valor), 1);
    } else if (M.aposta.valor == 1) {
        aposta_var(&M.registro, -1, -1, -1);
    } else {
        aposta_var(&M.registro, M.aposta.ultima_dupla, M.aposta.valor, 0);
    }
    M.registro.pontos_em_jogo = corrido ? M.pontos_mao_atual : M.aposta.valor;
    M.registro.encerrada_por_fuga = corrido;
    M.registro.em_andamento = 0;
    adiciona_var(&M.registro, M.var);
}

static void fechar_mao(int dupla, int pontos) {
    girar_roda(&M.mesa, (4 - M.giro % 4) % 4);
    M.placar[dupla] += pontos;
    registrar_dupla_vencedora(M.caderneta, M.mesa, dupla);
    if (pontos > M.maior) M.numero_maior = M.numero;
    registrar_maior_mao(&M.maior, pontos);
    passar_mao(&M.mesa);
    M.visivel = 0;
    M.dupla_mao_atual = dupla;
    M.pontos_mao_atual = pontos;
    if (M.placar[dupla] >= 12) {
        registrar_partida_jogadores(M.caderneta, M.mesa);
        M.etapa = FASE_FIM;
        snprintf(M.aviso, sizeof(M.aviso), "Dupla %d venceu a partida! Mao mais cara: %d ponto(s), na mao %d.",
                 dupla, M.maior, M.numero_maior);
    } else {
        M.etapa = FASE_MAO;
        snprintf(M.aviso, sizeof(M.aviso), "Dupla %d ganhou a mao e marcou %d ponto(s).", dupla, pontos);
    }
}

__attribute__((export_name("motor_confirmar_pronto")))
int motor_confirmar_pronto(void) {
    if (M.etapa != FASE_VEZ && M.etapa != FASE_RESPOSTA) return 0;
    M.visivel = 1;
    return 1;
}

__attribute__((export_name("motor_continuar")))
int motor_continuar(void) {
    if (M.etapa == FASE_MAO) { nova_mao(); return 1; }
    if (M.etapa != FASE_RODADA) return 0;
    int dupla = vencedor_da_mao(M.resultados, M.rodada, M.primeiro_dupla);
    if (dupla) fechar_mao(dupla, M.aposta.valor);
    else nova_rodada();
    return 1;
}

__attribute__((export_name("motor_jogar_carta")))
int motor_jogar_carta(int escolha) {
    if (M.etapa != FASE_VEZ || !M.visivel) return 0;
    tp_jogador j, descarte;
    int qtd = 3 - M.rodada;
    posicao_do_jogador(M.mesa, 0, &j);
    ordenar_mao(&j, qtd);
    if (!jogar_carta(&j, &qtd, escolha, &M.cartas[M.qtd_mesa])) return 0;
    registrar_jogada_var(&M.registro, M.qtd_mesa, j.nome, M.cartas[M.qtd_mesa]);
    snprintf(M.aviso, sizeof(M.aviso), "%s jogou %s de %s.", j.nome,
             json_valor_str(M.cartas[M.qtd_mesa].valor), json_naipe_str(M.cartas[M.qtd_mesa].naipe));
    removeFila(&M.mesa, &descarte);
    insereFila(&M.mesa, j);
    M.qtd_mesa++;
    M.visivel = 0;

    if (M.qtd_mesa == 4) {
        int duplas[4];
        for (int i = 0; i < 4; i++) {
            tp_jogador dono;
            posicao_do_jogador(M.mesa, i, &dono);
            duplas[i] = dono.dupla;
        }
        int dupla;
        int pos = vencedor_da_rodada(M.cartas, duplas, M.manilha, &dupla);
        registrar_rodada_atual(dupla, 0);
        if (pos >= 0) { girar_roda(&M.mesa, pos); M.giro += pos; }
        M.resultados[M.rodada++] = dupla;
        M.dupla_rodada_atual = dupla;
        M.etapa = FASE_RODADA;
        if (dupla) snprintf(M.aviso, sizeof(M.aviso), "Dupla %d venceu a rodada.", dupla);
        else snprintf(M.aviso, sizeof(M.aviso), "Rodada empatada.");
    } else {
        tp_jogador proximo;
        posicao_do_jogador(M.mesa, 0, &proximo);
        char msg_extra[160];
        snprintf(msg_extra, sizeof(msg_extra), " Passe para %s.", proximo.nome);
        strncat(M.aviso, msg_extra, sizeof(M.aviso) - strlen(M.aviso) - 1);
    }
    return 1;
}

static void preparar_resposta(void) {
    int dupla = M.aposta.dupla_pediu == 1 ? 2 : 1;
    jogador_da_dupla(M.mesa, dupla, &M.respondente);
    registrar_truco_recebido(M.caderneta, M.respondente.nome);
    M.etapa = FASE_RESPOSTA;
    M.visivel = 0;
    snprintf(M.aviso, sizeof(M.aviso), "%s pediu %s. Passe para %s responder.",
             M.pediu, nome_pedido(M.aposta.pedido), M.respondente.nome);
}

__attribute__((export_name("motor_pedir_aumento")))
int motor_pedir_aumento(void) {
    if (M.etapa != FASE_VEZ || !M.visivel) return 0;
    tp_jogador j;
    posicao_do_jogador(M.mesa, 0, &j);
    if (!pedir_aposta(&M.aposta, j.dupla)) return 0;
    registrar_truco_pedido(M.caderneta, j.nome);
    strcpy(M.pediu, j.nome);
    preparar_resposta();
    return 1;
}

/* opcao: 0 = aceitar ('A'), 1 = correr ('C'), 2 = aumentar ('T') */
__attribute__((export_name("motor_responder")))
int motor_responder(int opcao) {
    if (M.etapa != FASE_RESPOSTA || !M.visivel) return 0;
    char resposta = opcao == 0 ? 'A' : opcao == 1 ? 'C' : opcao == 2 ? 'T' : 0;
    if (resposta == 0) return 0;
    if (resposta == 'T' && proximo_valor(M.aposta.pedido) == 0) return 0;

    registrar_negociacao_var(&M.registro, M.pediu, M.aposta.dupla_pediu, M.respondente.nome,
                              M.aposta.pedido, resposta);
    if (resposta == 'A') {
        aceitar_aposta(&M.aposta);
        M.registro.pontos_em_jogo = M.aposta.valor;
        M.etapa = FASE_VEZ;
        M.visivel = 0;
        tp_jogador j;
        posicao_do_jogador(M.mesa, 0, &j);
        snprintf(M.aviso, sizeof(M.aviso), "%s aceitou. A mao vale %d ponto(s). Passe para %s.",
                 M.respondente.nome, M.aposta.valor, j.nome);
    } else if (resposta == 'T') {
        registrar_truco_pedido(M.caderneta, M.respondente.nome);
        strcpy(M.pediu, M.respondente.nome);
        aumentar_aposta(&M.aposta, M.respondente.dupla);
        preparar_resposta();
    } else {
        int dupla, pontos;
        registrar_truco_corrido(M.caderneta, M.respondente.nome);
        correr_aposta(&M.aposta, &dupla, &pontos);
        M.pontos_mao_atual = pontos;
        registrar_rodada_atual(dupla, 1);
        girar_roda(&M.mesa, (4 - M.qtd_mesa) % 4);
        fechar_mao(dupla, pontos);
    }
    return 1;
}

__attribute__((export_name("motor_enviar_sinal")))
int motor_enviar_sinal(int gesto) {
    if (M.etapa != FASE_VEZ || !M.visivel || gesto < 1 || gesto > 3) return 0;
    tp_jogador atual, parceiro;
    if (!criativo_encontrar_parceiro(M.mesa, &parceiro)) return 0;
    posicao_do_jogador(M.mesa, 0, &atual);
    const char *texto = criativo_gesto_texto(gesto);
    snprintf(M.aviso, sizeof(M.aviso), "%s %s para %s. A vez continua com %s.",
             atual.nome, texto, parceiro.nome, atual.nome);
    return 1;
}

__attribute__((export_name("motor_sinal_texto_ptr")))
int motor_sinal_texto_ptr(int gesto) { return (int)(intptr_t)criativo_gesto_texto(gesto); }

__attribute__((export_name("motor_reiniciar")))
int motor_reiniciar(void) {
    if (M.var != NULL) { Destroi_listad(M.var); M.var = NULL; }
    if (M.caderneta != NULL) { liberaABB(M.caderneta); M.caderneta = NULL; }
    memset(&M, 0, sizeof(M));
    return 1;
}
