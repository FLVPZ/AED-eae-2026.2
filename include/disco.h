#ifndef DISCO_H
#define DISCO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include "caderneta.h"
#include "listade.h"
#include "interface.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define DISCO_CAMINHO_MAX 4096
static char DISCO_ARQUIVO_CADERNETA[DISCO_CAMINHO_MAX] = "caderneta.txt";
static char DISCO_ARQUIVO_TEMPORARIO[DISCO_CAMINHO_MAX] = "caderneta.txt.tmp";
static char DISCO_ARQUIVO_HISTORICO[DISCO_CAMINHO_MAX] = "historico.txt";
static char disco_diretorio[DISCO_CAMINHO_MAX] = ".";
static char disco_ultimo_erro[1024] = "";

#define DISCO_CARREGADO 1
#define DISCO_AUSENTE 0
#define DISCO_ERRO -1

void disco_falha(const char *arquivo, const char *operacao) {
    snprintf(disco_ultimo_erro, sizeof(disco_ultimo_erro),
             "%s: %.500s\nMotivo: %s", operacao, arquivo, strerror(errno ? errno : EIO));
}

/* No Windows, a IDE pode iniciar por outra pasta: dados ficam junto do exe.
 * Nos demais sistemas, preserva a pasta escolhida no terminal.
 */
int disco_iniciar(void) {
#ifdef _WIN32
    DWORD n = GetModuleFileNameA(NULL, disco_diretorio, sizeof(disco_diretorio));
    if (!n || n >= sizeof(disco_diretorio)) return 0;
    char *fim = strrchr(disco_diretorio, '\\');
    if (fim == NULL) return 0;
    fim[1] = '\0';
#else
    if (getcwd(disco_diretorio, sizeof(disco_diretorio)) == NULL) return 0;
    size_t n = strlen(disco_diretorio);
    if (n + 1 >= sizeof(disco_diretorio)) return 0;
    if (n == 0 || disco_diretorio[n - 1] != '/') strcat(disco_diretorio, "/");
#endif
    const char *nomes[3] = {"caderneta.txt", "caderneta.txt.tmp", "historico.txt"};
    char *destinos[3] = {DISCO_ARQUIVO_CADERNETA, DISCO_ARQUIVO_TEMPORARIO, DISCO_ARQUIVO_HISTORICO};
    for (int i = 0; i < 3; i++) {
        int tamanho = snprintf(destinos[i], DISCO_CAMINHO_MAX, "%s%s", disco_diretorio, nomes[i]);
        if (tamanho < 0 || tamanho >= DISCO_CAMINHO_MAX) return 0;
    }
    return 1;
}

int disco_fechar_escrita(FILE *arquivo, int ok, const char *caminho) {
    if (!ok) disco_falha(caminho, "Falha ao escrever");
    if (fflush(arquivo) != 0) { ok = 0; disco_falha(caminho, "Falha ao concluir a escrita"); }
    if (fclose(arquivo) != 0) { ok = 0; disco_falha(caminho, "Falha ao fechar"); }
    return ok;
}

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
    if (!disco_registro_valido(info)) { errno = EINVAL; return 0; }
    if (fprintf(pcaderneta,
                "\n# Jogador: %s\n# Partidas jogadas: %d\n# Maos ganhas: %d\n"
                "# Trucos pedidos: %d\n# Trucos recebidos: %d\n# Trucos corridos (fugas): %d\n",
                info->nome_jogador, info->partidas_jogadas, info->maos_ganhas,
                info->trucos_pedidos, info->trucos_recebidos, info->trucos_corridos) < 0) return 0;
    if (info->trucos_recebidos) {
        if (fprintf(pcaderneta, "# Taxa de fuga: %.2f%%\n",
                    100.0 * info->trucos_corridos / info->trucos_recebidos) < 0) return 0;
    } else if (fputs("# Taxa de fuga: sem pedidos recebidos\n", pcaderneta) == EOF) return 0;
    if (fprintf(pcaderneta, "%s;%d;%d;%d;%d;%d\n", info->nome_jogador,
                info->partidas_jogadas, info->maos_ganhas, info->trucos_pedidos,
                info->trucos_corridos, info->trucos_recebidos) < 0) return 0;
    return gravar_preOrd(&(*raiz)->esq, pcaderneta)
        && gravar_preOrd(&(*raiz)->dir, pcaderneta);
}

