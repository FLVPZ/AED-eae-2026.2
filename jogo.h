
#include <stdio.h>
#ifndef Distribuicao_h
#define Distribuicao_h
#include "STACK.h"
#include "baralho.h"
#include "Jogador.h"
#include "fila.h"

//Chama a biblioteca ja embaralhada
void distribute_carts(tp_stack *s, tp_fila *f)	{
	tp_item element;
	tp_jogador jogador;
	for(int i=0;i<3;i++)	{
		for(int j=0;j<4;j++)	{
			removeFila(f, &jogador);
			pop(s, &element);
			jogador.mao[i] = element;
			insereFila(f, jogador);
		}
	}
}

void vira(tp_stack* s, tp_item* item) {
	pop(s, item);
} 

//De novo deve chamar a biblioteca embaralhada
void manila(tp_stack *s, tp_item manila[])	{
	tp_item item;
    tp_item element;
    int j=0;
    
    vira(s, &item);
    ini_pilha(s);
	put_stack(s);
	
	if(item.valor != 7 && item.valor >= 1 && item.valor <= 12)	{
		for(int i=0;i<40;i++)	{
			pop(s, &element);
			if(element.valor == item.valor+1)	{
				manila[j] = element;
				j++;
			}
		}
	}
	else if(item.valor == 13)	{	
		for(int i=0;i<40;i++)	{
			pop(s, &element);
			if(element.valor == 1)	{
				manila[j] = element;
				j++;
			}
		}
	}
	else if(item.valor == 7)	{
		for(int i=0;i<40;i++)	{
			pop(s, &element);
			if(element.valor == 11)	{
				manila[j] = element;
				j++;
			}
		}
	}
}


void exibir_mao(tp_fila f){
	//mostra as mãos dos jogadores sem alterar a ordem da mesa
	tp_jogador jogador;
    printf("Cartas dos jogadores: \n");
    for (int i = 0; i < 4; i++) {
        removeFila(&f, &jogador);
        printf("Cartas do jogador: %-30s ", jogador.nome);
		for(int j = 0; j < 3; j++){
			if (jogador.mao[j].valor == 1)
				printf("As de ");
			else if (jogador.mao[j].valor == 11)
				printf("Q de ");
			else if (jogador.mao[j].valor == 12)
				printf("J de ");
			else if (jogador.mao[j].valor == 13)
				printf("K de ");
			else 
				printf("%d de ", jogador.mao[j].valor);
			
			if (jogador.mao[j].naipe == 'E')
				printf("Espadas ");
			else if (jogador.mao[j].naipe == 'C')
				printf("Copas ");
			else if (jogador.mao[j].naipe == 'O')
				printf("Ouros ");
			else if (jogador.mao[j].naipe == 'P')
				printf("Paus ");

			printf("||");	
			
			
		}
		printf("\n");
    }
}


#endif
