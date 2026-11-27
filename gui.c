/* Interface nativa Windows. Compile gui.c, sem juntar main.c no comando. */
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "include/motor_grafico.h"

enum { ID_INICIAR=100, ID_CONTINUAR, ID_REVELAR, ID_PEDIR, ID_ACEITAR,
       ID_CORRER, ID_AUMENTAR, ID_SINAL, ID_GESTO, ID_VAR, ID_STATS, ID_AJUDA,
       ID_CARTA=200, ID_NOME=300, ID_ANTERIOR=400, ID_PROXIMA, ID_ORDEM, ID_MAOS, ID_TAXA };
static tp_motor_grafico jogo;
static HWND principal, nomes_edit[4], cartas_btn[3], iniciar_btn, continuar_btn,
    revelar_btn, pedir_btn, aceitar_btn, correr_btn, aumentar_btn, sinal_btn,
    gesto_combo, var_btn, stats_btn, ajuda_btn, janela_view, texto_view;
static HFONT fonte, fonte_titulo, fonte_carta;
static HBRUSH fundo, papel;
static int view_tipo, avisados;
static tp_no_de *view_rodada;
static tp_no_de view_parcial;
static int view_tem_parcial;

static void texto(HDC dc, int x, int y, int w, int h, const char *s, HFONT f, COLORREF cor) {
    RECT r={x,y,x+w,y+h};
    SelectObject(dc,f); SetTextColor(dc,cor); SetBkMode(dc,TRANSPARENT);
    DrawTextA(dc,s,-1,&r,DT_LEFT|DT_WORDBREAK|DT_NOPREFIX);
}
static void painel(HDC dc, int x, int y, int w, int h, COLORREF cor) {
    HBRUSH b=CreateSolidBrush(cor);
    HGDIOBJ velho=SelectObject(dc,b), caneta=SelectObject(dc,GetStockObject(NULL_PEN));
    RoundRect(dc,x,y,x+w,y+h,18,18);
    SelectObject(dc,caneta); SelectObject(dc,velho); DeleteObject(b);
}
static HWND controle(const char *classe,const char *rotulo,DWORD estilo,int id) {
    HWND h=CreateWindowExA(0,classe,rotulo,WS_CHILD|WS_TABSTOP|estilo,
                          0,0,1,1,principal,(HMENU)(INT_PTR)id,GetModuleHandle(NULL),NULL);
    SendMessage(h,WM_SETFONT,(WPARAM)fonte,TRUE); return h;
}
static void mostrar(HWND h,int sim) { ShowWindow(h,sim?SW_SHOW:SW_HIDE); }
static void posicionar(HWND h,int x,int y,int w,int altura) { MoveWindow(h,x,y,w,altura,TRUE); }

