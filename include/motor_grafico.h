#ifndef MOTOR_GRAFICO_H
#define MOTOR_GRAFICO_H

/* Fluxo sem leitura de teclado: a janela envia uma acao por clique.
 * Reutiliza as regras, a fila, a pilha, a AVL e o VAR existentes.
 */
#include "APOSTA.h"
#include "Embaralhar.h"
#include "jogo.h"
#include "Mao.h"
#include "disco.h"
#include "mural.h"

enum { GUI_CADASTRO, GUI_VEZ, GUI_RESPOSTA, GUI_RODADA, GUI_MAO, GUI_FIM, GUI_ERRO };
typedef struct {
    ArvAVL *caderneta;
    tp_listad *var;
    tp_fila mesa;
    tp_item vira, cartas[4];
    tp_aposta aposta;
    tp_item_VAR registro;
    tp_jogador respondente;
    char pediu[NOME_MAX];
    int etapa, visivel, placar[3], numero, rodada, qtd_mesa;
    int manilha, giro, primeiro_dupla, resultados[3];
    int maior, numero_maior, erros_disco;
    char aviso[240];
    char erro_disco[1024];
} tp_motor_grafico;

void motor_resultado_disco(tp_motor_grafico *m, int ok) {
    if (ok) return;
    m->erros_disco++;
    snprintf(m->erro_disco, sizeof(m->erro_disco), "%s", disco_ultimo_erro);
}

int motor_atual(tp_motor_grafico *m, tp_jogador *j) {
    if (!posicao_do_jogador(m->mesa, 0, j)) return 0;
    ordenar_mao(j, 3 - m->rodada);
    return 1;
}

void motor_nova_rodada(tp_motor_grafico *m) {
    m->qtd_mesa = 0;
    inicializa_variavel_var(&m->registro);
    m->registro.mao = m->numero;
    m->registro.rodada = m->rodada + 1;
    m->registro.manilha = m->manilha;
    m->registro.pontos_em_jogo = m->aposta.valor;
    for (int i = 0; i < 4; i++) {
        tp_jogador j;
        posicao_do_jogador(m->mesa, i, &j);
        strcpy(m->registro.jogadores[i], j.nome);
        m->cartas[i] = (tp_item){-1, 'A'};
    }
    m->visivel = 0;
    m->etapa = GUI_VEZ;
}

void motor_nova_mao(tp_motor_grafico *m) {
    tp_stack baralho;
    tp_item manilhas[4];
    tp_jogador primeiro;
    m->numero++;
    m->rodada = 0;
    m->giro = 0;
    memset(m->resultados, 0, sizeof(m->resultados));
    aposta_iniciar(&m->aposta);
    embaralhar_baralho(&baralho);
    distribuir_cartas(&baralho, &m->mesa);
    m->vira = vira(&baralho);
    manilha(m->vira, manilhas);
    m->manilha = manilhas[0].valor;
    posicao_do_jogador(m->mesa, 0, &primeiro);
    m->primeiro_dupla = primeiro.dupla;
    motor_nova_rodada(m);
}

int motor_iniciar(tp_motor_grafico *m, char nomes[4][NOME_MAX]) {
    for (int i = 0; i < 4; i++) {
        if (!disco_nome_valido(nomes[i]) || strlen(nomes[i]) >= NOME_MAX) {
            snprintf(m->aviso, sizeof(m->aviso), "Preencha os quatro nomes, sem ponto e virgula.");
            return 0;
        }
        for (int j = 0; j < i; j++) if (strcmp(nomes[i], nomes[j]) == 0) {
            snprintf(m->aviso, sizeof(m->aviso), "Use um nome diferente para cada jogador.");
            return 0;
        }
        if (!acessar_jogador(m->caderneta, nomes[i])) {
            snprintf(m->aviso, sizeof(m->aviso), "Nao foi possivel cadastrar: memoria insuficiente.");
            return 0;
        }
    }
    tp_listad *novo = inicializa_listad();
    if (!novo) {
        snprintf(m->aviso, sizeof(m->aviso), "Memoria insuficiente para iniciar o historico.");
        return 0;
    }
    if (m->var) Destroi_listad(m->var);
    m->var = novo;
    m->placar[1] = m->placar[2] = 0;
    m->numero = m->maior = m->numero_maior = m->erros_disco = 0;
    inicializaFila(&m->mesa);
    montar_mesa(&m->mesa, nomes);
    motor_resultado_disco(m, salvar_caderneta(m->caderneta));
    motor_resultado_disco(m, iniciar_historico(m->mesa));
    motor_nova_mao(m);
    snprintf(m->aviso, sizeof(m->aviso), "Partida iniciada. Entregue o controle ao jogador da vez.");
    return 1;
}

