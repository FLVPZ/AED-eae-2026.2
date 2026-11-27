#ifndef VAR_H
#define VAR_H
#include "STACK.h"
#include "listade.h"
#include "Jogador.h"
#include <stdio.h>
#include "interface.h"

//Chamar todo inicio de mão
void inicializa_variavel_var(tp_item_VAR *e){
    memset(e, 0, sizeof(*e));
    e->mao = -1;
    e->rodada = -1;
    e->dupla_vencedora=-1;
    e->manilha = -1;
    e->pontos_em_jogo = 1;
    e->em_andamento = 1;
    for (int i = 0; i < 4; i++) {
        e->cartas_jogadas[i].naipe = 'A';
        e->cartas_jogadas[i].valor = -1;
    }

    for (int i = 0; i < 3; i++) {
        e->aposta[i] = -1;
      
    }
}

/* Guarda a carta junto do nome no instante da jogada, antes de girar a fila. */
void registrar_jogada_var(tp_item_VAR *e, int posicao, const char *nome, tp_item carta) {
    if (e == NULL || nome == NULL || posicao < 0 || posicao >= 4) return;
    snprintf(e->jogadores[posicao], sizeof(e->jogadores[posicao]), "%s", nome);
    e->cartas_jogadas[posicao] = carta;
}

// Registra cada resposta valida antes de a aposta mudar de estado.
void registrar_negociacao_var(tp_item_VAR *e, const char *pediu, int dupla,
                              const char *respondeu, int valor, char resposta) {
    if (e == NULL || e->qtd_negociacoes >= 3) return;
    tp_negociacao_VAR *n = &e->negociacoes[e->qtd_negociacoes++];
    snprintf(n->pediu, sizeof(n->pediu), "%s", pediu);
    snprintf(n->respondeu, sizeof(n->respondeu), "%s", respondeu);
    n->dupla_pediu = dupla;
    n->valor = valor;
    n->resposta = resposta;
}

// imprime o valor de uma carta (1 = As, 11 = J, 12 = Q, 13 = K)
void imprimir_valor(int valor){
    switch (valor) {
        case 1:  printf("As"); break;
        case 11: printf("J");  break;
        case 12: printf("Q");  break;
        case 13: printf("K");  break;
        default: printf("%d", valor);
    }
}

void imprimir_carta(tp_item carta) {
    // carta ainda nao jogada (rodada interrompida por corrido)
    if (carta.valor < 1) {
        printf("(nao jogou)");
        return;
    }

    imprimir_valor(carta.valor);
    printf(" de ");

    switch (carta.naipe) {
        case 'E': printf("Espadas"); break;
        case 'C': printf("Copas");   break;
        case 'O': printf("Ouros");   break;
        case 'P': printf("Paus");    break;
    }
}

void imprimir_var(tp_no_de *atu){
	ui_titulo("VAR DO BAR  |  HISTORICO DA PARTIDA");
    if (atu->info.em_andamento) printf("RODADA EM ANDAMENTO - somente cartas ja jogadas.\n");
	printf("Numero da rodada: %d\n", atu->info.rodada);
	printf("Numero da mao: %d\n", atu->info.mao);
	printf("Cartas jogadas: \n");
		for(int j = 0; j < 4; j++){
			printf("  %s: ", atu->info.jogadores[j][0] ? atu->info.jogadores[j] : "Jogador");
			imprimir_carta(atu->info.cartas_jogadas[j]);
            printf("\n");
		}
		printf("\n");
		printf("A manilha da rodada foi: ");
		imprimir_valor(atu->info.manilha);
        printf("\n");
    
    if (atu->info.qtd_negociacoes == 0) {
        printf("Nenhum novo pedido de aposta nesta rodada.\n");
    }
    for (int i = 0; i < atu->info.qtd_negociacoes; i++) {
        const tp_negociacao_VAR *n = &atu->info.negociacoes[i];
        printf("%s (Dupla %d) pediu %d; %s %s\n", n->pediu, n->dupla_pediu,
               n->valor, n->respondeu, n->resposta == 'A' ? "aceitou." :
               n->resposta == 'C' ? "correu." : "aumentou.");
    }
    if (atu->info.aposta[0] > 0) printf("Valor registrado da aposta: %d\n", atu->info.aposta[0]);
    printf("Valor aceito da mao: %d ponto(s)\n", atu->info.pontos_em_jogo);
    if (atu->info.em_andamento) return;
    if (atu->info.encerrada_por_fuga) {
        printf("Mao encerrada por fuga: Dupla %d recebeu %d ponto(s).\n",
               atu->info.dupla_vencedora, atu->info.pontos_em_jogo);
        return;
    }

	if(atu->info.dupla_vencedora == 0)
		printf("Rodada empatada\n");
	else
		printf("Dupla ganhadora da rodada: %d\n", atu->info.dupla_vencedora);
}

