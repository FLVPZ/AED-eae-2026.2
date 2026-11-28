#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef CADERNETA_H
#define CADERNETA_H
#include <ctype.h>

typedef struct{
    char nome_jogador[50];
    int partidas_jogadas;
    int maos_ganhas;
    int trucos_pedidos;
    int trucos_corridos;
    int trucos_recebidos; // pedidos de truco/seis/doze respondidos pelo jogador
}info_caderneta;

typedef struct NO* ArvAVL;

struct NO{
   info_caderneta info;
   int alt; //altura da sub-árvore (facilita o cálculo do fator de balanceamento!)
   struct NO *esq;
   struct NO *dir;
};

ArvAVL* criarAVL();
int consultarValorAVL(ArvAVL* raiz, char valor[]);
void emOrd(ArvAVL* raiz);
int inserir(ArvAVL* raiz, info_caderneta valor) ;
struct NO* acessar_jogador(ArvAVL* raiz, char nome[]);
int maior(int x, int y);
int alt_NO(struct NO* no);
int fb_NO(struct NO* no);
void primeira_letra_maiuscula(char valor[]);
void RotacaoLL(ArvAVL* raiz);
void RotacaoRR(ArvAVL* raiz);
void RotacaoRL(ArvAVL* raiz);
void RotacaoLR(ArvAVL* raiz);

ArvAVL* criarAVL(){
	ArvAVL* raiz = (ArvAVL*)malloc(sizeof(ArvAVL));
	if(raiz!=NULL){
		*raiz = NULL;
	}
	return raiz;
}

void primeira_letra_maiuscula(char valor[]){
	for(int i=0; valor[i] != '\0'; i++){
		valor[i] = tolower((unsigned char)valor[i]);
	}
	valor[0] = toupper((unsigned char)valor[0]);
}




// Uma linha por jogador; a main apresenta o cabecalho da tabela.
void emOrd(ArvAVL* raiz){
   if(raiz == NULL || *raiz == NULL) return;
   emOrd(&((*raiz)->esq));
   info_caderneta info = (*raiz)->info;
   printf("  %-29.29s %4d %4d %5d %5d %5d\n", info.nome_jogador,
          info.partidas_jogadas, info.maos_ganhas, info.trucos_pedidos,
          info.trucos_recebidos, info.trucos_corridos);
   emOrd(&((*raiz)->dir));
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
	if(raiz == NULL) return 0;
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
	if(valor == NULL) return 0;
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
	primeira_letra_maiuscula(nome);
	if(raiz == NULL || nome == NULL || nome[0] == '\0'
	   || strlen(nome) >= sizeof(((info_caderneta*)0)->nome_jogador)) return NULL;
	if(consultarValorAVL(raiz, nome)==0){
		info_caderneta novo;
		strcpy(novo.nome_jogador, nome);
		novo.partidas_jogadas=0;
		novo.maos_ganhas=0;
		novo.trucos_pedidos=0;
		novo.trucos_corridos=0;
		novo.trucos_recebidos=0;
		if(!inserir(raiz, novo)) return NULL;
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
	return abs(esq - dir);
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

#endif