void motor_registrar_rodada(tp_motor_grafico *m, int vencedora, int corrido) {
    mao_var(&m->registro, m->numero);
    rodada_var(&m->registro, m->rodada + 1);
    dupla_vencedora_var(&m->registro, vencedora);
    cartas_var(&m->registro, m->cartas, m->manilha);
    if (corrido) aposta_var(&m->registro, m->aposta.dupla_pediu, proximo_valor(m->aposta.valor), 1);
    else if (m->aposta.valor == 1) aposta_var(&m->registro, -1, -1, -1);
    else aposta_var(&m->registro, m->aposta.ultima_dupla, m->aposta.valor, 0);
    m->registro.pontos_em_jogo = m->aposta.valor;
    m->registro.encerrada_por_fuga = corrido;
    m->registro.em_andamento = 0;
    motor_resultado_disco(m, gravar_rodada_historico(&m->registro));
    if (!adiciona_var(&m->registro, m->var)) {
        m->etapa = GUI_ERRO;
        snprintf(m->aviso, sizeof(m->aviso), "Memoria insuficiente para registrar o VAR. Encerre a partida.");
    }
}

void motor_fechar_mao(tp_motor_grafico *m, int dupla, int pontos) {
    girar_roda(&m->mesa, (4 - m->giro % 4) % 4);
    m->placar[dupla] += pontos;
    registrar_dupla_vencedora(m->caderneta, m->mesa, dupla);
    if (pontos > m->maior) m->numero_maior = m->numero;
    registrar_maior_mao(&m->maior, pontos);
    motor_resultado_disco(m, salvar_caderneta(m->caderneta));
    motor_resultado_disco(m, gravar_historico(m->numero, dupla, pontos, m->placar));
    passar_mao(&m->mesa);
    m->visivel = 0;
    if (m->placar[dupla] >= 12) {
        registrar_partida_jogadores(m->caderneta, m->mesa);
        motor_resultado_disco(m, salvar_caderneta(m->caderneta));
        motor_resultado_disco(m, encerrar_historico(m->placar, m->maior, m->numero_maior));
        m->etapa = GUI_FIM;
        snprintf(m->aviso, sizeof(m->aviso), "Dupla %d venceu! Mao mais cara: %d ponto(s), na mao %d.", dupla, m->maior, m->numero_maior);
    } else {
        m->etapa = GUI_MAO;
        snprintf(m->aviso, sizeof(m->aviso), "Dupla %d ganhou a mao e marcou %d ponto(s).", dupla, pontos);
    }
}

int motor_continuar(tp_motor_grafico *m) {
    if (m->etapa == GUI_MAO) { motor_nova_mao(m); return 1; }
    if (m->etapa != GUI_RODADA) return 0;
    int dupla = vencedor_da_mao(m->resultados, m->rodada, m->primeiro_dupla);
    if (dupla) motor_fechar_mao(m, dupla, m->aposta.valor);
    else motor_nova_rodada(m);
    return 1;
}

int motor_jogar(tp_motor_grafico *m, int escolha) {
    if (m->etapa != GUI_VEZ || !m->visivel) return 0;
    tp_jogador j, descarte;
    int qtd = 3 - m->rodada;
    motor_atual(m, &j);
    if (!jogar_carta(&j, &qtd, escolha, &m->cartas[m->qtd_mesa])) return 0;
    registrar_jogada_var(&m->registro, m->qtd_mesa, j.nome, m->cartas[m->qtd_mesa]);
    snprintf(m->aviso, sizeof(m->aviso), "%s jogou %s de %s.", j.nome,
             ui_valor(m->cartas[m->qtd_mesa].valor), ui_naipe(m->cartas[m->qtd_mesa].naipe));
    removeFila(&m->mesa, &descarte);
    insereFila(&m->mesa, j);
    m->qtd_mesa++;
    m->visivel = 0;
    if (m->qtd_mesa == 4) {
        int duplas[4];
        for (int i = 0; i < 4; i++) {
            tp_jogador dono;
            posicao_do_jogador(m->mesa, i, &dono);
            duplas[i] = dono.dupla;
        }
        int dupla;
        int pos = vencedor_da_rodada(m->cartas, duplas, m->manilha, &dupla);
        motor_registrar_rodada(m, dupla, 0);
        if (m->etapa == GUI_ERRO) return 0;
        if (pos >= 0) { girar_roda(&m->mesa, pos); m->giro += pos; }
        m->resultados[m->rodada++] = dupla;
        m->etapa = GUI_RODADA;
        if (dupla) snprintf(m->aviso, sizeof(m->aviso), "Dupla %d venceu a rodada. Confira as cartas e clique em Continuar.", dupla);
        else snprintf(m->aviso, sizeof(m->aviso), "Rodada empatada. Confira as cartas e clique em Continuar.");
    }
    return 1;
}

