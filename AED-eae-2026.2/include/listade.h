#ifndef LISTADE_H
#define LISTADE_H
#include "STACK.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int mao;
    int rodada;
    tp_item cartas_jogadas[4];
    int aposta[3]; // POSIÇÃO 0 = Valor da ultima aposta pedida (-1 valor padrão para aposta 0); POSIÇÃO 1 = dupla que pediu a aposta; POSIÇÃO 2 = Outra dupla aceitou, 1->N, 0->S.
	int dupla_vencedora;
    int manilha;
} tp_item_VAR;

typedef struct tp_no_aux{
	struct tp_no_aux *ant;
	tp_item_VAR info;
	struct tp_no_aux *prox;
} tp_no_de;

typedef struct{
	tp_no_de *ini;
	tp_no_de *fim;
} tp_listad;

tp_listad *inicializa_listad(){
	tp_listad *lista=(tp_listad*) malloc(sizeof(tp_listad));
	lista->fim = NULL;
	lista->ini = NULL;
	return lista;
}

int listad_vazia(tp_listad *lista){
	if((lista->ini == NULL))
		return 1;
	return 0;
}

tp_no_de *aloca(){
	tp_no_de* pt;
	pt=(tp_no_de*) malloc(sizeof(tp_no_de));
	return pt;
}



int insere_no_VAR (tp_listad *lista, tp_item_VAR e){
	tp_no_de *novo;
	novo=aloca();
	if(!novo) return 0;
	novo->info=e;
	novo->prox=NULL;
	if(listad_vazia(lista)){
		novo->ant=NULL;
		lista->ini = lista->fim = novo;	
	} else{
		novo->ant = lista->fim;
		lista->fim->prox = novo;
		lista->fim=novo; 
	}
	return 1;
}

int tamanho_var(tp_listad *lista){
    if(lista == NULL)
        return 0;

    int tamanho = 0;
    tp_no_de *atu = lista->ini;

    while(atu != NULL){
        tamanho++;
        atu = atu->prox;
    }

    return tamanho;
}


//Chamar essa função ao final de toda mesa, apos os jogadores verem o VAR final do jogo.
tp_listad * Destroi_listad (tp_listad *lista){   
  tp_no_de *atu;
  atu = lista->ini;
  while (atu != NULL)  { 
        lista->ini = atu->prox;
        free(atu);
        atu=lista->ini;
		}
  free(lista);
  return NULL;
}  



#endif
