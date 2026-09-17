#include <stdio.h>
#ifndef baralho_h
#define baralho_h
#include "STACK.h"
//Colocar todas as cartas no baralho
void put_stack(tp_stack *s)	{
	 tp_item baralho[40]={{1,'C'}, {2,'C'}, {3,'C'}, {4,'C'}, {5,'C'}, {6,'C'}, {7,'C'}, {11,'C'}, {12,'C'}, {13,'C'},
				   	                {1,'O'}, {2,'O'}, {3,'O'}, {4,'O'}, {5,'O'}, {6,'O'}, {7,'O'}, {11,'O'}, {12,'O'}, {13,'O'},
					                {1,'E'}, {2,'E'}, {3,'E'}, {4,'E'}, {5,'E'}, {6,'E'}, {7,'E'}, {11,'E'}, {12,'E'}, {13,'E'},
				                    {1,'P'}, {2,'P'}, {3,'P'}, {4,'P'}, {5,'P'}, {6,'P'}, {7,'P'}, {11,'P'}, {12,'P'}, {13,'P'} };
	int i=0;
				                    
	for(int i=0;i<40;i++)	{
		push(s, baralho[i]);
	}
}
//Limpar todo o baralho
void limp(tp_stack *s)	{
	tp_item e;
	while(!is_empty(s))		{
		pop(s, &e);
	}			  	
}


#endif