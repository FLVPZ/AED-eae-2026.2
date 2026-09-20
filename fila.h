#ifndef FILA_H
#define FILA_H
#include <stdio.h>
#define MAX 5
#include "STACK.h"
#define NOME_MAX 30

// guarda os dados que precisam ficar juntos para cada jogador
typedef struct {
    char nome[NOME_MAX];
    tp_item mao[3];
    int dupla;
} tp_jogador;

// fila usada para manter a ordem dos jogadores na mesa
typedef struct{
	tp_jogador item[MAX];
	int ini, fim;	
}tp_fila;

// começa a fila vazia
void inicializaFila(tp_fila *f){
	f->ini = f->fim = MAX-1;
}

int filaVazia(tp_fila *f){
	if(f->ini==f->fim) return 1;
	return 0;
}

// volta para o começo do vetor quando chega na última posição
int proximo(int pos){
	if(pos==MAX-1) return 0;
	return ++pos;
}

int filaCheia (tp_fila *f){
	if(proximo(f->fim)==f->ini) return 1;
	return 0;
}

// coloca o jogador no final da fila
int insereFila(tp_fila *f, tp_jogador e){
	if(filaCheia(f)) return 0;
	f->fim = proximo(f->fim);
	f->item[f->fim] = e;
	return 1;
}

// retira o primeiro jogador da fila
int removeFila (tp_fila *f, tp_jogador*e){
	if(filaVazia(f)) return 0;
	f->ini= proximo(f->ini);
	*e = f->item[f->ini];
	return 1;
}

// retorna quantos jogadores estão na fila
int tamanhoFila(tp_fila *f){
	if (filaVazia(f)) return 0;
	if (f->ini < f->fim) return f->fim - f->ini;
	return MAX - 1 - f->ini + f->fim + 1;
}
#endif
