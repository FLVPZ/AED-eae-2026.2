#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct{
    char nome_jogador[50];
    int partidas_jogadas;
    int maos_ganhas;
    int trucos_pedidos;
    int trucos_corridos;
}info_caderneta;


typedef struct NO* ArvAVL;

struct NO{
   info_caderneta info;
   int alt; //altura da sub-árvore (facilita o cálculo do fator de balanceamento!)
   struct NO *esq;
   struct NO *dir;
};

ArvAVL* criarAVL();
void preOrd(ArvAVL* raiz);
int consultarValorAVL(ArvAVL* raiz, char valor[]);
void emOrd(ArvAVL* raiz);
void posOrd(ArvAVL* raiz);
int inserir(ArvAVL* raiz, info_caderneta valor) ;
int remover(ArvAVL* raiz, info_caderneta valor); // Responsável pela busca do nó a ser removido
struct NO* buscarMenor(struct NO* atual); // Busca o nó mais a esquerda de uma árvore
struct NO* acessar_jogador(ArvAVL* raiz, char nome[]);
int maior(int x, int y);
int alt_NO(struct NO* no);
int fb_NO(struct NO* no);
void RotacaoLL(ArvAVL* raiz);
void RotacaoRR(ArvAVL* raiz);
void RotacaoRL(ArvAVL* raiz);
void RotacaoLR(ArvAVL* raiz);

int contPrint=0;

ArvAVL* criarAVL(){
	ArvAVL* raiz = (ArvAVL*)malloc(sizeof(ArvAVL));
	if(raiz!=NULL){
		*raiz = NULL;
	}
	return raiz;
}

//Arrumar daqui a poico
void preOrd(ArvAVL* raiz){
   if(raiz==NULL) return;
   if(*raiz != NULL){
	  printf("===============\n");	 
      printf("Nome do jogador: %s\n", (*raiz)->info.nome_jogador);
	  printf("Numero de partidas jogadas: %d\n", (*raiz)->info.partidas_jogadas);
      printf("Numero de mãos ganhas: %d\n", (*raiz)->info.maos_ganhas);
      printf("Numero de trucos pedidos: %d\n", (*raiz)->info.trucos_pedidos);
	  printf("Numero de trucos corridos: %d\n", (*raiz)->info.trucos_corridos);
	  printf("===============\n");	 

      preOrd(&((*raiz)->esq));
      preOrd(&((*raiz)->dir));
   }
}

void emOrd(ArvAVL* raiz){
   if(raiz==NULL) return;
   if(*raiz != NULL){
   	  //printf("Contador:%d", contPrint);
      emOrd(&((*raiz)->esq));

	  printf("===============\n");	 
      printf("Nome do jogador: %s\n", (*raiz)->info.nome_jogador);
	  printf("Numero de partidas jogadas: %d\n", (*raiz)->info.partidas_jogadas);
      printf("Numero de mãos ganhas: %d\n", (*raiz)->info.maos_ganhas);
      printf("Numero de trucos pedidos: %d\n", (*raiz)->info.trucos_pedidos);
	  printf("Numero de trucos corridos: %d\n", (*raiz)->info.trucos_corridos);
	  printf("===============\n");	 

      emOrd(&((*raiz)->dir));
   }
}

void posOrd(ArvAVL* raiz){
   if(raiz==NULL) return;
   if(*raiz != NULL){
      posOrd(&((*raiz)->esq));
      posOrd(&((*raiz)->dir));
	  printf("===============\n");	 
      printf("Nome do jogador: %s\n", (*raiz)->info.nome_jogador);
	  printf("Numero de partidas jogadas: %d\n", (*raiz)->info.partidas_jogadas);
      printf("Numero de mãos ganhas: %d\n", (*raiz)->info.maos_ganhas);
      printf("Numero de trucos pedidos: %d\n", (*raiz)->info.trucos_pedidos);
	  printf("Numero de trucos corridos: %d\n", (*raiz)->info.trucos_corridos);
	  printf("===============\n");	 

   }
}
// até aqui

