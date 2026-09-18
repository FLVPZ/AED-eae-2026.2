#include <stdio.h>
#ifndef Distribuicao_h
#define Distribuicao_h
#include "STACK.h"
#include "baralho.h"
//Chama a biblioteca ja embaralhada
void distribute_carts(tp_stack *s, tp_item player_1[], tp_item player_2[], tp_item player_3[], tp_item player_4[])	{
	tp_item element;
	
	for(int j=0;j<3;j++)	{
		pop(s, &element);
		player_1[j] = element;
		//
		pop(s, &element);
		player_2[j] = element;
		//
		pop(s, &element);
		player_3[j] = element;
		//
		pop(s, &element);
		player_4[j] = element;
	}
}

void vira(tp_stack *s, tp_item *item) {
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
#endif
