#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include/sinais_arquivo.h"

int main(int argc, char **argv) {
    assert(argc == 2);
    const char *caminho = argv[1];
    tp_jogador ana = {0}, bia = {0}, lido = {0};
    strcpy(ana.nome, "Ana Silva");
    strcpy(bia.nome, "Bia");
    sinais_padrao(&ana);
    sinais_padrao(&bia);
    assert(sinais_carregar(&ana, caminho) == 0);
    strcpy(ana.sinais[0], "levanta o copo");
    assert(sinais_salvar(&ana, caminho));
    strcpy(bia.sinais[1], "bate palma");
    assert(sinais_salvar(&bia, caminho));
    strcpy(ana.sinais[0], "toca no chapeu");
    assert(sinais_salvar(&ana, caminho));

    strcpy(lido.nome, "Ana Silva");
    lido.dupla = 2;
    lido.mao[0].valor = 7;
    assert(sinais_carregar(&lido, caminho) == 1);
    assert(strcmp(lido.sinais[0], "toca no chapeu") == 0);
    assert(lido.dupla == 2 && lido.mao[0].valor == 7);
    strcpy(lido.nome, "Bia");
    assert(sinais_carregar(&lido, caminho) == 1);
    assert(strcmp(lido.sinais[1], "bate palma") == 0);
    strcpy(lido.nome, "Caio");
    assert(sinais_carregar(&lido, caminho) == 0);
    assert(strcmp(lido.sinais[0], "pisca ;)") == 0);

    FILE *arquivo = fopen(caminho, "a");
    assert(arquivo != NULL);
    assert(fputs("S1\tAna Silva\t\tgesto\tgesto\n", arquivo) >= 0);
    for (int i = 0; i < 1000; i++) assert(fputc('x', arquivo) != EOF);
    assert(fputs("\nS1\tAna Silva\tregistro incompleto", arquivo) >= 0);
    assert(fclose(arquivo) == 0);
    strcpy(lido.nome, "Ana Silva");
    assert(sinais_carregar(&lido, caminho) == 1);
    assert(strcmp(lido.sinais[0], "toca no chapeu") == 0);

    /* Uma nova gravacao deve se recuperar do registro incompleto ao final. */
    memset(ana.sinais[0], 'a', SINAL_MAX - 1);
    ana.sinais[0][SINAL_MAX - 1] = '\0';
    assert(sinais_salvar(&ana, caminho));
    assert(sinais_carregar(&lido, caminho) == 1);
    assert(strcmp(lido.sinais[0], ana.sinais[0]) == 0);
    strcpy(ana.sinais[0], "nao\tpode");
    assert(!sinais_salvar(&ana, caminho));
    strcpy(ana.sinais[0], "   ");
    assert(!sinais_salvar(&ana, caminho));
    memset(ana.sinais[0], 'x', SINAL_MAX);
    assert(!sinais_salvar(&ana, caminho));
    assert(sinais_carregar(&lido, caminho) == 1);
    assert(strlen(lido.sinais[0]) == SINAL_MAX - 1);

    char invalido[1024];
    assert(strlen(caminho) + 10 < sizeof(invalido));
    sprintf(invalido, "%s/ausente", caminho);
    assert(!sinais_salvar(&lido, invalido));
    assert(sinais_carregar(&lido, invalido) == -1);
    assert(!sinais_salvar(NULL, caminho));
    assert(!sinais_salvar(&lido, NULL));
    assert(sinais_carregar(NULL, caminho) == -1);
    assert(sinais_carregar(&lido, NULL) == -1);
    puts("OK: leitura/escrita, atualizacao por nome, limites e registros incompletos/invalidos.");
    return 0;
}