static void atualizar(void) {
    RECT r; GetClientRect(principal,&r); int w=r.right,h=r.bottom,esq=w-340,dir=w-308;
    int cadastro=jogo.etapa==GUI_CADASTRO, vez=jogo.etapa==GUI_VEZ, resposta=jogo.etapa==GUI_RESPOSTA;
    for(int i=0;i<4;i++) {
        mostrar(nomes_edit[i],cadastro);
        posicionar(nomes_edit[i],240,265+i*54,esq-260,36);
    }
    mostrar(iniciar_btn,cadastro); posicionar(iniciar_btn,32,h-52,esq-48,40);
    mostrar(revelar_btn,vez&&!jogo.visivel); posicionar(revelar_btn,32,h-52,esq-48,40);
    if(vez) {
        tp_jogador j; motor_atual(&jogo,&j);
        char s[100]; snprintf(s,sizeof(s),"Revelar cartas de %s",j.nome); SetWindowTextA(revelar_btn,s);
        if(proximo_valor(jogo.aposta.valor)) snprintf(s,sizeof(s),"Pedir %s",nome_pedido(proximo_valor(jogo.aposta.valor)));
        else snprintf(s,sizeof(s),"Limite de 12 pontos");
        SetWindowTextA(pedir_btn,s);
        EnableWindow(pedir_btn,pode_pedir(jogo.aposta,j.dupla));
    }
    for(int i=0;i<3;i++) {
        int largura=(esq-64)/3;
        mostrar(cartas_btn[i],vez&&jogo.visivel&&i<3-jogo.rodada);
        posicionar(cartas_btn[i],32+i*(largura+8),h-190,largura,118);
    }
    mostrar(pedir_btn,vez); posicionar(pedir_btn,dir,325,276,44);
    mostrar(gesto_combo,vez); posicionar(gesto_combo,dir,408,276,180);
    mostrar(sinal_btn,vez); posicionar(sinal_btn,dir,448,276,40);
    EnableWindow(sinal_btn,vez);
    mostrar(aceitar_btn,resposta); mostrar(correr_btn,resposta); mostrar(aumentar_btn,resposta);
    posicionar(aceitar_btn,dir,325,276,44); posicionar(correr_btn,dir,380,276,44);
    posicionar(aumentar_btn,dir,435,276,44);
    int proximo=proximo_valor(jogo.aposta.pedido);
    char s[80]; if(proximo) snprintf(s,sizeof(s),"Aumentar para %s",nome_pedido(proximo));
    else snprintf(s,sizeof(s),"Limite de 12 pontos");
    SetWindowTextA(aumentar_btn,s);
    EnableWindow(aumentar_btn,proximo!=0);
    mostrar(continuar_btn,jogo.etapa==GUI_RODADA||jogo.etapa==GUI_MAO||jogo.etapa==GUI_FIM);
    posicionar(continuar_btn,32,h-52,esq-48,40);
    SetWindowTextA(continuar_btn,jogo.etapa==GUI_FIM?"Jogar outra partida":jogo.etapa==GUI_MAO?"Comecar proxima mao":"Continuar");
    mostrar(var_btn,1); mostrar(stats_btn,1); mostrar(ajuda_btn,1);
    posicionar(var_btn,dir,510,276,40); posicionar(stats_btn,dir,560,276,40); posicionar(ajuda_btn,dir,610,276,36);
    EnableWindow(var_btn,(jogo.var&&jogo.var->ini)||vez||resposta);
    InvalidateRect(principal,NULL,TRUE);
    if(jogo.erros_disco!=avisados) {
        avisados=jogo.erros_disco;
        char erro[1600];
        snprintf(erro,sizeof(erro),"%s\n\nPasta dos dados: %.400s\n\nOs dados da caderneta continuam na memoria. Feche o arquivo se estiver bloqueado e confira a permissao de escrita.",jogo.erro_disco,disco_diretorio);
        MessageBoxA(principal,erro,"Falha de gravacao",MB_OK|MB_ICONWARNING);
    }
}

