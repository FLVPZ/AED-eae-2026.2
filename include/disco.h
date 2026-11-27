#ifndef DISCO_H
#define DISCO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "caderneta.h"
#ifdef _WIN32
#include <windows.h>
#endif

#define DISCO_ARQUIVO_CADERNETA "caderneta.txt"
#define DISCO_ARQUIVO_TEMPORARIO "caderneta.txt.tmp"
#define DISCO_ARQUIVO_HISTORICO "historico.txt"

#define DISCO_CARREGADO 1
#define DISCO_AUSENTE 0
#define DISCO_ERRO -1

int disco_nome_valido(const char *nome) {
    if (nome == NULL) return 0;
    int tem_texto = 0;
    size_t i;
    for (i = 0; i < sizeof(((info_caderneta*)0)->nome_jogador); i++) {
        unsigned char c = (unsigned char)nome[i];
        if (c == '\0') return tem_texto;
        if (c == ';' || c < 32 || c == 127) return 0;
        if (c != ' ') tem_texto = 1;
    }
    return 0;
}

int disco_registro_valido(const info_caderneta *info) {
    return info != NULL && disco_nome_valido(info->nome_jogador)
        && info->partidas_jogadas >= 0 && info->maos_ganhas >= 0
        && info->trucos_pedidos >= 0 && info->trucos_corridos >= 0
        && info->trucos_recebidos >= info->trucos_corridos;
}

/* Formato: nome;partidas;maos;pedidos;corridos;recebidos.
 * Mantem o percurso pre-ordem de Artur, incluindo o contador novo.
 */
int gravar_preOrd(ArvAVL* raiz, FILE* pcaderneta) {
    if (raiz == NULL || pcaderneta == NULL) return 0;
    if (*raiz == NULL) return 1;
    info_caderneta *info = &(*raiz)->info;
    if (!disco_registro_valido(info)) return 0;
    if (fprintf(pcaderneta, "%s;%d;%d;%d;%d;%d\n", info->nome_jogador,
                info->partidas_jogadas, info->maos_ganhas, info->trucos_pedidos,
                info->trucos_corridos, info->trucos_recebidos) < 0) return 0;
    return gravar_preOrd(&(*raiz)->esq, pcaderneta)
        && gravar_preOrd(&(*raiz)->dir, pcaderneta);
}

int salvar_caderneta(ArvAVL* raiz) {
    if (raiz == NULL) return 0;
    FILE *arquivo = fopen(DISCO_ARQUIVO_TEMPORARIO, "w");
    if (arquivo == NULL) return 0;
    int ok = gravar_preOrd(raiz, arquivo);
    if (fflush(arquivo) != 0) ok = 0;
    if (fclose(arquivo) != 0) ok = 0;
    if (ok) {
        /* Substitui somente depois de concluir a escrita e fechar o arquivo.
         * No Windows, rename nao substitui um destino existente.
         */
#ifdef _WIN32
        ok = MoveFileExA(DISCO_ARQUIVO_TEMPORARIO, DISCO_ARQUIVO_CADERNETA,
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
        ok = rename(DISCO_ARQUIVO_TEMPORARIO, DISCO_ARQUIVO_CADERNETA) == 0;
#endif
    }
    if (!ok) remove(DISCO_ARQUIVO_TEMPORARIO);
    return ok;
}

/* Le exatamente os seis campos. Arquivos antigos sem recebidos precisam ser
 * corrigidos antes da carga: nao e possivel deduzir esse numero pelos demais.
 */
int disco_ler_registro(char *linha, info_caderneta *registro) {
    char *campos[6];
    char *atual = linha;
    for (int i = 0; i < 6; i++) {
        campos[i] = atual;
        char *separador = strchr(atual, ';');
        if (i == 5) {
            if (separador != NULL) return 0;
        } else {
            if (separador == NULL) return 0;
            *separador = '\0';
            atual = separador + 1;
        }
    }
    if (!disco_nome_valido(campos[0])) return 0;
    strcpy(registro->nome_jogador, campos[0]);
    int *contadores[5] = {&registro->partidas_jogadas, &registro->maos_ganhas,
        &registro->trucos_pedidos, &registro->trucos_corridos, &registro->trucos_recebidos};
    for (int i = 0; i < 5; i++) {
        char *fim;
        errno = 0;
        long valor = strtol(campos[i + 1], &fim, 10);
        if (errno == ERANGE || fim == campos[i + 1] || *fim != '\0'
            || valor < 0 || valor > INT_MAX) return 0;
        *contadores[i] = (int)valor;
    }
    return disco_registro_valido(registro);
}

int ler_caderneta_preOrd(ArvAVL* raiz, FILE* arquivo) {
    if (raiz == NULL || arquivo == NULL) return 0;
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *fim = strchr(linha, '\n');
        if (fim == NULL && !feof(arquivo)) return 0;
        if (fim != NULL) *fim = '\0';
        size_t tamanho = strlen(linha);
        if (tamanho > 0 && linha[tamanho - 1] == '\r') linha[--tamanho] = '\0';
        if (tamanho == 0) continue;
        info_caderneta registro = {0};
        if (!disco_ler_registro(linha, &registro) || !inserir(raiz, registro)) return 0;
    }
    return !ferror(arquivo);
}

