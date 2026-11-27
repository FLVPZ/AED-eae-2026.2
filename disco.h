#include <stdio.h>
#include <string.h>
#include "caderneta.h"
#include "Partida.h"

#define historico "historico.txt"
#define caderneta "caderneta.txt"

void gravar_preOrd(ArvAVL* raiz, FILE* pcaderneta) {
    if (raiz == NULL || pcaderneta == NULL) return;

    if (*raiz != NULL) {
        fprintf(pcaderneta, "%s;%d;%d;%d;%d\n",
                (*raiz)->info.nome_jogador,
                (*raiz)->info.partidas_jogadas,
                (*raiz)->info.maos_ganhas,
                (*raiz)->info.trucos_pedidos,
                (*raiz)->info.trucos_corridos);

        gravar_preOrd(&((*raiz)->esq), pcaderneta);
        gravar_preOrd(&((*raiz)->dir), pcaderneta);
    }
}

int salvar_caderneta(ArvAVL* raiz) {
    if (raiz == NULL) return 0;

    FILE *pcaderneta = fopen(caderneta, "w");
    if (pcaderneta == NULL) {
        return 0;
    }

    gravar_preOrd(raiz, pcaderneta);

    fclose(pcaderneta);
    return 1;
}

void ler_caderneta_preOrd(ArvAVL* raiz, FILE* pcaderneta) {
    char linha[200];  
	info_caderneta registro;

    if (raiz == NULL || pcaderneta == NULL) return;

    while (fgets(linha, sizeof(linha), pcaderneta) != NULL) {
        int campos = sscanf(linha, "%49[^;];%d;%d;%d;%d",
                                  registro.nome_jogador,
                                  &registro.partidas_jogadas,
                                  &registro.maos_ganhas,
                                  &registro.trucos_pedidos,
                                  &registro.trucos_corridos);

        if (campos == 5) {
            inserir(raiz, registro);
        }
    }
}

int carregar_caderneta(ArvAVL* raiz) {
    if (raiz == NULL) return 0;

    FILE *pcaderneta = fopen(caderneta, "r");
    if (pcaderneta == NULL) {
        return 0;
    }

    ler_caderneta_preOrd(raiz, pcaderneta);

    fclose(pcaderneta);
    return 1;
}

int gravar_historico(tp_mao_atual *mao, int placar[3]) {
    if (mao == NULL || placar == NULL) return 0;

    FILE *phistorico = fopen(historico, "a");
    if (phistorico == NULL) {
        return 0;
    }

    fprintf(phistorico, "%d;Dupla %d;+%d;%dx%d\n",
            mao->numero,
            mao->dupla_vencedora,
            mao->pontos,
            placar[1],
            placar[2]);

    fclose(phistorico);
    return 1;
}

int ler_historico(void) {
    FILE *phistorico = fopen(historico, "r");
    if (phistorico == NULL) {
        return 0;
    }

    char linha[200]; int numero_mao = 0; char dupla_vencedora[30];
    int pontos = 0; int placar_dupla1 = 0; int placar_dupla2 = 0;

    while (fgets(linha, sizeof(linha), phistorico) != NULL) {
        int campos = sscanf(linha, "%d;%29[^;];+%d;%dx%d",
                                  &numero_mao,
                                  dupla_vencedora,
                                  &pontos,
                                  &placar_dupla1,
                                  &placar_dupla2);

        if (campos == 5) {
            printf("Mao %d | %s (+%d pt) | Placar: %d x %d\n",
                   numero_mao,
                   dupla_vencedora,
                   pontos,
                   placar_dupla1,
                   placar_dupla2);
        }
    }

    fclose(phistorico);
    return 1;
}