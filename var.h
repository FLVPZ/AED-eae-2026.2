#ifndef VAR_H
#define VAR_H
#include "STACK.h"
#include "listade.h"
#include "Jogador.h"
#include <stdio.h>
#include <stdlib.h>

//Chamar todo inicio de mão
void inicializa_variavel_var(tp_item_VAR *e){
    e->mao = -1;
    e->rodada = -1;
    e->dupla_vencedora=-1;
    e->manilha = -1;
    for (int i = 0; i < 4; i++) {
        e->cartas_jogadas[i].naipe = 'A';
        e->cartas_jogadas[i].valor = -1;
    }

    for (int i = 0; i < 3; i++) {
        e->aposta[i] = -1;
      
    }
}

// imprime o valor de uma carta (1 = As, 11 = J, 12 = Q, 13 = K)
void imprimir_valor(int valor){
    switch (valor) {
        case 1:  printf("As"); break;
        case 11: printf("J");  break;
        case 12: printf("Q");  break;
        case 13: printf("K");  break;
        default: printf("%d", valor);
    }
}

void imprimir_carta(tp_item carta) {
    // carta ainda nao jogada (rodada interrompida por corrido)
    if (carta.valor < 1) {
        printf("(nao jogou)");
        return;
    }

    imprimir_valor(carta.valor);
    printf(" de ");

    switch (carta.naipe) {
        case 'E': printf("Espadas"); break;
        case 'C': printf("Copas");   break;
        case 'O': printf("Ouros");   break;
        case 'P': printf("Paus");    break;
    }
}

void imprimir_var(tp_no_de *atu){
	printf("\nVAR\n");
	printf("Numero da rodada: %d\n", atu->info.rodada);
	printf("Numero da mao: %d\n", atu->info.mao);
	printf("Cartas jogadas: \n");
		for(int j = 0; j < 4; j++){
			imprimir_carta(atu->info.cartas_jogadas[j]);
            printf("  ||  ");
		}
		printf("\n");
		printf("A manilha da rodada foi: ");
		imprimir_valor(atu->info.manilha);
        printf("\n");
    
	if(atu->info.aposta[0]==-1){
		printf("Nenhuma aposta realizada na rodada\n");
	} else {
		if(atu->info.aposta[0]==3){
			if(atu->info.aposta[2]==0){
				printf("Truco pedido pela dupla %d // dupla %d aceitou\n", atu->info.aposta[1], ((atu->info.aposta[1]%2)+1));
			} else{
				printf("Truco pedido pela dupla %d // dupla %d recusou\n", atu->info.aposta[1], ((atu->info.aposta[1]%2)+1));
			}
		} else {
			if(atu->info.aposta[2]==0){
				printf("%d pedido pela dupla %d // dupla %d aceitou\n", atu->info.aposta[0],atu->info.aposta[1], ((atu->info.aposta[1]%2)+1));
			} else{
				printf("%d pedido pela dupla %d // dupla %d recusou\n", atu->info.aposta[0],atu->info.aposta[1], ((atu->info.aposta[1]%2)+1));
			}
		}
	}

	if(atu->info.dupla_vencedora == 0)
		printf("Rodada empatada\n");
	else
		printf("Dupla ganhadora da rodada: %d\n", atu->info.dupla_vencedora);
}

// atual = posição (0 = primeira) de onde o VAR está agora; mudar = 0 mostra a atual, 1 mostra a próxima, -1 a anterior
void mostra_var(tp_listad *lista, int atual, int mudar) {
	if (lista==NULL || lista->ini==NULL)
		printf("Lista nao inicializada ou vazia");
	else {
    	tp_no_de *atu;     
        atu = lista->ini;
		for (int i=0; i<atual; i++){
			atu=atu->prox;
		}
		if(mudar==1 && atu->prox!=NULL){
			atu=atu->prox;
		} else if(mudar==-1 && atu->ant!=NULL){
			atu=atu->ant;
		}
		imprimir_var(atu);
    }
    printf("\n");
}

//Chamar para adicionar o numero da rodada na variavel que vai ao VAR
void rodada_var(tp_item_VAR *e, int rodada){
    e->rodada = rodada;
}

//Chamar para adicionar o numero da mão na variavel que vai ao VAR
void mao_var(tp_item_VAR *e, int mao){
    e->mao = mao;
}

//Chamar para adicionar as apostas da rodada na variavel que vai ao VAR
void aposta_var (tp_item_VAR *e, int dupla_pediu, int aposta ,int dupla_aceitou){
    e->aposta[0]=aposta;
    e->aposta[1]=dupla_pediu;
    e->aposta[2]=dupla_aceitou;
}

//Chamar para adicionar a dupla vencedora da rodada na variavel que vai ao VAR
void dupla_vencedora_var (tp_item_VAR *e, int dupla_vencedora){
    e->dupla_vencedora=dupla_vencedora;
}

//Chamar para adicionar as cartas e a manilha da rodada na variavel que vai ao VAR
void cartas_var(tp_item_VAR *e, tp_item cartas[4], int manilha){
    for (int i = 0; i < 4; i++){
        e->cartas_jogadas[i] = cartas[i];
    }
    e->manilha = manilha;
      
}

//Chamar para adicionar a variavel que vai ao VAR no VAR
int adiciona_var (tp_item_VAR *e, tp_listad *lista){
    if(insere_no_VAR (lista, *e)==0) return 0;
    inicializa_variavel_var(e);
    return 1;

}


//Chamar para a visualização do VAR
void ver_var(tp_listad *var){
    int cont = 1;
    int atual = 0;
    int escolha;
    int tam_VAR = tamanho_var(var);

    if (tam_VAR == 0){
        printf("\nO VAR ainda nao tem nenhuma rodada registrada.\n");
        return;
    }

    //Mover para a direira = 1, mover para a esquerda = -1;
    mostra_var(var, atual, 0);
    while (cont==1){
        printf("\nMenu do VAR\n");
        printf("Rodada %d de %d no VAR\n", atual + 1, tam_VAR);
        printf("Digite 1 se quiser sair do VAR, 2 para avançar o VAR para a direita ou 3 para avançar o VAR para a esquerda.\n");
        if (scanf("%d", &escolha) != 1){
            escolha = 0; // entrada que nao é numero
        }
        limpar_buffer();
        if(escolha==1){
            printf("Saindo do VAR.\n");
            break;
        } else if (escolha == 2){
            if(atual >= tam_VAR - 1){
                printf("Opção indisponivel, o VAR já se encontra no final e não consegue se movimentar para a direita\n");
            } else {
                mostra_var(var, atual, 1);
                atual++;
            }
        } else if(escolha == 3){
            if(atual==0){
                printf("Opção indisponivel, o VAR se encontra no inicio e não consegue se movimentar para a esquerda\n");
            } else {
                mostra_var(var, atual, -1);
                atual--;
            }
        } else {
            printf("Opção invalida, digite um numero valido\n");
        }
    }

}

#endif
