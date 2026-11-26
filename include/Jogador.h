
#ifndef JOGADOR_H
#define JOGADOR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "STACK.h"
#include "fila.h"

#define NOME_MAX 30

void limpar_buffer() { //limpa o buffer do teclado
    int c;
    while ((c = getchar()) != '\n' && c != EOF); //a variável c é temporária e armazena os caracteres do buffer, até a quebra de linha ou o fim do arquivo.
}

void limpar_tela() { //pode ser chamada na main para limpar o terminal (não é obrigatória)
#ifdef _WIN32 
    system("cls"); //se o OS for windows
#else
    system("clear"); //se o OS for linus/mac
#endif
}

void remover_quebra_linha(char *str) { //remove o enter do fgets na leitura de nomes
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// lê a opção digitada
char ler_opcao() {
    char linha[64];

    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        exit(0); // teclado fechado
    }
    if (strchr(linha, '\n') == NULL) {
        limpar_buffer();
    }

    int i = 0;
    while (linha[i] == ' ' || linha[i] == '\t') {
        i++;
    }
    return (char)toupper((unsigned char)linha[i]);
}

// recebe o nome dos quatro jogadores na ordem em que vão ficar na mesa
// Retorna zero se a entrada for encerrada antes de completar o cadastro.
int cadastrar_jogadores(char nomes[4][NOME_MAX]) {
    int duplas[4] = { 1, 2, 1, 2 };
    for (int i = 0; i < 4; i++) {
        printf("Nome do Jogador %d (Dupla %d): ", i + 1, duplas[i]);

        if (fgets(nomes[i], NOME_MAX, stdin) != NULL) {
            if (strchr(nomes[i], '\n') == NULL) {
                int seguinte = getchar();
                if (seguinte != '\n' && seguinte != EOF) {
                    limpar_buffer();
                    printf("Nome muito longo. Use ate %d caracteres.\n", NOME_MAX - 1);
                    i--;
                    continue;
                }
            }
            else {
                remover_quebra_linha(nomes[i]);
            }
            // A chave da caderneta nao pode ser vazia nem repetir na mesa.
            size_t inicio = 0;
            while (isspace((unsigned char)nomes[i][inicio])) inicio++;
            memmove(nomes[i], nomes[i] + inicio, strlen(nomes[i] + inicio) + 1);
            size_t tamanho = strlen(nomes[i]);
            while (tamanho > 0 && isspace((unsigned char)nomes[i][tamanho - 1])) {
                nomes[i][--tamanho] = '\0';
            }
            int repetido = 0;
            for (int j = 0; j < i; j++) {
                if (strcmp(nomes[i], nomes[j]) == 0) repetido = 1;
            }
            if (tamanho == 0 || repetido) {
                printf("Informe um nome nao vazio e diferente dos demais jogadores.\n");
                i--;
            }
        } else {
            return 0;
        }
    }
    return 1;
}

// coloca os jogadores na fila já alternando as duas duplas
void montar_mesa(tp_fila* m, char nomes[4][NOME_MAX]) {
    int duplas[4] = { 1, 2, 1, 2 }; // parceiros sentados alternados
    tp_jogador jogador;
    for (int i = 0; i < 4; i++) {
        strcpy(jogador.nome, nomes[i]);
        jogador.dupla = duplas[i];
        insereFila(m, jogador);
    }
}

int posicao_do_jogador(tp_fila m, int posicao, tp_jogador* j_out) { //consulta o jogador sem alterar a ordem
    if (posicao < 0 || posicao >= 4) return 0; //falha
    tp_jogador temp;
    for (int i = 0; i <= posicao; i++) {
        if (!removeFila(&m, &temp)) return 0; //falha
    }
    *j_out = temp;
    return 1; //sucesso
}

// usa uma cópia da fila, então mostrar a mesa não muda a ordem dos jogadores
void exibir_mesa(tp_fila m) {
    printf("\nMESA\n");
    tp_jogador j;

    for (int i = 0; i < 4; i++) {
        if (removeFila(&m, &j)) {
            if (i == 0) {
                printf("Jogador %d: %s | Dupla %d (MAO)\n", i + 1, j.nome, j.dupla);
            }
            else {
                printf("Jogador %d: %s | Dupla %d\n", i + 1, j.nome, j.dupla);
            }
        }
    }
}



#endif