void motor_preparar_resposta(tp_motor_grafico *m) {
    int dupla = m->aposta.dupla_pediu == 1 ? 2 : 1;
    jogador_da_dupla(m->mesa, dupla, &m->respondente);
    registrar_truco_recebido(m->caderneta, m->respondente.nome);
    motor_resultado_disco(m, salvar_evento_truco(m->caderneta, m->pediu, m->aposta.dupla_pediu,
                                               m->respondente.nome, m->aposta.pedido, 'P'));
    m->etapa = GUI_RESPOSTA;
    m->visivel = 0;
    snprintf(m->aviso, sizeof(m->aviso), "%s pediu %s. %s deve aceitar, correr ou aumentar.", m->pediu, nome_pedido(m->aposta.pedido), m->respondente.nome);
}

int motor_pedir(tp_motor_grafico *m) {
    if (m->etapa != GUI_VEZ) return 0;
    tp_jogador j;
    motor_atual(m, &j);
    if (!pedir_aposta(&m->aposta, j.dupla)) return 0;
    registrar_truco_pedido(m->caderneta, j.nome);
    strcpy(m->pediu, j.nome);
    motor_preparar_resposta(m);
    return 1;
}

int motor_responder(tp_motor_grafico *m, char resposta) {
    if (m->etapa != GUI_RESPOSTA || (resposta != 'A' && resposta != 'C' && resposta != 'T')) return 0;
    if (resposta == 'T' && proximo_valor(m->aposta.pedido) == 0) return 0;
    registrar_negociacao_var(&m->registro, m->pediu, m->aposta.dupla_pediu,
                             m->respondente.nome, m->aposta.pedido, resposta);
    if (resposta == 'A') {
        motor_resultado_disco(m, salvar_evento_truco(m->caderneta, m->pediu, m->aposta.dupla_pediu,
                                                   m->respondente.nome, m->aposta.pedido, resposta));
        aceitar_aposta(&m->aposta);
        m->registro.pontos_em_jogo = m->aposta.valor;
        m->etapa = GUI_VEZ;
        snprintf(m->aviso, sizeof(m->aviso), "%s aceitou. A mao vale %d ponto(s).", m->respondente.nome, m->aposta.valor);
    } else if (resposta == 'T') {
        registrar_truco_pedido(m->caderneta, m->respondente.nome);
        motor_resultado_disco(m, salvar_evento_truco(m->caderneta, m->pediu, m->aposta.dupla_pediu,
                                                   m->respondente.nome, m->aposta.pedido, resposta));
        strcpy(m->pediu, m->respondente.nome);
        aumentar_aposta(&m->aposta, m->respondente.dupla);
        motor_preparar_resposta(m);
    } else {
        int dupla, pontos;
        registrar_truco_corrido(m->caderneta, m->respondente.nome);
        motor_resultado_disco(m, salvar_evento_truco(m->caderneta, m->pediu, m->aposta.dupla_pediu,
                                                   m->respondente.nome, m->aposta.pedido, resposta));
        correr_aposta(&m->aposta, &dupla, &pontos);
        motor_registrar_rodada(m, dupla, 1);
        if (m->etapa == GUI_ERRO) return 0;
        girar_roda(&m->mesa, (4 - m->qtd_mesa) % 4);
        motor_fechar_mao(m, dupla, pontos);
    }
    return 1;
}

int motor_sinal(tp_motor_grafico *m, int sinal) {
    if (m->etapa != GUI_VEZ || sinal < 0 || sinal > 2) return 0;
    tp_jogador atual, j;
    tp_fila copia = m->mesa;
    removeFila(&copia, &atual);
    while (removeFila(&copia, &j)) if (j.dupla == atual.dupla) {
        const char *gestos[3] = {"pisca", "levanta a sobrancelha", "toca na mesa"};
        snprintf(m->aviso, sizeof(m->aviso), "%s %s para %s. A vez continua com %s.", atual.nome, gestos[sinal], j.nome, atual.nome);
        return 1;
    }
    return 0;
}
#endif