static void pintar(HDC dc) {
    RECT r; GetClientRect(principal,&r); int w=r.right,h=r.bottom,esq=w-340,dir=w-308;
    FillRect(dc,&r,fundo);
    painel(dc,0,0,w,83,RGB(63,44,31));
    texto(dc,30,16,w-60,42,"TRUCO NO BAR DO PATO",fonte_titulo,RGB(250,224,163));
    texto(dc,32,55,w-60,24,"Quatro jogadores. Duas duplas. Meta de 12 pontos.",fonte,RGB(241,229,207));
    painel(dc,dir-12,100,300,h-120,RGB(248,244,232));
    texto(dc,dir,116,276,40,"COMO JOGAR AGORA",fonte,RGB(28,73,57));
    char s[600];
    if(jogo.etapa==GUI_CADASTRO) {
        texto(dc,32,112,esq-40,48,"Monte a mesa",fonte_titulo,RGB(244,227,183));
        texto(dc,32,172,esq-40,70,"Preencha os quatro nomes. Os parceiros ficam em posicoes alternadas.",fonte,RGB(237,242,232));
        for(int i=0;i<4;i++) {
            snprintf(s,sizeof(s),"Jogador %d - Dupla %d",i+1,i%2+1);
            texto(dc,32,273+i*54,205,35,s,fonte,RGB(237,242,232));
        }
        texto(dc,32,505,esq-40,100,jogo.aviso[0]?jogo.aviso:"Os nomes ja cadastrados recuperam suas estatisticas automaticamente.",fonte,RGB(244,227,183));
        texto(dc,dir,170,276,140,"1. Preencha os nomes.\n2. Clique em Iniciar partida.\n3. Use os botoes para jogar.",fonte,RGB(47,63,52));
        return;
    }
    snprintf(s,sizeof(s),"DUPLA 1   %02d   x   %02d   DUPLA 2",jogo.placar[1],jogo.placar[2]);
    texto(dc,32,99,esq-40,42,s,fonte_titulo,RGB(250,224,163));
    for(int dupla=1;dupla<=2;dupla++) {
        tp_fila copia=jogo.mesa; tp_jogador j; int n=snprintf(s,sizeof(s),"Dupla %d: ",dupla),primeiro=1;
        while(removeFila(&copia,&j)) if(j.dupla==dupla) { n+=snprintf(s+n,sizeof(s)-(size_t)n,"%s%s",primeiro?"":" + ",j.nome); primeiro=0; }
        texto(dc,32,137+(dupla-1)*25,esq-40,25,s,fonte,RGB(235,241,230));
    }
    painel(dc,24,198,esq-32,64,RGB(221,236,221));
    texto(dc,36,205,esq-55,55,jogo.aviso,fonte,RGB(25,67,50));
    snprintf(s,sizeof(s),"Mao %d | Rodada %d | Vale %d ponto(s)\nVira: %s de %s | Manilha: %s",jogo.numero,
             (jogo.etapa==GUI_RODADA||jogo.etapa==GUI_MAO||jogo.etapa==GUI_FIM)&&!jogo.aposta.encerrada?jogo.rodada:jogo.rodada+1,jogo.aposta.valor,ui_valor(jogo.vira.valor),ui_naipe(jogo.vira.naipe),ui_valor(jogo.manilha));
    texto(dc,32,270,esq-45,55,s,fonte,RGB(235,241,230));
    texto(dc,32,330,esq-45,26,"CARTAS JOGADAS NESTA RODADA",fonte,RGB(250,224,163));
    for(int i=0;i<jogo.qtd_mesa;i++) {
        snprintf(s,sizeof(s),"%s: %s de %s%s",jogo.registro.jogadores[i],ui_valor(jogo.cartas[i].valor),ui_naipe(jogo.cartas[i].naipe),jogo.cartas[i].valor==jogo.manilha?"  [MANILHA]":"");
        /* registro e reinicializado ao copiar para o VAR; nomes permanecem na fila do registro salvo. */
        if(!jogo.registro.jogadores[i][0] && jogo.var && jogo.var->fim)
            snprintf(s,sizeof(s),"%s: %s de %s%s",jogo.var->fim->info.jogadores[i],ui_valor(jogo.cartas[i].valor),ui_naipe(jogo.cartas[i].naipe),jogo.cartas[i].valor==jogo.manilha?"  [MANILHA]":"");
        texto(dc,32,359+i*22,esq-45,22,s,fonte,RGB(235,241,230));
    }
    if(jogo.etapa==GUI_VEZ) {
        tp_jogador j; motor_atual(&jogo,&j);
        snprintf(s,sizeof(s),"VEZ DE %s  |  DUPLA %d",j.nome,j.dupla);
        texto(dc,32,h-228,esq-45,35,s,fonte,RGB(250,224,163));
        texto(dc,dir,170,276,140,jogo.visivel?"Clique na carta que deseja jogar.\n\nVoce tambem pode pedir truco ou enviar um sinal.":"Entregue o controle ao jogador indicado.\n\nClique em Revelar cartas para ver a mao dele.",fonte,RGB(47,63,52));
        texto(dc,dir,380,276,26,"Sinal para o parceiro:",fonte,RGB(47,63,52));
        if(jogo.visivel) texto(dc,32,h-64,esq-45,23,"Clique em uma das cartas acima para joga-la.",fonte,RGB(235,241,230));
    } else if(jogo.etapa==GUI_RESPOSTA) {
        snprintf(s,sizeof(s),"%s responde ao pedido de %s.\n\nAceitar: mao vale %d.\nCorrer: adversario leva %d.\nAumentar: devolve o pedido.",jogo.respondente.nome,nome_pedido(jogo.aposta.pedido),jogo.aposta.pedido,jogo.aposta.valor);
        texto(dc,dir,160,276,160,s,fonte,RGB(47,63,52));
    } else {
        texto(dc,dir,170,276,150,jogo.etapa==GUI_FIM?"Partida encerrada.\n\nVeja as estatisticas e o historico. Para jogar novamente, clique no botao abaixo da mesa.":"Confira o resultado e as cartas da rodada.\n\nClique em Continuar ou Comecar proxima mao.",fonte,RGB(47,63,52));
    }
    if(jogo.erros_disco) texto(dc,dir,660,276,55,"Atencao: ocorreu falha de gravacao nesta partida.",fonte,RGB(165,42,36));
}

