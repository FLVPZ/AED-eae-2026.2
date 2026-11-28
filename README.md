# Truco no Bar do Pato

Projeto de Algoritmos e Estrutura de Dados do curso de Engenharia da Computação do SENAI CIMATEC, semestre 2026.2.

Jogo em C baseado nas regras de Truco Paulista definidas no enunciado: quatro jogadores, duas duplas alternadas na mesa e partidas até uma dupla atingir 12 pontos. Usa baralho de 40 cartas, vira, manilhas e pedidos de truco, seis e doze. O jogo é executado no terminal por `main.c`, com placar, cartas e comandos visíveis durante a jogada. Há também uma interface gráfica de navegador, em `web/`, construída sobre a mesma lógica em C compilada para WebAssembly (veja "Interface gráfica (navegador)" abaixo).

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

## Como jogar no terminal

Informe os quatro nomes na ordem da mesa: dupla 1, dupla 2, dupla 1, dupla 2. Assim, o primeiro e o terceiro jogadores são parceiros, assim como o segundo e o quarto. Os nomes não podem ser vazios, repetidos na mesma mesa, conter ponto e vírgula ou caracteres de controle. O limite é de 29 bytes; letras acentuadas podem ocupar mais de um byte, dependendo da codificação do terminal.

A caderneta aparece em ordem alfabética, seguida dos murais. Em terminal interativo, pressione Enter quando solicitado para abrir a mesa.

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

O comando `V` inclui a rodada em andamento desde a primeira jogada, com o autor de cada carta. A tela e limpa ao navegar no VAR e negociar truco; o console antigo do Windows tem uma alternativa nativa quando nao aceita ANSI. Ao terminar uma rodada ou mao, pressione Enter para ler o resultado antes de continuar.

No encerramento, o programa anuncia a dupla vencedora e o placar. Após a pausa de leitura, abre o VAR; ao sair dele, mostra os murais finais.


## Caderneta e arquivos de dados

No Windows, `caderneta.txt` e `historico.txt` ficam na mesma pasta do executável. No Linux/macOS, ficam na pasta de onde o programa foi executado. O terminal informa essa pasta ao iniciar a partida.

Antes do cadastro, o programa procura a caderneta nessa pasta. Se ela nao existir, comeca vazia. Jogadores encontrados pelo mesmo nome mantem seus contadores; novos jogadores entram zerados. A comparacao diferencia maiusculas e minusculas.

Ao iniciar a partida com os quatro nomes, o programa cria ou atualiza **os dois arquivos**. A caderneta e salva a cada pedido/resposta de truco, ao fim de cada mao e no encerramento. O historico recebe o inicio e as duplas, pedidos e respostas, as rodadas com os nomes e cartas de cada jogador, os pontos de cada mao e o resultado final. Sair antes do fim nao registra uma partida concluida, mas preserva as acoes ja gravadas.

A caderneta tem um bloco descritivo por jogador, com partidas, maos ganhas, trucos pedidos, recebidos, fugas e taxa de fuga. As linhas com os seis campos `nome;partidas;maos;pedidos;corridos;recebidos` continuam presentes para recuperar a AVL; os comentarios explicativos comecam com `#`. Arquivos anteriores com os seis campos continuam sendo lidos. Nao e necessario editar os arquivos manualmente.

A gravacao usa `caderneta.txt.tmp` e substitui a caderneta somente apos concluir a escrita. Um arquivo invalido ou antigo, sem pedidos recebidos, e preservado e impede o inicio do jogo ate que o problema seja resolvido. Falhas de gravacao informam o caminho do arquivo e o motivo; os dados da caderneta continuam na memoria. Se houver falha, confira a permissao de escrita e se outro programa bloqueou o arquivo. A caderneta, o historico e o temporario sao dados locais ignorados pelo Git.

As estatísticas são acumuladas por nome: pedidos feitos, pedidos recebidos, fugas e mãos ganhas são atualizados durante o jogo; uma partida jogada é registrada para os quatro jogadores ao concluir a partida. Cada pedido de truco, seis ou doze conta uma vez para quem efetivamente responde, mesmo que digite opções inválidas antes de decidir. A taxa de fuga é `corridos / recebidos × 100`; sem pedidos recebidos, o mural informa essa condição. Cada mão ganha conta para os dois integrantes da dupla vencedora.

No encerramento, o programa mostra a mão de maior valor efetivamente pontuado (1, 3, 6 ou 12); em empate, mantém a primeira. O VAR identifica quem jogou cada carta e registra toda a negociação realizada em cada rodada, incluindo aumentos, aceite e fuga.

## Interface gráfica (navegador)

Além da versão de terminal, a pasta `web/` traz uma interface gráfica no tema
"Bar do Pato": mesa de feltro, cartas grandes com manilhas e ZAP em destaque,
placar, vira e valor da mão, pedidos de truco/seis/doze, sinais fixos entre
parceiros e consulta ao VAR e à caderneta sem perder a vez. A mão de quem
está jogando fica escondida sempre que o controle passa para outro jogador,
até a pessoa confirmar que está pronta — pensado para "passar o celular/PC"
em volta da mesa.

**A interface gráfica não reimplementa as regras.** O arquivo
`web/motor_web.c` inclui e chama diretamente os mesmos arquivos de
`include/` usados por `main.c` (a pilha do baralho, a fila dos jogadores, a
negociação de aposta, a AVL da caderneta e a lista do VAR); ele só expõe
essas funções para a página em vez de um laço de terminal. Esse arquivo é
compilado para WebAssembly e roda inteiramente no navegador: não é preciso
instalar nada, nem ter permissão de administrador, nem ter compilador C
instalado para **jogar**.

### Como jogar (sem compilar nada)

Abra `web/dist/index.html` com dois cliques em qualquer navegador atual
(Chrome, Edge, Firefox) — funciona offline, sem servidor e sem internet,
inclusive no Windows. O arquivo já traz o motor compilado embutido.

### Como recompilar a interface gráfica (opcional)

Só é necessário se você alterar `web/motor_web.c` ou algum arquivo de
`include/`. Requer o [WASI SDK](https://github.com/WebAssembly/wasi-sdk/releases)
(clang com suporte a `wasm32-wasi`) e Python 3 para montar o arquivo final.

```bash
# baixe e extraia o wasi-sdk (ex.: wasi-sdk-24.0-x86_64-linux.tar.gz)
cd web
WASI_SDK_PATH=/caminho/para/wasi-sdk ./build_web.sh
# gera web/dist/index.html e web/dist/motor.wasm
```

A persistência em disco (`caderneta.txt`/`historico.txt`, `include/disco.h`)
continua sendo exclusiva da versão de terminal: um módulo WebAssembly rodando
no navegador não tem acesso ao sistema de arquivos do jogador. Na interface
gráfica, a caderneta (mesma árvore AVL, mesmas regras de taxa de fuga) é
calculada em memória durante a sessão do navegador e reinicia a cada "Novo
jogo"; a versão de terminal continua salvando tudo em arquivo, como antes.

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

A pasta `web/` contém a interface gráfica (veja a seção acima):
`motor_web.c` (adaptador que chama a lógica de `include/` e expõe o estado
do jogo em JSON), `index_template.html`, `style.css` e `app.js` (fonte da
página) e `build_web.sh` (script que compila para WebAssembly e monta o
arquivo final `web/dist/index.html`).

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