// atual = posição (0 = primeira) de onde o VAR está agora; mudar = 0 mostra a atual, 1 mostra a próxima, -1 a anterior
void mostra_var(tp_listad *lista, int atual, int mudar) {
	if (lista==NULL || lista->ini==NULL)
		printf("Lista nao inicializada ou vazia");
	else {
    	tp_no_de *atu;     
        atu = lista->ini;
        for (int i=0; i<atual && atu->prox != NULL; i++){
			atu=atu->prox;
		}
		if(mudar==1 && atu->prox!=NULL){
			atu=atu->prox;
		} else if(mudar==-1 && atu->ant!=NULL){
			atu=atu->ant;
		}
		imprimir_var(atu);
    }
    printf("\n");
}

//Chamar para adicionar o numero da rodada na variavel que vai ao VAR
void rodada_var(tp_item_VAR *e, int rodada){
    e->rodada = rodada;
}

//Chamar para adicionar o numero da mão na variavel que vai ao VAR
void mao_var(tp_item_VAR *e, int mao){
    e->mao = mao;
}

//Chamar para adicionar as apostas da rodada na variavel que vai ao VAR
void aposta_var (tp_item_VAR *e, int dupla_pediu, int aposta ,int dupla_aceitou){
    e->aposta[0]=aposta;
    e->aposta[1]=dupla_pediu;
    e->aposta[2]=dupla_aceitou;
}

//Chamar para adicionar a dupla vencedora da rodada na variavel que vai ao VAR
void dupla_vencedora_var (tp_item_VAR *e, int dupla_vencedora){
    e->dupla_vencedora=dupla_vencedora;
}

//Chamar para adicionar as cartas e a manilha da rodada na variavel que vai ao VAR
void cartas_var(tp_item_VAR *e, tp_item cartas[4], int manilha){
    for (int i = 0; i < 4; i++){
        e->cartas_jogadas[i] = cartas[i];
    }
    e->manilha = manilha;
      
}

//Chamar para adicionar a variavel que vai ao VAR no VAR
int adiciona_var (tp_item_VAR *e, tp_listad *lista){
    e->em_andamento = 0;
    if(insere_no_VAR (lista, *e)==0) return 0;
    inicializa_variavel_var(e);
    return 1;

}


//Chamar para a visualização do VAR
void ver_var_com_rodada(tp_listad *var, const tp_item_VAR *parcial){
    int cont = 1;
    int atual = 0;
    int escolha;
    int salvas = tamanho_var(var);
    int tam_VAR = salvas + (parcial != NULL);
    if (parcial) atual = salvas;

    if (tam_VAR == 0){
        printf("\nO VAR ainda nao tem nenhuma rodada registrada.\n");
        return;
    }

    char aviso[100] = "";
    while (cont==1){
        ui_limpar();
        if (parcial && atual == salvas) {
            tp_no_de consulta = {0};
            consulta.info = *parcial;
            imprimir_var(&consulta);
        } else mostra_var(var, atual, 0);
        if (aviso[0]) printf("%s\n", aviso);
        aviso[0] = '\0';
        ui_secao("CONTROLE DO VAR");
        printf("Rodada %d de %d no VAR\n", atual + 1, tam_VAR);
        printf("  [1] Voltar ao jogo  [2] Proxima rodada  [3] Rodada anterior\n  Sua escolha: ");
        int leitura = scanf("%d", &escolha);
        if (leitura == EOF) return;
        if (leitura != 1){
            escolha = 0; // entrada que nao é numero
        }
        limpar_buffer();
        if(escolha==1){
            printf("Saindo do VAR.\n");
            break;
        } else if (escolha == 2){
            if(atual >= tam_VAR - 1){
                snprintf(aviso, sizeof(aviso), "Fim do VAR: nao ha proxima rodada.");
            } else {
                atual++;
            }
        } else if(escolha == 3){
            if(atual==0){
                snprintf(aviso, sizeof(aviso), "Inicio do VAR: nao ha rodada anterior.");
            } else {
                atual--;
            }
        } else {
            snprintf(aviso, sizeof(aviso), "Opcao invalida, digite um numero valido.");
        }
    }

}

void ver_var(tp_listad *var) {
    ver_var_com_rodada(var, NULL);
}

#endif