static void desenhar_carta(DRAWITEMSTRUCT *d) {
    int i=(int)d->CtlID-ID_CARTA; tp_jogador j;
    if(i<0||i>=3||jogo.etapa!=GUI_VEZ||!jogo.visivel||!motor_atual(&jogo,&j)) return;
    tp_item c=j.mao[i]; HDC dc=d->hDC; RECT r=d->rcItem;
    int salvo=SaveDC(dc); int manilha=c.valor==jogo.manilha;
    painel(dc,0,0,r.right,r.bottom,manilha?RGB(247,213,128):RGB(255,251,240));
    COLORREF cor=(c.naipe=='C'||c.naipe=='O')?RGB(161,43,44):RGB(31,54,44);
    texto(dc,15,7,r.right-25,45,ui_valor(c.valor),fonte_carta,cor);
    if(manilha) texto(dc,65,17,r.right-75,26,c.naipe=='P'?"ZAP":"MANILHA",fonte,cor);
    texto(dc,15,52,r.right-25,28,ui_naipe(c.naipe),fonte,cor);
    char s[80];snprintf(s,sizeof(s),"[%d] Jogar carta",i+1);
    texto(dc,15,87,r.right-25,25,s,fonte,RGB(31,54,44));
    if(d->itemState&ODS_FOCUS) { InflateRect(&r,-4,-4);DrawFocusRect(dc,&r); }
    RestoreDC(dc,salvo);
}

