#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef _WIN32
static int falhar_troca = 0;
static int trocar_arquivo(const char *origem, const char *destino) {
    if (falhar_troca) { errno = EACCES; return -1; }
    return rename(origem, destino);
}
#define rename trocar_arquivo
#endif
#include "../include/disco.h"
#include "../include/disco.h" /* inclusao repetida precisa compilar */
#ifndef _WIN32
#undef rename
#endif

static void escrever(const char *texto) {
    FILE *f = fopen(DISCO_ARQUIVO_CADERNETA, "w");
    assert(f != NULL);
    assert(fputs(texto, f) >= 0);
    assert(fclose(f) == 0);
}

static void ler(char *texto, size_t limite) {
    FILE *f = fopen(DISCO_ARQUIVO_CADERNETA, "r");
    assert(f != NULL);
    size_t n = fread(texto, 1, limite - 1, f);
    texto[n] = '\0';
    assert(!ferror(f));
    assert(fclose(f) == 0);
}

int main(void) {
    ArvAVL *caderneta = criarAVL(); /* nao pode colidir com macro de arquivo */
    assert(caderneta != NULL);
    assert(carregar_caderneta(caderneta) == DISCO_AUSENTE);
    assert(carregar_caderneta(NULL) == DISCO_ERRO);
    assert(!salvar_caderneta(NULL));
    assert(!disco_registro_valido(NULL));
    info_caderneta ana = {"Ana Silva", 3, 5, 7, 1, 4};
    info_caderneta bia = {"Bia", 2, 9, 4, 0, 2};
    assert(inserir(caderneta, ana));
    assert(inserir(caderneta, bia));
    assert(salvar_caderneta(caderneta));
    char antes[1024], depois[1024];
    ler(antes, sizeof(antes));
    assert(strstr(antes, "Ana Silva;3;5;7;1;4") != NULL);
    FILE *temp = fopen(DISCO_ARQUIVO_TEMPORARIO, "r");
    assert(temp == NULL);

    ArvAVL *recuperada = criarAVL();
    assert(recuperada != NULL);
    assert(acessar_jogador(recuperada, "Antigo") != NULL);
    assert(carregar_caderneta(recuperada) == DISCO_CARREGADO);
    assert(totalNOsABB(recuperada) == 2);
    assert(!consultarValorAVL(recuperada, "Antigo"));
    struct NO *j = acessar_jogador(recuperada, "Ana Silva");
    assert(j->info.partidas_jogadas == 3 && j->info.maos_ganhas == 5);
    assert(j->info.trucos_pedidos == 7 && j->info.trucos_corridos == 1);
    assert(j->info.trucos_recebidos == 4);
    assert(salvar_caderneta(recuperada)); /* substituicao de arquivo existente */
    ler(depois, sizeof(depois));
    assert(strcmp(antes, depois) == 0);

    j->info.trucos_recebidos = 8;
#ifndef _WIN32
    falhar_troca = 1;
    assert(!salvar_caderneta(recuperada));
    ler(depois, sizeof(depois));
    assert(strcmp(antes, depois) == 0);
    assert(j->info.trucos_recebidos == 8);
    falhar_troca = 0;
#endif
    j->info.trucos_recebidos = 0; /* contador inconsistente nao pode apagar o arquivo */
    assert(!salvar_caderneta(recuperada));
    ler(depois, sizeof(depois));
    assert(strcmp(antes, depois) == 0);
    j->info.trucos_recebidos = 4;

    const char *invalidos[] = {
        "Ana;3;5;7;1\n", /* formato antigo sem recebidos */
        "Ana;3;5;7;1;0\n", /* mais fugas que pedidos recebidos */
        "Ana;-1;5;7;1;4\n", "Ana;2147483648;5;7;1;4\n",
        "Ana;3;5;7;1;4;lixo\n", "Ana;3;5;7;1;4abc\n",
        "Ana;3;5;7;1;4\nAna;3;5;7;1;4\n", ";3;5;7;1;4\n"
    };
    for (size_t i = 0; i < sizeof(invalidos) / sizeof(invalidos[0]); i++) {
        escrever(invalidos[i]);
        struct NO *raiz_antes = *recuperada;
        assert(carregar_caderneta(recuperada) == DISCO_ERRO);
        assert(*recuperada == raiz_antes);
        assert(acessar_jogador(recuperada, "Ana Silva") == j);
        assert(j->info.trucos_recebidos == 4);
        ler(depois, sizeof(depois));
        assert(strcmp(depois, invalidos[i]) == 0);
    }
    escrever("Ana;3;5;7;1;4\r\nBia;2;9;4;0;2");
    assert(carregar_caderneta(recuperada) == DISCO_CARREGADO);
    assert(totalNOsABB(recuperada) == 2);
    assert(acessar_jogador(recuperada, "Ana")->info.trucos_recebidos == 4);

    int placar[3] = {0, 3, 0};
    assert(gravar_historico(1, 1, 3, placar));
    placar[2] = 1;
    assert(gravar_historico(2, 2, 1, placar));
    assert(!gravar_historico(0, 1, 3, placar));
    assert(!gravar_historico(1, 3, 3, placar));
    assert(!gravar_historico(1, 1, 2, placar));
    assert(!gravar_historico(1, 1, 3, NULL));
    assert(ler_historico() == DISCO_CARREGADO);
    liberaABB(caderneta);
    liberaABB(recuperada);
    puts("OK: disco preserva os cinco contadores, dados anteriores e historico por mao.");
    return 0;
}
