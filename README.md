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
- `jogo.h`: distribuição, vira, manilha, força das cartas e vencedor da rodada e da mão
- `APOSTA.h`: pedir truco, aumentar, aceitar e correr
- `listade.h` e `var.h`: lista duplamente encadeada e navegação do VAR
- `Partida.h`: loop da partida, mãos e rodadas
- `caderneta.h`: cadastro acumulado em árvore AVL, busca por nome, percursos e liberação da árvore; ainda sem integração com a partida

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

A terceira entrega fecha as contas do bar: correções anteriores, caderneta persistente, mural, estatísticas e item criativo.

**Situação em 26/11/2026:** checklist baseado na leitura do código disponível na branch `main`. Um item marcado indica implementação presente no escopo descrito; não representa validação completa por testes. Trabalho local dos integrantes ainda não enviado ao repositório não está contabilizado.

### O que já está no repositório

- [x] Encerrar a partida quando uma dupla alcançar ou ultrapassar 12 pontos, anunciando os vencedores e o placar final em `Partida.h`.
- [x] Criar a estrutura da caderneta em `include/caderneta.h`, usando uma árvore AVL ordenada pelo nome.
- [x] Armazenar nome, partidas jogadas, mãos ganhas, trucos pedidos e trucos corridos em `info_caderneta`.
- [x] Implementar busca por nome e inserção com balanceamento AVL, para busca logarítmica.
- [x] Disponibilizar `acessar_jogador`: recuperar um jogador existente ou inserir um novo com os contadores zerados.
- [x] Implementar listagem alfabética pelo percurso em ordem (`emOrd`).
- [x] Disponibilizar percursos em pré-ordem e pós-ordem e funções para liberar a árvore.

### O que falta concluir ou integrar

**Correções e caderneta**

- [ ] Conferir e concluir as correções solicitadas nas entregas anteriores.
- [ ] Revisar a apresentação dos dados da caderneta, que ainda possui indicação de ajuste no código.
- [ ] Integrar a caderneta ao cadastro dos quatro jogadores, preservando os dados de quem já existe.
- [ ] Validar busca, inserção, balanceamento, listagem e liberação da caderneta com diferentes nomes e ordens de cadastro.
- [ ] Registrar os pedidos recebidos, conforme a divisão da equipe, para calcular a taxa de fuga.

**Persistência em disco — Pessoa 2**

- [ ] Implementar `include/disco.h`.
- [ ] Ler a caderneta do disco ao iniciar o programa e reconstruir a árvore.
- [ ] Salvar a caderneta e registrar o histórico de pontos ao fim de cada mão que atribuir pontos.
- [ ] Garantir fechamento dos arquivos após a gravação e recuperação dos dados já salvos ao reabrir o programa.
- [ ] Validar a persistência encerrando o programa entre mãos e iniciando uma nova execução.

**Mural do bar — Pessoa 3**

- [ ] Implementar `include/mural.h`.
- [ ] Montar uma árvore auxiliar ordenada por mãos ganhas, preservando jogadores com a mesma quantidade de vitórias em mãos.
- [ ] Exibir os jogadores em ordem crescente de mãos ganhas, considerando o acumulado de todas as partidas.
- [ ] Exibir o mural no início e no encerramento do jogo, conforme a divisão da equipe.
- [ ] Calcular e exibir a taxa de fuga de cada jogador, definindo o critério de contagem e tratando quem não recebeu pedidos.

**Atualização das estatísticas — Pessoa 4**

- [ ] Atualizar os contadores de pedidos e fugas durante a negociação em `APOSTA.h`.
- [ ] Atualizar as mãos ganhas dos dois jogadores da dupla vencedora ao fim de cada mão em `Partida.h`.
- [ ] Atualizar as partidas jogadas dos quatro participantes ao concluir a partida.
- [ ] Rastrear e exibir a mão mais cara da partida, considerando os valores de 1, 3, 6 ou 12 pontos.
- [ ] Integrar as atualizações com a caderneta e com o salvamento em disco.

**Organização, item criativo e relatório — Pessoa 5 (Felipe)**

- [ ] Organizar a `main.c`: carregar os dados, exibir mural/caderneta, consultar ou cadastrar jogadores, executar a partida, exibir os resultados finais e liberar a memória.
- [ ] Implementar `include/criativo.h` com sinais do parceiro e/ou marcação visual das manilhas, sem alterar as regras internas do jogo.
- [ ] Integrar o item criativo à interface da partida.
- [ ] Consolidar o relatório final em `RELATORIO.md`, com a participação de cada integrante e as correções realizadas.
- [ ] Validar o fluxo completo com os módulos da terceira unidade integrados.

### Divisão da terceira unidade

| Pessoa | Responsabilidade | Arquivos principais |
| --- | --- | --- |
| 1 — Eduardo | Estrutura e operações da caderneta | `include/caderneta.h` |
| 2 — Artur | Leitura e gravação em disco | `include/disco.h`, integração com `include/Partida.h` |
| 3 — Diego | Mural por mãos ganhas e mural da vergonha | `include/mural.h` |
| 4 | Atualização das estatísticas e mão mais cara | `include/APOSTA.h`, `include/Partida.h` |
| 5 — Felipe | Organização do fluxo, item criativo e relatório final | `main.c`, `include/criativo.h`, `RELATORIO.md` |

**Observações de integração:** a caderneta atual usa AVL, uma árvore binária de busca balanceada. O mural solicitado no enunciado é por **mãos ganhas**, e não por partidas ganhas. Na versão consultada, `main.c`, `Partida.h` e `APOSTA.h` ainda não utilizam a caderneta; `disco.h`, `mural.h`, `criativo.h` e `RELATORIO.md` ainda não estão presentes.
