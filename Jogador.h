
#ifndef JOGADOR_H
#define JOGADOR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void cadastrar_jogadores(char nomes[4][NOME_MAX]) {
    int duplas[4] = { 1, 2, 1, 2 };
    for (int i = 0; i < 4; i++) {
        printf("Nome do Jogador %d (Dupla %d): ", i + 1, duplas[i]);

        if (fgets(nomes[i], NOME_MAX, stdin) != NULL) {
            if (strchr(nomes[i], '\n') == NULL) {
                limpar_buffer(); //caso o nome seja maior que NOME_MAX
            }
            else {
                remover_quebra_linha(nomes[i]);
            }
        }
    }
}

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

void exibir_mesa(tp_fila m) { //recebe uma cópia da fila para não alterar a original
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
