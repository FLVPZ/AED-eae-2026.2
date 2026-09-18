#include <stdio.h>
#ifndef Distribuicao_h
#define Distribuicao_h
#include "STACK.h"
#include "baralho.h"
//Chama a biblioteca ja embaralhada
void distribute_carts(tp_stack *s, tp_mesa *cadeira)	{
	tp_item element;
	
	for(int i=0;i<3;i++)	{
		for(int j=0;j<4;j++)	{
			int idx=cadeira->ordem[j];
			pop(s, &element);
			cadeira->jogadores[idx].mao[i] = element;
		}
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
