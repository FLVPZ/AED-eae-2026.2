# Truco no Bar do Pato

Projeto de Algoritmos e Estrutura de Dados do curso de Engenharia da Computação do SENAI CIMATEC, semestre 2026.2.

O jogo é desenvolvido em C, com quatro jogadores organizados em duas duplas, seguindo a proposta de Truco Paulista.

## Como compilar e jogar

```
gcc main.c -o truco
./truco
```

No Windows (Dev-C++ ou gcc) é o mesmo comando, e o executável gerado é o `truco.exe`.

## Organização do projeto

O `main.c`, o `README.md` e o `.gitignore` ficam na raiz. Todos os módulos ficam na pasta `include/`:

- `STACK.h`: carta e pilha do baralho
- `baralho.h` e `Embaralhar.h`: criação e embaralhamento das 40 cartas
- `fila.h`: fila da mesa e struct do jogador
- `Jogador.h`: cadastro dos jogadores e montagem da mesa
- `Roda.h`: ordem de jogo (quem é "mão" e giro da roda)
- `Mao.h`: mão do jogador (jogar qualquer carta, ordenar, exibir)
- `jogo.h`: distribuição, vira, manilha e força das cartas
- `Rodada.h`: vencedor da rodada e da mão
- `APOSTA.h` e `Truco.h`: pedir truco, aumentar, aceitar e correr
- `listade.h` e `var.h`: lista duplamente encadeada e navegação do VAR
- `Historico.h`: registro do histórico da partida
- `Partida.h`: loop da partida, mãos e rodadas

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