static void anexar(const char *s) {
    SendMessageA(texto_view,EM_SETSEL,(WPARAM)-1,(LPARAM)-1);
    SendMessageA(texto_view,EM_REPLACESEL,FALSE,(LPARAM)s);
}
static void linha_ficha(const info_caderneta *i) {
    char s[360];
    snprintf(s,sizeof(s),"%s\r\nPartidas: %d | Maos ganhas: %d\r\nPedidos: %d | Recebidos: %d | Fugas: %d\r\n",i->nome_jogador,i->partidas_jogadas,i->maos_ganhas,i->trucos_pedidos,i->trucos_recebidos,i->trucos_corridos);anexar(s);
    if(i->trucos_recebidos) { snprintf(s,sizeof(s),"Taxa de fuga: %.2f%%\r\n\r\n",taxa_correu(*i));anexar(s); }
    else anexar("Taxa de fuga: sem pedidos recebidos\r\n\r\n");
}
static void listar_nomes(struct NO *no) { if(!no)return;listar_nomes(no->esq);linha_ficha(&no->info);listar_nomes(no->dir); }
static void preencher_view(int ordem) {
    SetWindowTextA(texto_view,"");char s[400];
    if(view_tipo==ID_VAR && view_rodada) {
        tp_item_VAR *v=&view_rodada->info;
        snprintf(s,sizeof(s),"MAO %d - RODADA %d\r\nManilha: %s\r\n",v->mao,v->rodada,ui_valor(v->manilha));anexar(s);
        if(!v->em_andamento&&!v->encerrada_por_fuga){snprintf(s,sizeof(s),"Dupla vencedora: %d (0 = empate)\r\n",v->dupla_vencedora);anexar(s);}
        if(v->em_andamento)anexar("RODADA EM ANDAMENTO: somente cartas ja jogadas.\r\nAinda nao ha vencedora desta rodada.\r\n\r\n");
        snprintf(s,sizeof(s),"Valor aceito da mao: %d ponto(s).\r\n",v->pontos_em_jogo);anexar(s);
        if(v->encerrada_por_fuga){snprintf(s,sizeof(s),"Mao encerrada por fuga: Dupla %d recebeu %d ponto(s).\r\n",v->dupla_vencedora,v->pontos_em_jogo);anexar(s);}
        for(int i=0;i<4;i++) { if(v->cartas_jogadas[i].valor<1)snprintf(s,sizeof(s),"%s: nao jogou\r\n",v->jogadores[i]);else snprintf(s,sizeof(s),"%s: %s de %s\r\n",v->jogadores[i],ui_valor(v->cartas_jogadas[i].valor),ui_naipe(v->cartas_jogadas[i].naipe));anexar(s); }
        anexar("\r\nNEGOCIACAO DA APOSTA\r\n");
        if(v->em_andamento&&jogo.etapa==GUI_RESPOSTA){snprintf(s,sizeof(s),"%s pediu %d. Aguardando resposta de %s.\r\n",jogo.pediu,jogo.aposta.pedido,jogo.respondente.nome);anexar(s);}
        if(!v->qtd_negociacoes&&!(v->em_andamento&&jogo.etapa==GUI_RESPOSTA))anexar("Nenhum pedido novo nesta rodada.\r\n");
        for(int i=0;i<v->qtd_negociacoes;i++) {tp_negociacao_VAR *n=&v->negociacoes[i];snprintf(s,sizeof(s),"%s pediu %d. %s %s\r\n",n->pediu,n->valor,n->respondeu,n->resposta=='A'?"aceitou.":n->resposta=='C'?"correu.":"aumentou.");anexar(s);}
        EnableWindow(GetDlgItem(janela_view,ID_ANTERIOR),view_rodada->ant!=NULL);
        EnableWindow(GetDlgItem(janela_view,ID_PROXIMA),view_rodada->prox!=NULL||(view_tem_parcial&&view_rodada==jogo.var->fim));
    } else if(view_tipo==ID_STATS) {
        if(jogo.maior) {snprintf(s,sizeof(s),"MAO MAIS CARA DESTA PARTIDA: %d ponto(s), mao %d.\r\n\r\n",jogo.maior,jogo.numero_maior);anexar(s);}
        anexar("Pasta dos arquivos de dados:\r\n");anexar(disco_diretorio);anexar("\r\n\r\n");
        if(jogo.erros_disco){anexar("ATENCAO: houve falha de gravacao:\r\n");anexar(jogo.erro_disco);anexar("\r\n\r\n");}
        if(!*jogo.caderneta)anexar("Nenhum jogador cadastrado.\r\n");
        if(ordem==ID_ORDEM)listar_nomes(*jogo.caderneta);
        else {int n=0;struct NO **lista=ordem==ID_MAOS?mural(jogo.caderneta,&n):mural_vergonha(jogo.caderneta,&n);if(!lista&&*jogo.caderneta)anexar("Nao foi possivel montar o ranking.\r\n");for(int i=0;i<n;i++)linha_ficha(&lista[i]->info);free(lista);}
    }
    SendMessage(texto_view,EM_SETSEL,0,0);SendMessage(texto_view,EM_SCROLLCARET,0,0);
}
static LRESULT CALLBACK ViewProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_CREATE) {
        janela_view=hwnd;
        texto_view=CreateWindowExA(WS_EX_CLIENTEDGE,"EDIT","",WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL,16,16,690,425,hwnd,NULL,GetModuleHandle(NULL),NULL);
        SendMessage(texto_view,WM_SETFONT,(WPARAM)fonte,TRUE);SendMessage(texto_view,EM_SETLIMITTEXT,0x7ffffffe,0);
        int ids[3]={view_tipo==ID_VAR?ID_ANTERIOR:ID_ORDEM,view_tipo==ID_VAR?ID_PROXIMA:ID_MAOS,ID_TAXA};
        const char *labels[3]={view_tipo==ID_VAR?"Rodada anterior":"Ordem alfabetica",view_tipo==ID_VAR?"Proxima rodada":"Menos maos ganhas","Maior taxa de fuga"};
        int total=view_tipo==ID_VAR?2:3;
        for(int i=0;i<total;i++){HWND b=CreateWindowExA(0,"BUTTON",labels[i],WS_CHILD|WS_VISIBLE|WS_TABSTOP,16+i*234,455,220,40,hwnd,(HMENU)(INT_PTR)ids[i],GetModuleHandle(NULL),NULL);SendMessage(b,WM_SETFONT,(WPARAM)fonte,TRUE);}
        preencher_view(ID_ORDEM);return 0;
    }
    if(msg==WM_COMMAND){
        int id=LOWORD(wp);
        if(HIWORD(wp)!=BN_CLICKED||id<ID_ANTERIOR||id>ID_TAXA)return 0;
        if(view_tipo==ID_VAR){
            if(id==ID_ANTERIOR&&view_rodada->ant)view_rodada=view_rodada->ant;
            if(id==ID_PROXIMA){
                if(view_rodada->prox)view_rodada=view_rodada->prox;
                else if(view_tem_parcial&&view_rodada==jogo.var->fim)view_rodada=&view_parcial;
            }
        }
        preencher_view(id);return 0;
    }
    if(msg==WM_CLOSE){DestroyWindow(hwnd);return 0;}
    if(msg==WM_DESTROY){janela_view=NULL;EnableWindow(principal,TRUE);SetActiveWindow(principal);return 0;}
    return DefWindowProcA(hwnd,msg,wp,lp);
}
static void abrir_view(int tipo) {
    view_tipo=tipo;view_rodada=jogo.var?jogo.var->fim:NULL;
    view_tem_parcial=jogo.etapa==GUI_VEZ||jogo.etapa==GUI_RESPOSTA;
    if(view_tem_parcial){
        memset(&view_parcial,0,sizeof(view_parcial));
        view_parcial.info=jogo.registro;view_parcial.info.pontos_em_jogo=jogo.aposta.valor;
        view_parcial.ant=view_rodada;view_rodada=&view_parcial;
    }
    EnableWindow(principal,FALSE);
    HWND h=CreateWindowExA(WS_EX_DLGMODALFRAME,"PatoView",tipo==ID_VAR?"Historico da partida - VAR":"Caderneta e estatisticas",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,80,80,745,550,principal,NULL,GetModuleHandle(NULL),NULL);
    if(h)ShowWindow(h,SW_SHOW);else EnableWindow(principal,TRUE);
}

