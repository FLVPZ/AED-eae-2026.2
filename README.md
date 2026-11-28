# Truco no Bar do Pato

Projeto de Algoritmos e Estrutura de Dados do SENAI CIMATEC, semestre 2026.2. Jogo de Truco Paulista em C: quatro jogadores, duas duplas e meta de 12 pontos, com baralho de 40 cartas, vira, manilhas e pedidos de truco, seis e doze.

A interface de navegador utiliza o código C compilado para WebAssembly. A versão de terminal continua disponível.

## Como jogar no navegador

1. No [GitHub](https://github.com/FLVPZ/AED-eae-2026.2), selecione **main** e clique em **Code → Download ZIP**.
2. Extraia o ZIP baixado (**Extrair Tudo**, no Windows).
3. Na pasta extraída, abra **`web/dist/index.html`** no Chrome ou Edge.
4. Informe os quatro nomes e clique em **Abrir a mesa**. Confirme o jogador indicado para revelar a mão e clique na carta que deseja jogar.

**Não precisa instalar Dev-C++ nem compilar.** Após baixar, o jogo funciona offline. Abra o arquivo extraído no computador; a página do arquivo no GitHub mostra o código.

Jogadores 1 e 3 são parceiros; jogadores 2 e 4 formam a outra dupla. Use os botões de **truco**, **sinal**, **VAR**, **caderneta** e **ajuda** durante a partida. Clique em **Continuar** após os resultados e passe o dispositivo ao próximo jogador.

## Dados e estatísticas

- **Navegador:** os dados ficam na sessão. Recarregar a página ou iniciar **Novo jogo** reinicia a partida e a caderneta.
- **Terminal:** `caderneta.txt` guarda os contadores por jogador; `historico.txt` registra participantes, negociações, rodadas e resultados. Pedidos e respostas são salvos na hora; resultados, ao fim da mão.
- **Taxa de fuga:** `corridos / recebidos × 100`. Sem pedidos recebidos, a interface informa essa condição.

No Windows, os arquivos ficam junto do executável; no Linux/macOS, na pasta de execução. Nomes são padronizados antes do cadastro. Arquivos inválidos ou com nomes repetidos após a padronização são preservados para revisão.

<details>
<summary>Executar a versão de terminal</summary>

Requer GCC com suporte a C99. Abra o terminal na raiz do projeto e compile somente `main.c`, mantendo a pasta `include/`.

**Linux/macOS:**

```bash
gcc -std=c99 -Wall -Wextra -pedantic main.c -o truco
./truco
```

**Windows — PowerShell:**

```powershell
gcc -std=c99 -Wall -Wextra -pedantic main.c -o truco.exe
.\truco.exe
```

No CMD, execute `truco.exe`. No Dev-C++, compile `main.c` como C99.

| Comando | Ação |
| --- | --- |
| `1`, `2` ou `3` | Jogar uma carta disponível. |
| `T` | Pedir o próximo aumento permitido. |
| `A`, `C` ou `T` | Aceitar, correr ou aumentar durante a negociação. |
| `S` | Escolher um sinal fixo; `0` cancela. |
| `V` | Consultar o VAR sem passar a vez. |
| VAR: `1`, `2`, `3` | Voltar ao jogo, próxima rodada ou rodada anterior. |

Pressione Enter nas pausas de leitura. As manilhas aparecem destacadas, e a de Paus recebe a marca ZAP. Combinem o significado dos sinais antes de jogar.

</details>

<details>
<summary>Estrutura do projeto e recompilação da interface</summary>

| Arquivos | Responsabilidade |
| --- | --- |
| `main.c` | Inicialização e integração da versão de terminal. |
| `STACK.h`, `Embaralhar.h` | Pilha, criação e embaralhamento do baralho. |
| `fila.h`, `Jogador.h`, `Roda.h` | Cadastro, duplas e ordem dos jogadores. |
| `Mao.h`, `jogo.h`, `APOSTA.h`, `Partida.h` | Cartas, regras, apostas e condução da partida. |
| `caderneta.h`, `estatisticas.h`, `mural.h` | AVL, contadores e rankings. |
| `listade.h`, `var.h` | Histórico em lista duplamente encadeada e consulta ao VAR. |
| `criativo.h`, `interface.h` | Sinais, destaque das manilhas e apresentação no terminal. |
| `disco.h` | Leitura e gravação dos arquivos de dados. |
| `web/motor_web.c` | Adaptador C para WebAssembly. |
| `web/app.js`, `web/style.css`, `web/index_template.html` | Apresentação e interação no navegador. |
| `web/dist/` | Versão pronta para jogar. |

Os módulos `.h` ficam em `include/`. A interface web reutiliza as regras e estruturas do projeto.

Para recompilar a página após alterar os fontes, use Python 3 e [WASI SDK](https://github.com/WebAssembly/wasi-sdk/releases) ou [Zig](https://ziglang.org/). Execute na raiz:

```bash
WASI_SDK_PATH=/caminho/para/wasi-sdk ./web/build_web.sh
# ou:
ZIG=/caminho/para/zig ./web/build_web.sh
```

O script atualiza `web/dist/motor.wasm` e `web/dist/index.html` juntos. A versão atual foi compilada com Zig 0.17.1.

A caderneta mantém linhas `nome;partidas;maos;pedidos;corridos;recebidos` para recuperar a AVL, além dos comentários descritivos. A escrita usa um arquivo temporário antes de substituir a caderneta. Dados locais são ignorados pelo Git.

</details>

## Entregas

<details>
<summary>1ª unidade</summary>

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

</details>

<details>
<summary>2ª unidade</summary>

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

</details>

<details>
<summary>3ª unidade</summary>

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

</details>