/* Carrega em outra AVL. Falha preserva a arvore atual e o arquivo original. */
int carregar_caderneta(ArvAVL* raiz) {
    if (raiz == NULL) return DISCO_ERRO;
    FILE *arquivo = fopen(DISCO_ARQUIVO_CADERNETA, "r");
    if (arquivo == NULL) return errno == ENOENT ? DISCO_AUSENTE : DISCO_ERRO;
    ArvAVL *carregada = criarAVL();
    if (carregada == NULL) {
        fclose(arquivo);
        return DISCO_ERRO;
    }
    int ok = ler_caderneta_preOrd(carregada, arquivo);
    if (fclose(arquivo) != 0) ok = 0;
    if (!ok) {
        liberaABB(carregada);
        return DISCO_ERRO;
    }
    liberaNO(*raiz);
    *raiz = *carregada;
    free(carregada);
    return DISCO_CARREGADO;
}

/* Dados simples evitam dependencia circular entre disco.h e Partida.h. */
int gravar_historico(int numero_mao, int dupla_vencedora, int pontos,
                     const int placar[3]) {
    if (placar == NULL || numero_mao <= 0 || dupla_vencedora < 1
        || dupla_vencedora > 2 || placar[1] < 0 || placar[2] < 0
        || (pontos != 1 && pontos != 3 && pontos != 6 && pontos != 12)) return 0;
    FILE *arquivo = fopen(DISCO_ARQUIVO_HISTORICO, "a");
    if (arquivo == NULL) return 0;
    int ok = fprintf(arquivo, "\n%d;Dupla %d;+%d;%dx%d\n", numero_mao,
                     dupla_vencedora, pontos, placar[1], placar[2]) >= 0;
    if (fflush(arquivo) != 0) ok = 0;
    if (fclose(arquivo) != 0) ok = 0;
    return ok;
}

int ler_historico(void) {
    FILE *arquivo = fopen(DISCO_ARQUIVO_HISTORICO, "r");
    if (arquivo == NULL) return errno == ENOENT ? DISCO_AUSENTE : DISCO_ERRO;
    char linha[200], dupla_vencedora[30];
    int numero_mao, pontos, placar1, placar2;
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (sscanf(linha, "%d;%29[^;];+%d;%dx%d", &numero_mao, dupla_vencedora,
                   &pontos, &placar1, &placar2) == 5) {
            printf("Mao %d | %s (+%d pt) | Placar: %d x %d\n",
                   numero_mao, dupla_vencedora, pontos, placar1, placar2);
        }
    }
    int ok = !ferror(arquivo);
    if (fclose(arquivo) != 0) ok = 0;
    return ok ? DISCO_CARREGADO : DISCO_ERRO;
}

#endif
