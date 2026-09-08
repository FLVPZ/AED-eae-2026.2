#include <stdio.h>
#include "STACK.h"
#ifndef Embaralhar_h
#define Embaralhar_h
#include <stdlib.h>
#include <time.h>


void inicializar_seed(){
    //Usar essa função sempre antes de começão o programa e embaralhar o baralho.
    srand((unsigned int)time(NULL));
}

void embaralhar_baralho(tp_stack *p){
    // Ela já tem o baralho "organizado", basta colocar o endereço de memoria do stack do baralho que será usado em jogo criado na main.
    initiliaze(p);
    tp_item baralho[40]={{1,'C'}, {2,'C'}, {3,'C'}, {4,'C'}, {5,'C'}, {6,'C'}, {7,'C'}, {11,'C'}, {12,'C'}, {13,'C'},
				   	     {1,'O'}, {2,'O'}, {3,'O'}, {4,'O'}, {5,'O'}, {6,'O'}, {7,'O'}, {11,'O'}, {12,'O'}, {13,'O'},
					     {1,'E'}, {2,'E'}, {3,'E'}, {4,'E'}, {5,'E'}, {6,'E'}, {7,'E'}, {11,'E'}, {12,'E'}, {13,'E'},
				         {1,'P'}, {2,'P'}, {3,'P'}, {4,'P'}, {5,'P'}, {6,'P'}, {7,'P'}, {11,'P'}, {12,'P'}, {13,'P'}};
    for(int i=39; i>0; i--){
        int troca = rand() % i;
        tp_item reserva = baralho[i];
        baralho[i] = baralho[troca];
        baralho[troca] = reserva;
    }

    for(int i=0; i<40; i++){
        push(p, baralho[i]);
    }
}

#endif
