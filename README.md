# Truco no Bar do Pato

Projeto de Algoritmos e Estrutura de Dados do curso de Engenharia da Computação do SENAI CIMATEC, semestre 2026.2.

O jogo é desenvolvido em C, com quatro jogadores organizados em duas duplas, seguindo a proposta de Truco Paulista. A interface funciona no terminal, com o tema do Bar do Pato.

## Como compilar e jogar

```
gcc main.c -o truco
./truco
```

No Windows (Dev-C++ ou gcc) é o mesmo comando, e o executável gerado é o `truco.exe`.

Durante a jogada, use `S` para escolher um sinal ao parceiro. Nesse menu, use `0` para voltar ao jogo. Enviar um sinal mantém a sua vez. As manilhas aparecem com `[MANILHA]`, e a manilha de Paus recebe o destaque `[MANILHA - ZAP]`.

A tela da mesa reúne o placar, as duplas, a vira, a manilha, as cartas já jogadas e a mão do jogador da vez. As cartas aparecem lado a lado, com cores de naipe e destaque dourado nas manilhas. Os menus de aposta, sinais e VAR mostram as teclas disponíveis. O último aviso permanece na mesa após consultar o VAR, enviar um sinal ou digitar uma opção inválida.

Use de preferência um terminal com pelo menos 80 colunas e 28 linhas. Cores e limpeza de tela são ativadas automaticamente quando o console oferece suporte. Com a variável `NO_COLOR` definida, as cores são desativadas; com `TERM=dumb` ou saída redirecionada, não são enviados códigos de controle. Em janelas estreitas, as cartas passam para uma lista compacta. No Windows, se o console não aceitar sequências ANSI, a apresentação continua em texto simples. Os rótulos dos naipes e das manilhas aparecem também por escrito.

Antes de começar e antes de conferir o VAR final, o terminal interativo aguarda Enter para permitir a leitura dos resumos. Esses passos não consomem entrada quando o programa recebe dados por redirecionamento.

Os quatro nomes sao cadastrados na caderneta AVL. Nomes vazios, repetidos na mesma mesa ou com mais de 29 caracteres precisam ser informados novamente. A caderneta aparece em ordem alfabetica antes da partida, e os murais aparecem antes e depois do jogo.

Esta integracao e parcial: os contadores ainda nao sao atualizados durante a partida e os dados ainda nao sao salvos em disco. Por isso, jogadores novos aparecem com zero maos ganhas e sem pedidos recebidos. Os modulos de estatisticas e persistencia serao conectados quando estiverem prontos.

## Organização do projeto

O `main.c`, o `README.md` e o `.gitignore` ficam na raiz. Todos os módulos ficam na pasta `include/`:

- `STACK.h`: carta e pilha do baralho
- `baralho.h` e `Embaralhar.h`: criação e embaralhamento das 40 cartas
- `fila.h`: fila da mesa e struct do jogador
- `Jogador.h`: cadastro dos jogadores e montagem da mesa
- `Roda.h`: ordem de jogo (quem é "mão" e giro da roda)
- `Mao.h`: mão do jogador (jogar qualquer carta, ordenar, exibir)
- `jogo.h`: distribuição, vira, manilha, força das cartas e vencedor da rodada e da mão
- `APOSTA.h`: pedir truco, aumentar, aceitar e correr
- `listade.h` e `var.h`: lista duplamente encadeada e navegação do VAR
- `Partida.h`: loop da partida, mãos e rodadas
- `criativo.h`: destaque das manilhas e sinais fixos escolhidos pelo jogador para o parceiro
- `interface.h`: tema do terminal, cores opcionais, cartas, cabeçalhos, avisos e pausas de leitura
- `caderneta.h`: cadastro em arvore AVL integrado aos nomes da mesa, busca por nome, percursos e liberacao da arvore
- `mural.h`: ordenacao e exibicao dos murais de maos ganhas e taxa de fuga, preservando os nos da caderneta

## 1ª unidade

