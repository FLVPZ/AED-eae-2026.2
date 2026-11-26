#ifndef CRIATIVO_H
#define CRIATIVO_H

#include <stdio.h>
#include "STACK.h"
#include "fila.h"
#include "Jogador.h"
#include "sinais_arquivo.h"

// Usa a mesma fila e conserva a ordem da mesa ao carregar cada jogador.
static void criativo_carregar_sinais(tp_fila *mesa) {
    int total = tamanhoFila(mesa);
    for (int i = 0; i < total; i++) {
        tp_jogador j;
        removeFila(mesa, &j);
        int resultado = sinais_carregar(&j, ARQUIVO_SINAIS);
        if (resultado == 1) printf("Sinais de %s recuperados.\n", j.nome);
        if (resultado < 0) printf("Nao foi possivel ler os sinais de %s; usando os padroes.\n", j.nome);
        insereFila(mesa, j);
    }
}

// Edita uma copia: cancelar ou falhar ao salvar preserva a configuracao atual.
static void criativo_personalizar_sinais(tp_jogador *atual) {
    tp_jogador novos = *atual;
    printf("\nPersonalizar sinais de %s. Enter mantem; 0 cancela tudo.\n", atual->nome);
    for (int i = 0; i < SINAIS_QTD; i++) {
        for (;;) {
            char texto[SINAL_MAX + 2];
            printf("Sinal %d [%s] (ate %d caracteres): ", i + 1,
                   novos.sinais[i], SINAL_MAX - 1);
            if (fgets(texto, sizeof(texto), stdin) == NULL) return;
            char *fim = strchr(texto, '\n');
            if (fim == NULL) {
                limpar_buffer();
                printf("Sinal muito longo ou entrada incompleta.\n");
                continue;
            }
            *fim = '\0';
            size_t tamanho = strlen(texto);
            if (tamanho > 0 && texto[tamanho - 1] == '\r') texto[--tamanho] = '\0';
            if (strcmp(texto, "0") == 0) return;
            if (texto[0] == '\0') break;
            if (!sinais_texto_valido(texto, SINAL_MAX)) {
                printf("Use um sinal de texto nao vazio, sem tabulacoes, de ate %d caracteres.\n", SINAL_MAX - 1);
                continue;
            }
            strcpy(novos.sinais[i], texto);
            break;
        }
    }
    if (!sinais_salvar(&novos, ARQUIVO_SINAIS)) {
        printf("Nao foi possivel salvar. Seus sinais anteriores foram mantidos.\n");
        return;
    }
    memcpy(atual->sinais, novos.sinais, sizeof(atual->sinais));
    printf("Sinais salvos para as proximas partidas de %s.\n", atual->nome);
}

// Os destaques dependem da manilha já calculada pelo jogo.
// Esta função só escreve na tela: não muda a carta nem sua força.
static void criativo_destacar_manilha(tp_item carta, int valor_manilha) {
    if (valor_manilha <= 0 || carta.valor != valor_manilha) return;

    switch (carta.naipe) {
        case 'P': printf(" [MANILHA - ZAP]"); break;
        case 'C':
        case 'E':
        case 'O': printf(" [MANILHA]"); break;
    }
}

static void criativo_exibir_mao(const tp_jogador *j, int qtd_cartas,
                               int valor_manilha) {
    printf("Sua mao: ");
    for (int i = 0; i < qtd_cartas; i++) {
        tp_item carta = j->mao[i];
        printf("[%d] ", i + 1);

        switch (carta.valor) {
            case 1:  printf("As"); break;
            case 11: printf("J"); break;
            case 12: printf("Q"); break;
            case 13: printf("K"); break;
            default: printf("%d", carta.valor);
        }
        switch (carta.naipe) {
            case 'O': printf(" de Ouros"); break;
            case 'E': printf(" de Espadas"); break;
            case 'C': printf(" de Copas"); break;
            case 'P': printf(" de Paus"); break;
        }

        criativo_destacar_manilha(carta, valor_manilha);
        if (i < qtd_cartas - 1) printf(" | ");
    }
    printf("\n");
}

// A fila recebida é uma cópia. O primeiro é quem está jogando;
// o parceiro é procurado entre os demais pela dupla, mesmo após giros.
static int criativo_encontrar_parceiro(tp_fila mesa, tp_jogador *parceiro) {
    tp_jogador atual, candidato;
    if (!removeFila(&mesa, &atual)) return 0;

    while (removeFila(&mesa, &candidato)) {
        if (candidato.dupla == atual.dupla) {
            *parceiro = candidato;
            return 1;
        }
    }
    return 0;
}

// O jogador escolhe o gesto. Nenhuma carta do parceiro é consultada,
// e enviar um sinal não joga carta, não muda a aposta e não passa a vez.
static void criativo_enviar_sinal(tp_fila mesa, tp_jogador *atual) {
    tp_jogador parceiro;
    if (!criativo_encontrar_parceiro(mesa, &parceiro)) {
        printf("Parceiro indisponivel.\n");
        return;
    }

    for (;;) {
        printf("\nSinal para %s (Dupla %d)\n", parceiro.nome, parceiro.dupla);
        for (int i = 0; i < SINAIS_QTD; i++) printf("[%d] %s | ", i + 1, atual->sinais[i]);
        printf("[P] Personalizar e salvar | [0] Voltar: ");
        char op = ler_opcao();
        const char *gesto;

        switch (op) {
            case '0': return;
            case '1': gesto = atual->sinais[0]; break;
            case '2': gesto = atual->sinais[1]; break;
            case '3': gesto = atual->sinais[2]; break;
            case 'P':
                criativo_personalizar_sinais(atual);
                continue;
            default:
                printf("Opcao invalida.\n");
                continue;
        }

        printf("\n[SINAL] %s %s para %s.\n", atual->nome, gesto, parceiro.nome);
        printf("%s continua com a vez.\n", atual->nome);
        return;
    }
}

static void criativo_apresentar(void) {
    printf("\nTRUCO NO BAR DO PATO\n");
    printf("As manilhas aparecem com [MANILHA]; a de Paus, com [MANILHA - ZAP].\n");
    printf("Na sua vez, use [S] para enviar um sinal ao parceiro e [P] nesse menu para personalizar.\n");
    printf("Seus sinais personalizados ficam salvos e voltam quando voce usa o mesmo nome.\n");
    printf("Combinem o significado dos gestos antes de jogar.\n\n");
}

#endif