int alturaAVL(ArvAVL* raiz){
	if(raiz == NULL) return 0;
	if(*raiz == NULL) return 0;
	int alt_esq = alturaAVL(&((*raiz)->esq));
	int alt_dir = alturaAVL(&((*raiz)->dir));
	if(alt_esq > alt_dir){
		return alt_esq + 1;
	}else{
		return alt_dir + 1;
	}
}

int totalNOsABB(ArvAVL* raiz){
	if(raiz == NULL) return 0;
	if(*raiz == NULL) return 0;
	int tot_esq = totalNOsABB(&((*raiz)->esq));
	int tot_dir = totalNOsABB(&((*raiz)->dir));
	return (tot_esq + tot_dir + 1);
}

int estah_vaziaABB(ArvAVL* raiz){
	if(raiz==NULL) return 1;
	if(*raiz == NULL) return 1;
	return 0;
}	

int inserir(ArvAVL* raiz, info_caderneta info){	
	int res;
	if(*raiz==NULL){
		struct NO* novo;
		novo = (struct NO*)malloc(sizeof(struct NO));
		if(novo==NULL) return 0;
		novo->info = info;
		novo->alt = 0;
		novo->dir = NULL;
		novo->esq = NULL;
		*raiz = novo;
		return 1;	
	} else {
		struct NO* atual = *raiz;
		if(strcmp(info.nome_jogador, atual->info.nome_jogador)<0){
			if((res=inserir(&(atual->esq), info))==1){
				if(fb_NO(atual) >= 2){
					if(strcmp(info.nome_jogador, (*raiz)->esq->info.nome_jogador)<0){
						RotacaoLL(raiz);
					}else{
						RotacaoLR(raiz);
					}
				}
			}
		}else{
			if(strcmp(info.nome_jogador, atual->info.nome_jogador)>0){
				if((res=inserir(&(atual->dir), info))==1){
					if(fb_NO(atual) >= 2){
						if(strcmp(info.nome_jogador, (*raiz)->dir->info.nome_jogador)>0){
							RotacaoRR(raiz);
						}else{
							RotacaoRL(raiz);
						}
					}
				}
			}else{
				return 0; // Valor Duplicado
			}
		}
		atual->alt = maior(alt_NO(atual->esq), alt_NO(atual->dir)) + 1;
		return res;
	} 
}


void liberaNO(struct NO* no){
   if(no==NULL) return;
   liberaNO(no->esq);
   liberaNO(no->dir);
   free(no);
   no = NULL;
}

void liberaABB(ArvAVL* raiz){
   if(raiz==NULL) return;
   liberaNO(*raiz);
   free(raiz);
}

int consultarValorAVL(ArvAVL* raiz, char valor[]){
	if(raiz == NULL) return 0;
	if(*raiz == NULL) return 0;
	struct NO* atual = *raiz;
	while(atual != NULL){
		if(strcmp(atual->info.nome_jogador, valor)==0){
			return 1;
		}else{
			if(strcmp(atual->info.nome_jogador, valor)>0){
				atual = atual->esq;
			}else{
				atual = atual->dir;
			}
		}
	}
	return 0;
}



struct NO* acessar_jogador(ArvAVL* raiz, char nome[]){
	if(consultarValorAVL(raiz, nome)==0){
		info_caderneta novo;
		strcpy(novo.nome_jogador, nome);
		novo.partidas_jogadas=0;
		novo.maos_ganhas=0;
		novo.trucos_pedidos=0;
		novo.trucos_corridos=0;
		inserir(raiz, novo);
	}
	if(raiz == NULL) return NULL;
	if(*raiz == NULL) return NULL;
	struct NO* atual = *raiz;
	while(atual != NULL){
		if(strcmp(atual->info.nome_jogador, nome) == 0){
			return atual;
		}else{
			if(strcmp(atual->info.nome_jogador, nome) > 0){
				atual = atual->esq;
			}else{
				atual = atual->dir;
			}
		}
	}
	return NULL;
}