A primeira entrega prepara a mesa e o baralho. As cartas serão distribuídas e mostradas na tela. As jogadas ficam para a próxima unidade.

- [x] Cadastrar os quatro jogadores, mantendo nome, dupla e cartas de cada um juntos.
- [x] Organizar a mesa com as duplas alternadas: dupla 1, dupla 2, dupla 1, dupla 2.
- [x] Controlar a ordem dos jogadores e passar o primeiro para o fim a cada nova mão.
- [x] Criar e embaralhar as 40 cartas do baralho.
- [x] Retirar as cartas apenas pelo topo do baralho.
- [x] Distribuir três cartas para cada jogador.
- [x] Revelar a vira, calcular a manilha e mostrar as cartas de cada jogador.
- [x] Dividir o código em módulos e comentar o funcionamento.
- [x] Preparar o relatório com a participação de cada integrante.

## 2ª unidade

A segunda entrega coloca a mão para rolar e liga o VAR da mesa.

- [x] Transformar a mão do jogador em algo jogável: ele escolhe qualquer carta (da ponta ou do meio), ela sai da mão para a mesa e não volta até a próxima distribuição.
- [x] Jogar a rodada com as 4 cartas na mesa, comparando a força com a manilha valendo.
- [x] Apontar quem levou a rodada e quem levou a mão (2 de 3, com a regra de empate na 1ª rodada).
- [x] Pedir truco, aumentar (seis e doze), aceitar e correr, ajustando o valor da mão (1, 3, 6 ou 12 pontos).
- [x] Passar a vez de responder de uma dupla para a outra a cada pedido.
- [x] Girar a roda a cada nova mão, aproveitando a roda da 1ª unidade.
- [x] Ligar o VAR do começo ao fim da partida, registrando cada rodada com as cartas, a dupla vencedora e a aposta.
- [x] Navegar no VAR rodada por rodada nos dois sentidos, atravessando de uma mão para a outra (opção `V` durante a jogada e no fim da partida).
- [x] Preparar o relatório com a participação de cada integrante.

## 3ª unidade

A terceira entrega fecha as contas do bar, com caderneta, estatísticas, dados salvos em disco e um item criativo.

Andamento atual: a caderneta de Eduardo e os murais de Diego estão integrados ao cadastro e à exibição do jogo. A AVL foi corrigida e o item criativo está funcionando. Faltam a atualização das estatísticas durante a partida, a mão mais cara e a persistência em disco. Até esses módulos serem conectados, os jogadores novos aparecem com contadores zerados e os dados ficam somente na execução atual.

- [ ] Concluir as correções solicitadas nas entregas anteriores.
- [x] Encerrar a partida aos 12 pontos, mostrando a dupla vencedora e o placar final.
- [x] Criar a caderneta em árvore AVL, com busca por nome e cadastro de novos jogadores com os contadores zerados.
- [x] Implementar a listagem da caderneta em ordem alfabética.
- [x] Integrar a caderneta ao cadastro dos jogadores.
- [ ] Atualizar partidas jogadas, mãos ganhas, pedidos feitos, pedidos recebidos e fugas durante o jogo.
- [ ] Salvar os dados ao fim de cada mão e recuperar a caderneta ao abrir o programa novamente.
- [x] Exibir o mural com os jogadores em ordem crescente de maos ganhas.
- [x] Exibir o mural da taxa de fuga calculada a partir dos contadores da caderneta.
- [ ] Identificar e exibir a mão mais cara da partida.
- [x] Implementar o item criativo: sinais do parceiro e marcação visual das manilhas.
- [x] Organizar a interface no terminal com placar, cartas, comandos e resumos no tema do Bar do Pato.
- [x] Corrigir as alturas e o balanceamento da AVL após remoções.
- [x] Integrar a caderneta e os murais na `main.c`, com liberação dos vetores e da árvore.
- [ ] Integrar os módulos de estatísticas e persistência quando estiverem prontos.
- [ ] Preparar o relatório com a participação de cada integrante.
