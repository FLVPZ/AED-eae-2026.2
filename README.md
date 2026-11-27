# Truco no Bar do Pato

Projeto de Algoritmos e Estrutura de Dados do curso de Engenharia da Computação do SENAI CIMATEC, semestre 2026.2.

Jogo em C baseado nas regras de Truco Paulista definidas no enunciado: quatro jogadores, duas duplas alternadas na mesa e partidas até uma dupla atingir 12 pontos. Usa baralho de 40 cartas, vira, manilhas e pedidos de truco, seis e doze. A interface funciona no terminal, no tema do Bar do Pato, com placar, cartas e comandos visíveis durante a jogada.

## Como compilar e executar

É necessário um compilador C com suporte a C99, como o GCC. Abra o terminal na pasta raiz do projeto, onde estão `main.c` e a pasta `include/`. Compile somente `main.c`; os módulos `.h` são incluídos pelo programa.

### Linux e macOS

```bash
gcc -std=c99 -Wall -Wextra -pedantic main.c -o truco
./truco
```

### Windows com GCC — PowerShell

```powershell
gcc -std=c99 -Wall -Wextra -pedantic main.c -o truco.exe
.\truco.exe
```

No Prompt de Comando (CMD), use a mesma compilação e execute `truco.exe`. Se utilizar uma IDE, como o Dev-C++, compile `main.c` como C com suporte a C99 e mantenha a pasta `include/` junto do código.

## Como jogar

Informe os quatro nomes na ordem da mesa: dupla 1, dupla 2, dupla 1, dupla 2. Assim, o primeiro e o terceiro jogadores são parceiros, assim como o segundo e o quarto. Os nomes não podem ser vazios, repetidos na mesma mesa, conter ponto e vírgula ou caracteres de controle. O limite é de 29 bytes; letras acentuadas podem ocupar mais de um byte, dependendo da codificação do terminal.

A caderneta aparece em ordem alfabética, seguida dos murais. Em terminal interativo com suporte ANSI, pressione Enter quando solicitado para abrir a mesa.

| Tela | Comando | Ação |
| --- | --- | --- |
| Mesa | `1`, `2` ou `3` | Jogar uma das cartas disponíveis na mão. |
| Mesa | `T` | Pedir o próximo valor da aposta, quando essa opção estiver disponível. |
| Mesa | `S` | Abrir o menu de sinais fixos para o parceiro. |
| Mesa | `V` | Consultar o VAR sem passar a vez. |
| Sinais | `1`, `2` ou `3` | Piscar, levantar a sobrancelha ou tocar na mesa. |
| Sinais | `0` | Cancelar o sinal e voltar à jogada. |
| Resposta à aposta | `A`, `C` ou `T` | Aceitar, correr ou aumentar, quando permitido. |
| VAR | `1`, `2` ou `3` | Voltar, avançar uma rodada ou retornar à rodada anterior. |

A tela da mesa reúne o placar, as duplas, a mão e a rodada atuais, o valor da aposta, a vira, a manilha e as cartas já jogadas. As manilhas recebem o rótulo `[MANILHA]`; a de Paus recebe `[MANILHA - ZAP]`. Os sinais são apenas uma interação visual e mantêm o jogador na vez. Combinem o significado dos gestos antes da partida.

No encerramento, o programa anuncia a dupla vencedora e o placar. Após a pausa de leitura, abre o VAR; ao sair dele, mostra os murais finais.


## Caderneta e arquivos de dados

Ao iniciar, o programa procura `caderneta.txt` na pasta de execução, antes do cadastro. Se o arquivo não existir, começa com uma caderneta vazia. Jogadores encontrados pelo mesmo nome mantêm seus contadores; novos jogadores entram zerados. A comparação diferencia maiúsculas e minúsculas.

Ao fim de cada mão, salva a caderneta e acrescenta o número da mão, a dupla vencedora, os pontos e o placar em `historico.txt`. A caderneta também é salva no encerramento da partida. Esses arquivos são criados pelo programa: não é necessário prepará-los manualmente. Execute sempre pela mesma pasta para recuperar os mesmos dados.

