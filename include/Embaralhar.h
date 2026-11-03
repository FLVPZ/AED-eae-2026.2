#include "STACK.h"
#ifndef Embaralhar_h
#define Embaralhar_h
#include <stdlib.h>
#include <time.h>

void inicializar_seed(){
    // faz o rand gerar uma sequência diferente a cada execução
    srand((unsigned int)time(NULL));
}

void embaralhar_baralho(tp_stack *p){
    // limpa a pilha antes de montar um novo baralho
    ini_pilha(p);

    tp_item baralho[40];
    char naipes[4] = {'C', 'O', 'E', 'P'};
    int valores[10] = {1, 2, 3, 4, 5, 6, 7, 11, 12, 13};
    int pos = 0;

    // monta as 40 cartas combinando cada naipe com todos os valores
    for(int n=0; n<4; n++){
        for(int v=0; v<10; v++){
            baralho[pos].naipe = naipes[n];
            baralho[pos].valor = valores[v];
            pos++;
        }
    }

    // troca as cartas de posição para embaralhar o vetor
    for(int i=39; i>0; i--){
        int troca = rand() % (i+1);
        tp_item reserva = baralho[i];
        baralho[i] = baralho[troca];
        baralho[troca] = reserva;
    }

    // depois de embaralhado, coloca o baralho inteiro na pilha
    for(int i=0; i<40; i++){
        push(p, baralho[i]);
    }
}

#endif