int alt_NO(struct NO* no){
   if(no == NULL) return -1;
   else return no->alt;
}

int fb_NO(struct NO* no){
	int esq = alt_NO(no->esq);
	int dir = alt_NO(no->dir);
	return labs(esq - dir);
}

int maior(int x, int y){
	return (x>y)?x:y;
}

void RotacaoLL(ArvAVL* raiz){
   struct NO* no;
   no = (*raiz)->esq;
   (*raiz)->esq = no->dir;
   no->dir = *raiz;
   (*raiz)->alt = maior(alt_NO((*raiz)->esq), alt_NO((*raiz)->dir)) + 1;
   no->alt = maior(alt_NO(no->esq), (*raiz)->alt) + 1;
   *raiz = no;
}

void RotacaoRR(ArvAVL* raiz){
   struct NO* no;
   no = (*raiz)->dir;
   (*raiz)->dir = no->esq;
   no->esq = *raiz;
   (*raiz)->alt = maior(alt_NO((*raiz)->esq), alt_NO((*raiz)->dir)) + 1;
   no->alt = maior(alt_NO(no->dir), (*raiz)->alt) + 1;
   *raiz = no;
}

void RotacaoLR(ArvAVL* raiz){
	RotacaoRR(&(*raiz)->esq);
	RotacaoLL(raiz);
}

void RotacaoRL(ArvAVL* raiz){
	RotacaoLL(&(*raiz)->dir);
	RotacaoRR(raiz);
}

int remover(ArvAVL* raiz, info_caderneta valor){	
	int res;
	if(*raiz==NULL){ // Valor não encontrado!
		printf("Nome %s nao encontrado na árvore!", valor.nome_jogador);
		return 0;	
	} 
	if(strcmp(valor.nome_jogador, (*raiz)->info.nome_jogador)<0){
		if((res=remover(&(*raiz)->esq, valor))==1){
			if(fb_NO(*raiz) >= 2){
				if(alt_NO((*raiz)->dir->esq) <= alt_NO((*raiz)->dir->dir)){
					RotacaoRR(raiz);
				}else{
					RotacaoRL(raiz);
				}
			}
		}				
	}else if(strcmp(valor.nome_jogador, (*raiz)->info.nome_jogador)>0){
		if((res=remover(&(*raiz)->dir, valor))==1){
			if(fb_NO(*raiz) >= 2){
				if(alt_NO((*raiz)->esq->dir) <= alt_NO((*raiz)->esq->esq)){
					RotacaoLL(raiz);
				}else{
					RotacaoLR(raiz);
				}
			}
		}							
	}else{ // raiz->info == valor...
		if(((*raiz)->esq == NULL) || ((*raiz)->dir == NULL)){ // O Pai tem 1 ou nenhum filho
			struct NO* noVelho = (*raiz);
			if((*raiz)->esq != NULL) *raiz = (*raiz)->esq;
			else *raiz = (*raiz)->dir;
			free(noVelho);
			return 1;
		}else{ // o nó tem 2 filhos: precisamos substituir pelo nó mais a esquerda da subárvore da direita
			struct NO* temp = buscarMenor((*raiz)->dir);
			(*raiz)->info = temp->info;
			remover(&(*raiz)->dir, (*raiz)->info);
			if(fb_NO(*raiz) >= 2){
				if(alt_NO((*raiz)->esq->dir) <= alt_NO((*raiz)->esq->esq)){
					RotacaoLL(raiz);
				}else{
					RotacaoLR(raiz);
				}
			}
			return 1;			
		}
		
	} 
	return res;
}

struct NO* buscarMenor(struct NO* atual){
	struct NO* no1 = atual;
	struct NO* no2 = atual->esq;
	while(no2 != NULL){
		no1 = no2;
		no2 = no1->esq;
	}
	return no1;
}