int salvar_caderneta(ArvAVL* raiz) {
    if (raiz == NULL) { errno = EINVAL; disco_falha(DISCO_ARQUIVO_CADERNETA, "Caderneta invalida"); return 0; }
    FILE *arquivo = fopen(DISCO_ARQUIVO_TEMPORARIO, "w");
    if (arquivo == NULL) { disco_falha(DISCO_ARQUIVO_TEMPORARIO, "Nao foi possivel abrir para salvar"); return 0; }
    int ok = fputs("# CADERNETA DO BAR DO PATO\n"
                   "# Pedidos incluem truco, seis e doze. Fuga = corridos / recebidos x 100.\n"
                   "# Partidas contam ao terminar. Estatisticas de truco sao salvas a cada acao.\n"
                   "# Dados da AVL: nome | partidas | maos | pedidos | corridos | recebidos\n", arquivo) != EOF;
    if (ok) ok = gravar_preOrd(raiz, arquivo);
    ok = disco_fechar_escrita(arquivo, ok, DISCO_ARQUIVO_CADERNETA);
    if (ok) {
        /* Substitui somente depois de concluir a escrita e fechar o arquivo.
         * No Windows, rename nao substitui um destino existente.
         */
#ifdef _WIN32
        ok = MoveFileExA(DISCO_ARQUIVO_TEMPORARIO, DISCO_ARQUIVO_CADERNETA,
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
        if (!ok) {
            DWORD codigo = GetLastError();
            char motivo[300] = "";
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL, codigo, 0, motivo, sizeof(motivo), NULL);
            snprintf(disco_ultimo_erro, sizeof(disco_ultimo_erro),
                     "Falha ao substituir %.500s\nWindows %lu: %s", DISCO_ARQUIVO_CADERNETA,
                     (unsigned long)codigo, motivo);
        }
#else
        ok = rename(DISCO_ARQUIVO_TEMPORARIO, DISCO_ARQUIVO_CADERNETA) == 0;
        if (!ok) disco_falha(DISCO_ARQUIVO_CADERNETA, "Falha ao substituir");
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
        if (tamanho == 0 || (linha[0] == '#' && strchr(linha, ';') == NULL)) continue;
        info_caderneta registro = {0};
        if (!disco_ler_registro(linha, &registro) || !inserir(raiz, registro)) return 0;
    }
    return !ferror(arquivo);
}

/* Carrega em outra AVL. Falha preserva a arvore atual e o arquivo original. */
int carregar_caderneta(ArvAVL* raiz) {
    if (raiz == NULL) return DISCO_ERRO;
    FILE *arquivo = fopen(DISCO_ARQUIVO_CADERNETA, "r");
    if (arquivo == NULL) {
        if (errno == ENOENT) return DISCO_AUSENTE;
        disco_falha(DISCO_ARQUIVO_CADERNETA, "Nao foi possivel carregar a caderneta");
        return DISCO_ERRO;
    }
    ArvAVL *carregada = criarAVL();
    if (carregada == NULL) {
        fclose(arquivo);
        return DISCO_ERRO;
    }
    int ok = ler_caderneta_preOrd(carregada, arquivo);
    if (fclose(arquivo) != 0) ok = 0;
    if (!ok) {
        errno = EINVAL;
        disco_falha(DISCO_ARQUIVO_CADERNETA, "Caderneta invalida ou incompleta. Arquivo preservado");
        liberaABB(carregada);
        return DISCO_ERRO;
    }
    liberaNO(*raiz);
    *raiz = *carregada;
    free(carregada);
    return DISCO_CARREGADO;
}

FILE *disco_abrir_historico(void) {
    FILE *arquivo = fopen(DISCO_ARQUIVO_HISTORICO, "a");
    if (arquivo == NULL) disco_falha(DISCO_ARQUIVO_HISTORICO, "Nao foi possivel abrir o historico");
    return arquivo;
}

int iniciar_historico(tp_fila mesa) {
    FILE *arquivo = disco_abrir_historico();
    if (arquivo == NULL) return 0;
    time_t agora = time(NULL);
    struct tm *data = localtime(&agora);
    char horario[40] = "horario indisponivel";
    if (data) strftime(horario, sizeof(horario), "%d/%m/%Y %H:%M:%S", data);
    int ok = fprintf(arquivo, "\n=== NOVA PARTIDA - BAR DO PATO ===\nInicio: %s\n", horario) >= 0;
    tp_jogador j;
    while (removeFila(&mesa, &j)) {
        if (fprintf(arquivo, "Dupla %d - Jogador: %s\n", j.dupla, j.nome) < 0) ok = 0;
    }
    if (fputs("As maos concluidas sao registradas abaixo; sair antes nao conclui a partida.\n", arquivo) == EOF) ok = 0;
    return disco_fechar_escrita(arquivo, ok, DISCO_ARQUIVO_HISTORICO);
}