static LRESULT CALLBACK JanelaProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_CREATE) {
        principal=hwnd;
        for(int i=0;i<4;i++) {nomes_edit[i]=controle("EDIT","",WS_BORDER|ES_AUTOHSCROLL,ID_NOME+i);SendMessage(nomes_edit[i],EM_SETLIMITTEXT,NOME_MAX-1,0);}
        for(int i=0;i<3;i++)cartas_btn[i]=controle("BUTTON","Jogar carta",BS_OWNERDRAW,ID_CARTA+i);
        iniciar_btn=controle("BUTTON","Iniciar partida",BS_DEFPUSHBUTTON,ID_INICIAR);
        continuar_btn=controle("BUTTON","Continuar",BS_PUSHBUTTON,ID_CONTINUAR);
        revelar_btn=controle("BUTTON","Revelar minhas cartas",BS_PUSHBUTTON,ID_REVELAR);
        pedir_btn=controle("BUTTON","Pedir truco",BS_PUSHBUTTON,ID_PEDIR);
        aceitar_btn=controle("BUTTON","Aceitar pedido",BS_PUSHBUTTON,ID_ACEITAR);
        correr_btn=controle("BUTTON","Correr",BS_PUSHBUTTON,ID_CORRER);
        aumentar_btn=controle("BUTTON","Aumentar pedido",BS_PUSHBUTTON,ID_AUMENTAR);
        gesto_combo=controle("COMBOBOX","",CBS_DROPDOWNLIST,ID_GESTO);
        const char *gestos[3]={"Piscar","Levantar a sobrancelha","Tocar na mesa"};
        for(int i=0;i<3;i++) { SendMessageA(gesto_combo,CB_ADDSTRING,0,(LPARAM)gestos[i]); }
        SendMessage(gesto_combo,CB_SETCURSEL,0,0);
        sinal_btn=controle("BUTTON","Enviar sinal ao parceiro",BS_PUSHBUTTON,ID_SINAL);
        var_btn=controle("BUTTON","Ver historico (VAR)",BS_PUSHBUTTON,ID_VAR);
        stats_btn=controle("BUTTON","Caderneta e estatisticas",BS_PUSHBUTTON,ID_STATS);
        ajuda_btn=controle("BUTTON","Ajuda: todos os comandos",BS_PUSHBUTTON,ID_AJUDA);
        atualizar();return 0;
    }
    if(msg==WM_GETMINMAXINFO){MINMAXINFO *m=(MINMAXINFO*)lp;m->ptMinTrackSize.x=1000;m->ptMinTrackSize.y=720;return 0;}
    if(msg==WM_SIZE){if(iniciar_btn)atualizar();return 0;}
    if(msg==WM_COMMAND) {
        int id=LOWORD(wp);
        if(id>=ID_NOME&&id<ID_NOME+4)return 0;
        if(id==ID_GESTO)return 0;
        if(id==ID_INICIAR&&jogo.etapa==GUI_CADASTRO){char nomes[4][NOME_MAX];for(int i=0;i<4;i++){GetWindowTextA(nomes_edit[i],nomes[i],NOME_MAX);size_t a=0;while(nomes[i][a]==' ')a++;memmove(nomes[i],nomes[i]+a,strlen(nomes[i]+a)+1);size_t n=strlen(nomes[i]);while(n&&nomes[i][n-1]==' ')nomes[i][--n]='\0';}if(!motor_iniciar(&jogo,nomes))MessageBoxA(hwnd,jogo.aviso,"Confira o cadastro",MB_OK|MB_ICONINFORMATION);avisados=0;}
        else if(id==ID_REVELAR&&jogo.etapa==GUI_VEZ)jogo.visivel=1;
        else if(id>=ID_CARTA&&id<ID_CARTA+3)motor_jogar(&jogo,id-ID_CARTA+1);
        else if(id==ID_PEDIR)motor_pedir(&jogo);
        else if(id==ID_ACEITAR)motor_responder(&jogo,'A');
        else if(id==ID_CORRER)motor_responder(&jogo,'C');
        else if(id==ID_AUMENTAR)motor_responder(&jogo,'T');
        else if(id==ID_SINAL)motor_sinal(&jogo,(int)SendMessage(gesto_combo,CB_GETCURSEL,0,0));
        else if(id==ID_CONTINUAR){if(jogo.etapa==GUI_FIM)jogo.etapa=GUI_CADASTRO;else motor_continuar(&jogo);}
        else if(id==ID_VAR||id==ID_STATS){abrir_view(id);return 0;}
        else if(id==ID_AJUDA)MessageBoxA(hwnd,"INICIAR: informe quatro nomes; jogadores 1 e 3 sao parceiros.\n\nREVELAR CARTAS: entrega a vez ao jogador indicado.\nJOGAR: clique na carta desejada.\nPEDIR TRUCO: aumenta o valor da mao, se permitido.\nACEITAR / CORRER / AUMENTAR: respondem ao pedido.\nSINAL: selecione o gesto e envie; voce mantem a vez.\nCONTINUAR: confirma a leitura do resultado da rodada.\nHISTORICO: inclui a rodada em andamento, so com cartas ja jogadas.\nESTATISTICAS: consulta jogadores, rankings e a pasta dos arquivos.\n\nCaderneta e historico sao criados ao iniciar a partida. Pedidos e respostas de truco sao salvos na hora; os resultados sao salvos ao terminar a mao.","Como jogar",MB_OK|MB_ICONINFORMATION);
        atualizar();return 0;
    }
    if(msg==WM_DRAWITEM){desenhar_carta((DRAWITEMSTRUCT*)lp);return TRUE;}
    if(msg==WM_PAINT){PAINTSTRUCT p;HDC dc=BeginPaint(hwnd,&p);pintar(dc);EndPaint(hwnd,&p);return 0;}
    if(msg==WM_CTLCOLORSTATIC||msg==WM_CTLCOLOREDIT){SetTextColor((HDC)wp,RGB(25,60,44));SetBkColor((HDC)wp,RGB(255,251,240));return (LRESULT)papel;}
    if(msg==WM_CLOSE){if(jogo.etapa!=GUI_CADASTRO&&jogo.etapa!=GUI_FIM&&MessageBoxA(hwnd,"Sair da partida? Os dados da ultima mao concluida ja foram salvos, se nao houve erro de gravacao.","Encerrar jogo",MB_YESNO|MB_ICONQUESTION)!=IDYES)return 0;DestroyWindow(hwnd);return 0;}
    if(msg==WM_DESTROY){if(janela_view)DestroyWindow(janela_view);if(jogo.var)Destroi_listad(jogo.var);liberaABB(jogo.caderneta);DeleteObject(fonte);DeleteObject(fonte_titulo);DeleteObject(fonte_carta);DeleteObject(fundo);DeleteObject(papel);PostQuitMessage(0);return 0;}
    return DefWindowProcA(hwnd,msg,wp,lp);
}
static int executar_janela(HINSTANCE instancia,int mostrar_janela) {
    SetProcessDPIAware();
    if(!disco_iniciar()){MessageBoxA(NULL,"Nao foi possivel localizar a pasta dos arquivos de dados.","Erro",MB_OK|MB_ICONERROR);return 1;}
    jogo.caderneta=criarAVL();
    if(!jogo.caderneta)return 1;
    if(carregar_caderneta(jogo.caderneta)==DISCO_ERRO){MessageBoxA(NULL,disco_ultimo_erro,"Erro na caderneta - arquivo preservado",MB_OK|MB_ICONERROR);liberaABB(jogo.caderneta);return 1;}
    inicializar_seed();
    fonte=CreateFontA(-18,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,"Segoe UI");
    fonte_titulo=CreateFontA(-28,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,"Segoe UI");
    fonte_carta=CreateFontA(-36,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,"Segoe UI");
    fundo=CreateSolidBrush(RGB(23,70,51));papel=CreateSolidBrush(RGB(255,251,240));
    WNDCLASSA wc={0};wc.lpfnWndProc=JanelaProc;wc.hInstance=instancia;wc.lpszClassName="BarDoPato";wc.hCursor=LoadCursor(NULL,IDC_ARROW);wc.hbrBackground=fundo;
    if(!RegisterClassA(&wc))return 1;
    wc.lpfnWndProc=ViewProc;wc.lpszClassName="PatoView";wc.hbrBackground=papel;
    if(!RegisterClassA(&wc))return 1;
    RECT area={0,0,1120,820}; SystemParametersInfoA(SPI_GETWORKAREA,0,&area,0);
    int largura=area.right-area.left<1120?area.right-area.left:1120;
    int altura=area.bottom-area.top<820?area.bottom-area.top:820;
    HWND h=CreateWindowExA(0,"BarDoPato","Truco no Bar do Pato",WS_OVERLAPPEDWINDOW,area.left+(area.right-area.left-largura)/2,area.top+(area.bottom-area.top-altura)/2,largura,altura,NULL,NULL,instancia,NULL);
    if(!h){MessageBoxA(NULL,"Nao foi possivel criar a janela do jogo.","Erro",MB_OK|MB_ICONERROR);return 1;}
    ShowWindow(h,mostrar_janela);UpdateWindow(h);
    MSG msg;int leitura;
    while((leitura=(int)GetMessageA(&msg,NULL,0,0))>0){HWND dialogo=janela_view?janela_view:h;if(!IsDialogMessageA(dialogo,&msg)){TranslateMessage(&msg);DispatchMessageA(&msg);}}
    return leitura<0?1:(int)msg.wParam;
}
int WINAPI WinMain(HINSTANCE instancia,HINSTANCE anterior,LPSTR comando,int mostrar_janela) {
    (void)anterior;(void)comando;return executar_janela(instancia,mostrar_janela);
}
/* Permite compilar como arquivo C avulso em IDE configurada para console. */
int main(void){return executar_janela(GetModuleHandle(NULL),SW_SHOWDEFAULT);}
#else
#include <stdio.h>
int main(void){fputs("A interface grafica gui.c e para Windows. Use main.c para o terminal.\n",stderr);return 1;}
#endif
