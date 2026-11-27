#define main main_do_jogo
#include "../main.c"
#undef main
#include <assert.h>

int main(int argc, char **argv) {
    assert(argc == 2);
    int aumentos = atoi(argv[1]);
    ArvAVL *cad = criarAVL(); assert(cad);
    tp_fila mesa;
    char nomes[4][NOME_MAX] = {"Ana", "Bia", "Caio", "Davi"};
    inicializaFila(&mesa); montar_mesa(&mesa, nomes);
    for (int i=0;i<4;i++) assert(acessar_jogador(cad,nomes[i]));
    tp_fila antes = mesa;
    tp_aposta aposta; aposta_iniciar(&aposta);
    tp_item_VAR registro; inicializa_variavel_var(&registro);
    tp_jogador atual; posicao_do_jogador(mesa,0,&atual);
    int vencedora=0,pontos=0;
    int resultado = negociar_truco(mesa,&aposta,atual,&vencedora,&pontos,cad,&registro);
    assert(memcmp(&mesa,&antes,sizeof(mesa))==0);
    assert(registro.qtd_negociacoes == aumentos+1);
    for (int i=0;i<=aumentos;i++) {
        const tp_negociacao_VAR *n=&registro.negociacoes[i];
        assert(n->valor==(i==0?3:i==1?6:12));
        assert(strcmp(n->pediu,i%2?"Bia":"Ana")==0);
        assert(strcmp(n->respondeu,i%2?"Ana":"Bia")==0);
        assert(n->resposta==(i<aumentos?'T':resultado==TRUCO_CORRIDO?'C':'A'));
    }
    info_caderneta ana=acessar_jogador(cad,"Ana")->info;
    info_caderneta bia=acessar_jogador(cad,"Bia")->info;
    assert(ana.trucos_pedidos == (aumentos==2?2:1));
    assert(bia.trucos_pedidos == (aumentos>0?1:0));
    assert(ana.trucos_recebidos == (aumentos>0?1:0));
    assert(bia.trucos_recebidos == (aumentos==2?2:1));
    assert(ana.trucos_corridos == (resultado==TRUCO_CORRIDO && aumentos==1));
    assert(bia.trucos_corridos == (resultado==TRUCO_CORRIDO && aumentos!=1));
    assert(acessar_jogador(cad,"Caio")->info.trucos_recebidos==0);
    assert(acessar_jogador(cad,"Davi")->info.trucos_recebidos==0);
    if(resultado==TRUCO_CORRIDO) assert(pontos==(aumentos==0?1:aumentos==1?3:6));
    registrar_dupla_vencedora(cad,mesa,1);
    assert(acessar_jogador(cad,"Ana")->info.maos_ganhas==1);
    assert(acessar_jogador(cad,"Caio")->info.maos_ganhas==1);
    assert(acessar_jogador(cad,"Bia")->info.maos_ganhas==0);
    registrar_partida_jogadores(cad,mesa);
    for(int i=0;i<4;i++) assert(acessar_jogador(cad,nomes[i])->info.partidas_jogadas==1);
    int maior=0; registrar_maior_mao(&maior,3); registrar_maior_mao(&maior,1);
    assert(maior==3); registrar_maior_mao(&maior,12); assert(maior==12);
    assert(memcmp(&mesa,&antes,sizeof(mesa))==0);
    liberaABB(cad);
    puts("OK: negociacao, pedidos/recebidos/fugas, dupla, partida e maior mao.");
    return 0;
}
