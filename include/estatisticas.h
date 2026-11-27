#ifndef ESTATISTICAS_H
#define ESTATISTICAS_H
#include "caderneta.h"
#include "fila.h"

// Incrementa a quantidade de trucos pedidos pelo jogador
void registrar_truco_pedido(ArvAVL *raiz, char nome[]){
    if(raiz == NULL) return;
    struct NO *jogador;
    jogador = acessar_jogador(raiz, nome);
    if(jogador != NULL){
        jogador->info.trucos_pedidos++;
    }
}

// Incrementa os trucos recebidos p calcular a taxa de fuga
void registrar_truco_recebido(ArvAVL *raiz, char nome[]){
    if(raiz == NULL) return;
    struct NO *jogador;
    jogador = acessar_jogador(raiz, nome);
    if(jogador != NULL){
        jogador->info.trucos_recebidos++;
    }
}

// Incrementa a quantidade de vezes que o jogador correu
void registrar_truco_corrido(ArvAVL *raiz, char nome[]){
    if(raiz == NULL) return;
    struct NO *jogador;
    jogador = acessar_jogador(raiz, nome);

    if(jogador != NULL){
        jogador->info.trucos_corridos++;
    }
}

// Incrementa a quantidade de mãos ganhas
void registrar_mao_ganha(ArvAVL *raiz, char nome[]){
    if(raiz == NULL) return;
    struct NO *jogador;
    jogador = acessar_jogador(raiz, nome);
    if(jogador != NULL){
        jogador->info.maos_ganhas++;
    }
}

// Incrementa as partidas jogadas
void registrar_partida(ArvAVL *raiz, char nome[]){
    if(raiz == NULL) return;
    struct NO *jogador;
    jogador = acessar_jogador(raiz, nome);

    if(jogador != NULL){
        jogador->info.partidas_jogadas++;
    }
}

// Registra uma mão ganha para os dois jogadores da dupla vencedora
void registrar_dupla_vencedora(ArvAVL *raiz,tp_fila mesa,int dupla_vencedora){
    tp_jogador j;

    // A fila é passada por valor para não alterar a mesa original
    while(removeFila(&mesa, &j)){
        if(j.dupla == dupla_vencedora){
            registrar_mao_ganha(raiz, j.nome);
        }
    }
}

// Registra uma partida jogada para os quatro na mesa
void registrar_partida_jogadores(ArvAVL *raiz,tp_fila mesa){
    tp_jogador j;

    // removeFila retorna 0 quando a cópia da fila fica vazia
    while(removeFila(&mesa, &j)){
        registrar_partida(raiz, j.nome);
    }
}

// Armazena o maior valor de mão registrado durante a partida
void registrar_maior_mao(int *maior_mao, int pontos){
    if(maior_mao == NULL) return;

    // Utiliza um ponteiro para modificar a variável original
    if(pontos > *maior_mao){
        *maior_mao = pontos;
    }
}

#endif