Cada linha de `caderneta.txt` contém `nome;partidas;maos;pedidos;corridos;recebidos`. A gravação usa `caderneta.txt.tmp` e substitui a caderneta somente após concluir a escrita. Um arquivo inválido ou antigo, sem pedidos recebidos, é preservado e impede o início do jogo até que o problema seja resolvido. A caderneta, o histórico e o temporário são ignorados pelo Git.

As estatísticas são acumuladas por nome: pedidos feitos, pedidos recebidos, fugas e mãos ganhas são atualizados durante o jogo; uma partida jogada é registrada para os quatro jogadores ao concluir a partida. Cada pedido de truco, seis ou doze conta uma vez para quem efetivamente responde, mesmo que digite opções inválidas antes de decidir. A taxa de fuga é `corridos / recebidos × 100`; sem pedidos recebidos, o mural informa essa condição. Cada mão ganha conta para os dois integrantes da dupla vencedora.

No encerramento, o programa mostra a mão de maior valor efetivamente pontuado (1, 3, 6 ou 12); em empate, mantém a primeira. O VAR identifica quem jogou cada carta e registra toda a negociação realizada em cada rodada, incluindo aumentos, aceite e fuga.

## Organização do projeto

O arquivo `main.c` organiza o carregamento da caderneta, o cadastro, a partida, a exibição dos murais e a liberação da memória. Os módulos ficam em `include/`:

- `STACK.h`: tipos da carta e da pilha, com operações sobre o topo do baralho.
- `baralho.h` e `Embaralhar.h`: criação e embaralhamento das 40 cartas.
- `fila.h`: estrutura do jogador e fila da mesa.
- `Jogador.h`: leitura dos nomes, cadastro e montagem das duplas.
- `Roda.h`: ordem dos jogadores e giro da mesa a cada nova mão.
- `Mao.h`: ordenação da mão, escolha e remoção da carta jogada.
- `jogo.h`: distribuição, vira, manilha, comparação das cartas e apuração dos vencedores.
- `APOSTA.h`: pedidos de truco, seis e doze, aceite e fuga, com atualização das estatísticas e registro das negociações no VAR.
- `estatisticas.h`: contadores por jogador, vitórias da dupla, partidas concluídas e maior valor de mão.
- `listade.h` e `var.h`: histórico da partida e navegação nos dois sentidos.
- `Partida.h`: condução das mãos e rodadas, placar e chamadas de salvamento.
- `criativo.h`: sinais fixos para o parceiro e destaque visual das manilhas.
- `interface.h`: apresentação no terminal, cartas, cores opcionais, avisos e pausas.
- `caderneta.h`: cadastro em árvore AVL, busca por nome, percursos e liberação da árvore.
- `mural.h`: ordenação e exibição dos murais de mãos ganhas e taxa de fuga.
- `disco.h`: leitura e gravação da caderneta e histórico de pontos por mão.

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

- [x] Revisar e corrigir os problemas identificados nas entregas anteriores.
- [x] Encerrar a partida aos 12 pontos, mostrando a dupla vencedora e o placar final.
- [x] Criar a caderneta em árvore AVL, com busca por nome e cadastro de novos jogadores com os contadores zerados.
- [x] Implementar a listagem da caderneta em ordem alfabética.
- [x] Integrar a caderneta ao cadastro dos jogadores.
- [x] Atualizar partidas jogadas, mãos ganhas, pedidos feitos, pedidos recebidos e fugas durante o jogo.
- [x] Salvar os dados ao fim de cada mão e recuperar a caderneta ao abrir o programa novamente.
- [x] Exibir o mural com os jogadores em ordem crescente de maos ganhas.
- [x] Exibir o mural da taxa de fuga calculada a partir dos contadores da caderneta.
- [x] Identificar e exibir a mão mais cara da partida.
- [x] Implementar o item criativo: sinais do parceiro e marcação visual das manilhas.
- [x] Organizar a interface no terminal com placar, cartas, comandos e resumos no tema do Bar do Pato.
- [x] Corrigir as alturas e o balanceamento da AVL após remoções.
- [x] Integrar a caderneta e os murais na `main.c`, com liberação dos vetores e da árvore.
- [x] Integrar a persistência na `main.c`, no fim de cada mão e no encerramento da partida.
- [x] Preparar o relatório com a participação de cada integrante.