/* Salva os contadores tambem durante a negociacao, sem esperar acabar a mao. */
int salvar_evento_truco(ArvAVL *raiz, const char *pediu, int dupla,
                         const char *respondeu, int valor, char resposta) {
    int ok = salvar_caderneta(raiz);
    char erro_caderneta[sizeof(disco_ultimo_erro)] = "";
    if (!ok) snprintf(erro_caderneta, sizeof(erro_caderneta), "%s", disco_ultimo_erro);
    FILE *arquivo = disco_abrir_historico();
    int historico = 0;
    if (arquivo != NULL) {
        historico = fprintf(arquivo, "TRUCO | %s (Dupla %d) pediu %d | %s: %s\n",
                            pediu, dupla, valor, respondeu,
                            resposta == 'P' ? "aguardando resposta" : resposta == 'A' ? "aceitou" :
                            resposta == 'C' ? "correu" : "aumentou") >= 0;
        historico = disco_fechar_escrita(arquivo, historico, DISCO_ARQUIVO_HISTORICO);
    }
    if (!ok) {
        char erro_historico[sizeof(disco_ultimo_erro)] = "";
        if (!historico) snprintf(erro_historico, sizeof(erro_historico), "%s", disco_ultimo_erro);
        snprintf(disco_ultimo_erro, sizeof(disco_ultimo_erro), "%.490s%s%.490s",
                 erro_caderneta, historico ? "" : "\n", erro_historico);
    }
    return ok && historico;
}

int gravar_rodada_historico(const tp_item_VAR *registro) {
    FILE *arquivo = disco_abrir_historico();
    if (arquivo == NULL) return 0;
    int ok = fprintf(arquivo, "\nVAR | Mao %d | Rodada %d | Manilha: %s | Valor aceito: %d\n",
                     registro->mao, registro->rodada, ui_valor(registro->manilha), registro->pontos_em_jogo) >= 0;
    for (int i = 0; i < 4; i++) {
        tp_item carta = registro->cartas_jogadas[i];
        if (carta.valor < 1) {
            if (fprintf(arquivo, "  %s: nao jogou\n", registro->jogadores[i]) < 0) ok = 0;
        } else if (fprintf(arquivo, "  %s: %s de %s\n", registro->jogadores[i], ui_valor(carta.valor), ui_naipe(carta.naipe)) < 0) ok = 0;
    }
    if (fprintf(arquivo, "  %s: Dupla %d (0 = empate)\n",
                registro->encerrada_por_fuga ? "Mao encerrada por fuga" : "Vencedora da rodada",
                registro->dupla_vencedora) < 0) ok = 0;
    return disco_fechar_escrita(arquivo, ok, DISCO_ARQUIVO_HISTORICO);
}

/* Registra o resumo de pontos da mao no historico. */
int gravar_historico(int numero_mao, int dupla_vencedora, int pontos,
                     const int placar[3]) {
    if (placar == NULL || numero_mao <= 0 || dupla_vencedora < 1
        || dupla_vencedora > 2 || placar[1] < 0 || placar[2] < 0
        || (pontos != 1 && pontos != 3 && pontos != 6 && pontos != 12)) return 0;
    FILE *arquivo = disco_abrir_historico();
    if (arquivo == NULL) return 0;
    int ok = fprintf(arquivo, "\n%d;Dupla %d;+%d;%dx%d\n", numero_mao,
                     dupla_vencedora, pontos, placar[1], placar[2]) >= 0;
    return disco_fechar_escrita(arquivo, ok, DISCO_ARQUIVO_HISTORICO);
}

int encerrar_historico(const int placar[3], int maior_mao, int numero_maior_mao) {
    FILE *arquivo = disco_abrir_historico();
    if (arquivo == NULL) return 0;
    int ok = fprintf(arquivo, "FIM DA PARTIDA | Dupla %d venceu | Placar: %d x %d\n"
                     "Mao mais cara: %d ponto(s), mao %d.\n",
                     placar[1] >= 12 ? 1 : 2, placar[1], placar[2], maior_mao, numero_maior_mao) >= 0;
    return disco_fechar_escrita(arquivo, ok, DISCO_ARQUIVO_HISTORICO);
}

#endif
