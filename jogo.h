
#include <stdio.h>
#ifndef Distribuicao_h
#define Distribuicao_h
#include "STACK.h"
#include "baralho.h"
#include "Jogador.h"
#include "fila.h"

//Chama a biblioteca ja embaralhada
void distribuir_cartas(tp_stack *s, tp_fila *f)	{
	tp_item elemento;
	tp_jogador jogador;
	for(int i=0;i<3;i++)	{
		for(int j=0;j<4;j++)	{
			removeFila(f, &jogador);
			pop(s, &elemento);
			jogador.mao[i] = elemento;
			insereFila(f, jogador);
		}
	}
}

tp_item vira(tp_stack* s) {
	tp_item item;
	pop(s, &item);
	return item;
} 

//De novo deve chamar a biblioteca embaralhada
void manilha(tp_item item, tp_item manilha[])	{
    int valor_manilha;

    switch (item.valor) {
        case 4:  valor_manilha = 5; break;
        case 5:  valor_manilha = 6; break;
        case 6:  valor_manilha = 7; break;
        case 7:  valor_manilha = 12; break;
        case 12: valor_manilha = 11; break;
        case 11: valor_manilha = 13; break;
        case 13: valor_manilha = 1; break;
        case 1:  valor_manilha = 2; break;
        case 2:  valor_manilha = 3; break;
        case 3:  valor_manilha = 4; break;
        default: valor_manilha = 0; break;
    }

    manilha[0] = (tp_item){valor_manilha, 'O'};
    manilha[1] = (tp_item){valor_manilha, 'E'};
    manilha[2] = (tp_item){valor_manilha, 'C'};
    manilha[3] = (tp_item){valor_manilha, 'P'};
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
				printf("J de ");
			else if (jogador.mao[j].valor == 12)
				printf("Q de ");
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
