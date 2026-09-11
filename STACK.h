#ifndef STACK_h
#define STACK_h

#define STACK_MAX 40

// valor: 1-7, 11(J), 12(Q), 13(K) | naipe: 'C' 'O' 'E' 'P'
typedef struct{
    int valor;
    char naipe;
} tp_item;

typedef struct{
    tp_item itens[STACK_MAX];
    int topo;
} tp_stack;

void ini_pilha(tp_stack *p){
    p->topo = 0;
}

int push(tp_stack *p, tp_item item){
    if(p->topo >= STACK_MAX) return 0;
    p->itens[p->topo] = item;
    p->topo++;
    return 1;
}

// única forma de tirar carta: sempre do topo
int pop(tp_stack *p, tp_item *item){
    if(p->topo == 0) return 0;
    p->topo--;
    *item = p->itens[p->topo];
    return 1;
}

#endif
