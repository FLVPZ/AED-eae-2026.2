# Relatório de participação — 3ª unidade

## Felipe Vieira — Item criativo e integração da interface

Implementei o módulo `include/criativo.h` com o destaque visual das manilhas e uma opção para enviar sinais ao parceiro durante a jogada. As cartas da mão e as cartas jogadas recebem a marca `[MANILHA]`, com um destaque específico para o zap de Paus.

Na opção `S`, o jogador escolhe entre piscar, levantar a sobrancelha ou tocar na mesa. O parceiro é identificado pela dupla em uma cópia da fila, respeitando os giros da mesa. O sinal mantém a vez do jogador, e o significado dos gestos fica combinado entre os participantes.

Integrei essa opção ao fluxo de `Partida.h`, acrescentei a apresentação dos comandos na `main.c` e atualizei o README. Os sinais não consultam as cartas do parceiro e não alteram as cartas, a aposta, a pontuação ou o embaralhamento.

Também removi de `Partida.h` as definições duplicadas de `vencedor_da_rodada` e `vencedor_da_mao`, que impediam a compilação. A partida passou a usar as implementações já existentes em `jogo.h`.

Validei a compilação em C99 com avisos habilitados, os quatro naipes de manilha, os três sinais, o cancelamento, as opções inválidas, a identificação do parceiro após giros da fila e o registro de uma rodada no VAR. Comparei uma partida completa com uma referência sem o item criativo, usando o mesmo embaralhamento: as cartas jogadas e a pontuação permaneceram iguais.

## Consolidação da entrega

A integração final com os módulos de disco, mural e estatísticas e as contribuições dos demais integrantes ainda precisam ser acrescentadas para concluir o relatório da terceira unidade.
