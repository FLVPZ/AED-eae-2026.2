#ifndef SINAIS_ARQUIVO_H
#define SINAIS_ARQUIVO_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "fila.h"

#ifndef ARQUIVO_SINAIS
#define ARQUIVO_SINAIS "sinais.txt"
#endif

/* Preferencias ficam no proprio Jogador; nao criamos outra entidade do jogo.
 * Cada linha: S1 TAB nome TAB sinal1 TAB sinal2 TAB sinal3.
 * O ultimo registro completo de um nome prevalece. Atualizacoes sao anexadas,
 * preservando os outros jogadores e as configuracoes completas anteriores.
 */
void sinais_padrao(tp_jogador *j) {
    strcpy(j->sinais[0], "pisca ;)");
    strcpy(j->sinais[1], "levanta a sobrancelha ^_^");
    strcpy(j->sinais[2], "toca na mesa: toc, toc!");
}

int sinais_texto_valido(const char *texto, size_t limite) {
    if (texto == NULL) return 0;
    size_t tamanho = 0;
    int tem_texto = 0;
    while (tamanho < limite && texto[tamanho] != '\0') {
        unsigned char c = (unsigned char)texto[tamanho];
        if (c < 32 || c == 127) return 0; /* protege os separadores do arquivo */
        if (c != ' ') tem_texto = 1;
        tamanho++;
    }
    return tamanho < limite && tem_texto;
}

/* Altera somente nome e sinais da ficha temporaria ao validar uma linha. */
int sinais_ler_registro(char *linha, tp_jogador *j) {
    char *campos[2 + SINAIS_QTD];
    char *atual = linha;
    for (int i = 0; i < 2 + SINAIS_QTD; i++) {
        campos[i] = atual;
        char *tab = strchr(atual, '\t');
        if (i == 1 + SINAIS_QTD) {
            if (tab != NULL) return 0;
        } else {
            if (tab == NULL) return 0;
            *tab = '\0';
            atual = tab + 1;
        }
    }
    if (strcmp(campos[0], "S1") != 0
        || !sinais_texto_valido(campos[1], NOME_MAX)) return 0;
    for (int i = 0; i < SINAIS_QTD; i++) {
        if (!sinais_texto_valido(campos[i + 2], SINAL_MAX)) return 0;
    }
    strcpy(j->nome, campos[1]);
    for (int i = 0; i < SINAIS_QTD; i++) strcpy(j->sinais[i], campos[i + 2]);
    return 1;
}

/* 1 = encontrou, 0 = sem registro, -1 = erro de leitura.
 * Arquivo ausente e linhas incompletas/invalidas conservam os sinais padrao.
 */
int sinais_carregar(tp_jogador *j, const char *caminho) {
    if (j == NULL || caminho == NULL) return -1;
    sinais_padrao(j);
    if (!sinais_texto_valido(j->nome, NOME_MAX)) return -1;
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) return errno == ENOENT ? 0 : -1;

    char linha[NOME_MAX + SINAIS_QTD * SINAL_MAX + 16];
    tp_jogador registro = {0};
    char ultimos[SINAIS_QTD][SINAL_MAX] = {{0}};
    int encontrou = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *fim = strchr(linha, '\n');
        if (fim == NULL) { /* descarta linha longa ou gravacao interrompida */
            int c;
            while ((c = fgetc(arquivo)) != '\n' && c != EOF) {}
            continue;
        }
        *fim = '\0';
        size_t tamanho = strlen(linha);
        if (tamanho > 0 && linha[tamanho - 1] == '\r') linha[tamanho - 1] = '\0';
        if (sinais_ler_registro(linha, &registro) && strcmp(registro.nome, j->nome) == 0) {
            memcpy(ultimos, registro.sinais, sizeof(ultimos));
            encontrou = 1;
        }
    }
    int erro = ferror(arquivo);
    if (fclose(arquivo) != 0) erro = 1;
    if (erro) return -1;
    if (encontrou) memcpy(j->sinais, ultimos, sizeof(j->sinais));
    return encontrou;
}

/* 1 = salvo, 0 = erro. Uma linha em branco separa eventual escrita anterior
 * interrompida do novo registro, permitindo recuperar a ultima linha completa.
 */
int sinais_salvar(const tp_jogador *j, const char *caminho) {
    if (j == NULL || caminho == NULL || !sinais_texto_valido(j->nome, NOME_MAX)) return 0;
    for (int i = 0; i < SINAIS_QTD; i++) {
        if (!sinais_texto_valido(j->sinais[i], SINAL_MAX)) return 0;
    }
    FILE *arquivo = fopen(caminho, "a");
    if (arquivo == NULL) return 0;
    int ok = fprintf(arquivo, "\nS1\t%s\t%s\t%s\t%s\n", j->nome,
                     j->sinais[0], j->sinais[1], j->sinais[2]) >= 0;
    if (fflush(arquivo) != 0) ok = 0;
    if (fclose(arquivo) != 0) ok = 0;
    return ok;
}

#endif
