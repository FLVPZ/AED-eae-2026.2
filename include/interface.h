#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "STACK.h"
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif

/* Apenas apresentacao: nao consulta cartas ocultas nem altera o estado do jogo.
 * Sem terminal, TERM=dumb ou NO_COLOR, a saida continua legivel sem cores.
 */
static int interface_cores = 0;
static int interface_interativa = 0;
static int interface_largura = 72;
static char interface_ultimo_aviso[160] = "";

void ui_iniciar(void) {
    int saida_terminal;
    int entrada_terminal;
    int suporta_ansi = 1;
#ifdef _WIN32
    saida_terminal = _isatty(_fileno(stdout));
    entrada_terminal = _isatty(_fileno(stdin));
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo;
    suporta_ansi = GetConsoleMode(console, &modo)
        && SetConsoleMode(console, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(console, &info)) {
        int largura = info.srWindow.Right - info.srWindow.Left + 1;
        if (largura >= 36 && largura < interface_largura) interface_largura = largura;
    }
#else
    saida_terminal = isatty(STDOUT_FILENO);
    entrada_terminal = isatty(STDIN_FILENO);
    struct winsize tamanho;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &tamanho) == 0
        && tamanho.ws_col >= 36 && tamanho.ws_col < interface_largura) {
        interface_largura = tamanho.ws_col;
    }
#endif
    const char *term = getenv("TERM");
    if (term != NULL && strcmp(term, "dumb") == 0) suporta_ansi = 0;
    interface_cores = saida_terminal && suporta_ansi && getenv("NO_COLOR") == NULL;
    interface_interativa = saida_terminal && entrada_terminal && suporta_ansi;
}

void ui_cor(const char *cor) {
    if (interface_cores) fputs(cor, stdout);
}

void ui_limpar(void) {
    if (interface_interativa) fputs("\033[2J\033[H", stdout);
}

void ui_linha(void) {
    ui_cor("\033[33m");
    for (int i = 0; i < interface_largura; i++) putchar('=');
    ui_cor("\033[0m");
    putchar('\n');
}

void ui_titulo(const char *titulo) {
    ui_linha();
    ui_cor("\033[1;32m");
    printf("  %.*s\n", interface_largura - 2, titulo);
    ui_cor("\033[0m");
    ui_linha();
}

void ui_secao(const char *titulo) {
    ui_cor("\033[1;33m");
    printf("\n  %s\n", titulo);
    ui_cor("\033[0m");
}

void ui_aviso(const char *formato, ...) {
    va_list argumentos;
    va_start(argumentos, formato);
    vsnprintf(interface_ultimo_aviso, sizeof(interface_ultimo_aviso), formato, argumentos);
    va_end(argumentos);
}

void ui_mostrar_aviso(void) {
    if (interface_ultimo_aviso[0] == '\0') return;
    ui_cor("\033[36m");
    printf("  > %.*s\n", interface_largura - 4, interface_ultimo_aviso);
    ui_cor("\033[0m");
}

void ui_pausar(const char *mensagem) {
    if (!interface_interativa) return;
    char linha[128];
    printf("\n  %s [Enter] ", mensagem);
    fflush(stdout);
    if (fgets(linha, sizeof(linha), stdin) != NULL && strchr(linha, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
}

const char *ui_valor(int valor) {
    switch (valor) {
        case 1: return "A"; case 2: return "2"; case 3: return "3";
        case 4: return "4"; case 5: return "5"; case 6: return "6";
        case 7: return "7"; case 11: return "J"; case 12: return "Q";
        case 13: return "K"; default: return "?";
    }
}

const char *ui_naipe(char naipe) {
    switch (naipe) {
        case 'O': return "OUROS"; case 'E': return "ESPADAS";
        case 'C': return "COPAS"; case 'P': return "PAUS";
        default: return "?";
    }
}

void ui_cartas(const tp_item *cartas, int qtd, int manilha) {
    printf("  SUA MAO\n");
    if (interface_largura < 68) {
        for (int i = 0; i < qtd; i++) {
            printf("  [%d] %s de %s", i + 1, ui_valor(cartas[i].valor), ui_naipe(cartas[i].naipe));
            if (cartas[i].valor == manilha) {
                fputs(cartas[i].naipe == 'P' ? " [MANILHA - ZAP]" : " [MANILHA]", stdout);
            }
            putchar('\n');
        }
        return;
    }
    for (int linha = 0; linha < 5; linha++) {
        printf("  ");
        for (int i = 0; i < qtd; i++) {
            int eh_manilha = cartas[i].valor == manilha;
            int vermelho = cartas[i].naipe == 'O' || cartas[i].naipe == 'C';
            ui_cor(eh_manilha ? "\033[103;30m" : (vermelho ? "\033[107;31m" : "\033[107;30m"));
            if (linha == 0 || linha == 4) printf("+------------------+");
            if (linha == 1) printf("| %-16s |", ui_valor(cartas[i].valor));
            if (linha == 2) printf("|%*s%-*s|", 5, "", 13, ui_naipe(cartas[i].naipe));
            if (linha == 3) printf("| %16s |", ui_valor(cartas[i].valor));
            ui_cor("\033[0m");
            printf("  ");
        }
        putchar('\n');
    }
    printf("  ");
    for (int i = 0; i < qtd; i++) {
        char legenda[32];
        const char *marca = cartas[i].valor != manilha ? ""
            : (cartas[i].naipe == 'P' ? " [MANILHA - ZAP]" : " [MANILHA]");
        snprintf(legenda, sizeof(legenda), "[%d]%s", i + 1, marca);
        ui_cor(cartas[i].valor == manilha ? "\033[1;33m" : "\033[1;32m");
        printf("%-20s  ", legenda);
        ui_cor("\033[0m");
    }
    putchar('\n');
}

#endif
