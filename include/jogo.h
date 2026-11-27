#include <stdio.h>
#ifndef jogo_h
#define jogo_h
#include "STACK.h"
#include "Jogador.h"
#include "fila.h"

typedef struct {
	tp_item carta;
	int jogador	;	
}tp_vencedor;

int forca_valor(int valor) {
    switch(valor) {
        case 3:  return 10;
        case 2:  return 9;
        case 1:  return 8;  
        case 13: return 7;  
        case 11: return 6;  
        case 12: return 5;  
        case 7:  return 4;
        case 6:  return 3;
        case 5:  return 2;
        case 4:  return 1;
        default: return 0;
    }
}

int forca_naipe(char naipe) {
    switch(naipe) {
        case 'P': return 4;
        case 'C': return 3;  
        case 'E': return 2;  
        case 'O': return 1;  
        default: return 0;
    }
}

// distribui uma carta por vez até cada jogador ficar com três
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

// retira a próxima carta do baralho para usar como vira
tp_item vira(tp_stack* s) {
	tp_item item;
	pop(s, &item);
	return item;
} 

// descobre o valor da manilha a partir da vira
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


// mostra as cartas dos jogadores sem alterar a ordem da mesa
void exibir_mao(tp_fila f){
	tp_jogador jogador;
    printf("Cartas dos jogadores: \n");
    for (int i = 0; i < 4; i++) {
        removeFila(&f, &jogador);
        printf("Cartas do jogador: %-30s ", jogador.nome);
		for(int j = 0; j < 3; j++){
			// transforma os valores das figuras em letras na hora de mostrar
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

void cartas_rodada(tp_fila* mesa, tp_vencedor carta[], int rodada)	{	//rodada deve ser considerada a partir do numero 1.
	if(mesa == NULL || rodada < 1 || rodada > 3)
        return;
    
	for(int i=0;i<4;i++)	{
		carta[i].carta = mesa->item[i].mao[rodada-1];
		carta[i].jogador = i+1;
	}
	
}

int eh_manilha(tp_item carta, tp_item manilha[]) {
    for(int i = 0; i<4 ;i++) {
        if(carta.valor == manilha[i].valor && carta.naipe == manilha[i].naipe) {
            return 1;
        }
    }

    return 0;
}



int comparar_cartas(tp_item a, tp_item b, tp_item manilha[]) {
    int manilha_a = eh_manilha(a, manilha);
    int manilha_b = eh_manilha(b, manilha);

    if(manilha_a && !manilha_b)
        return 1;

    if(!manilha_a && manilha_b)
        return -1;

    if(manilha_a && manilha_b)
        return forca_naipe(a.naipe) - forca_naipe(b.naipe);

    return forca_valor(a.valor) - forca_valor(b.valor);
}

void ordenar_cartas(tp_vencedor carta[], tp_item manilha[]) {
    tp_vencedor aux;

    for(int i=0;i<3;i++) {
        for(int j=0;j<3-i;j++) {
            if(comparar_cartas(carta[j].carta, carta[j+1].carta, manilha) < 0) {
                aux = carta[j];
                carta[j] = carta[j+1];
                carta[j+1] = aux;
            }
        }
    }
}

int vencedor_rodada(tp_vencedor carta[], tp_item manilha[]) {
    ordenar_cartas(carta, manilha);

    if(comparar_cartas(carta[0].carta, carta[1].carta, manilha) == 0)
        return 0;

    return carta[0].jogador;
}

// força da carta: manilha ganha das comuns e desempata pelo naipe
int forca_carta(tp_item c, int valor_manilha) {
    if (c.valor == valor_manilha) {
        return 100 + forca_naipe(c.naipe);
    }
    return forca_valor(c.valor);
}

// retorna a posição de quem ganhou a rodada (-1 se empatou) e grava a dupla em *dupla_rodada
int vencedor_da_rodada(tp_item cartas[4], int duplas[4], int valor_manilha, int *dupla_rodada) {
    int melhor = 0;

    for (int i = 1; i < 4; i++) {
        if (forca_carta(cartas[i], valor_manilha) > forca_carta(cartas[melhor], valor_manilha)) {
            melhor = i;
        }
    }

    int forca_max = forca_carta(cartas[melhor], valor_manilha);
    for (int i = 0; i < 4; i++) {
        if (i != melhor && forca_carta(cartas[i], valor_manilha) == forca_max
            && duplas[i] != duplas[melhor]) {
            *dupla_rodada = 0;
            return -1;
        }
    }

    *dupla_rodada = duplas[melhor];
    return melhor;
}

// retorna a dupla que ganhou a mão ou 0 se ainda não foi decidida
int vencedor_da_mao(int resultados[3], int n_rodadas, int dupla_primeiro) {
    if (n_rodadas < 2) {
        return 0;
    }

    int vitorias[3] = { 0, 0, 0 };
    for (int i = 0; i < n_rodadas; i++) {
        vitorias[resultados[i]]++;
    }

    if (n_rodadas == 2) {
        if (resultados[0] == 0 && resultados[1] != 0) return resultados[1]; // empatou a 1ª: a 2ª decide
        if (resultados[0] != 0 && resultados[1] == 0) return resultados[0]; // empatou a 2ª: vale a 1ª
        if (resultados[0] != 0 && resultados[0] == resultados[1]) return resultados[0]; // 2 a 0
        return 0;
    }

    // 3 rodadas jogadas
    if (vitorias[1] > vitorias[2]) return 1;
    if (vitorias[2] > vitorias[1]) return 2;
    if (resultados[0] != 0) return resultados[0]; // 1 a 1: vale quem levou a 1ª
    return dupla_primeiro;                         // tudo empatado: dupla de quem abriu a mão
}

#endif
