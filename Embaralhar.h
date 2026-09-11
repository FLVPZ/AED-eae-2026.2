#include "STACK.h"
#ifndef Embaralhar_h
#define Embaralhar_h
#include <stdlib.h>
#include <time.h>

void inicializar_seed(){
    //Usar essa função sempre antes de começar o programa e embaralhar o baralho.
    srand((unsigned int)time(NULL));
}

void embaralhar_baralho(tp_stack *p){
    // Ela já tem o baralho "organizado", basta colocar o endereço de memoria do stack do baralho que será usado em jogo criado na main.
    ini_pilha(p);

    tp_item baralho[40];
    char naipes[4] = {'C', 'O', 'E', 'P'};
    int valores[10] = {1, 2, 3, 4, 5, 6, 7, 11, 12, 13};
    int pos = 0;

    for(int n=0; n<4; n++){
        for(int v=0; v<10; v++){
            baralho[pos].naipe = naipes[n];
            baralho[pos].valor = valores[v];
            pos++;
        }
    }

    for(int i=39; i>0; i--){
        int troca = rand() % (i+1);
        tp_item reserva = baralho[i];
        baralho[i] = baralho[troca];
        baralho[troca] = reserva;
    }

    for(int i=0; i<40; i++){
        push(p, baralho[i]);
    }
}

#endif
