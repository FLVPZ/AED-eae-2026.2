// Bar do Pato | Truco - interface grafica
//
// Este arquivo NAO contem nenhuma regra do jogo. Todas as decisoes (quem
// pode jogar, quem venceu a rodada, taxa de fuga, manilha, etc.) vem do
// modulo WebAssembly compilado a partir do C existente (web/motor_web.c,
// que inclui os mesmos arquivos de include/ usados por main.c). Este
// arquivo so chama as funcoes exportadas pelo motor e desenha o
// resultado na tela.

(function () {
  "use strict";

  const NOME_MAX = 30;

  // ---------------------------------------------------------------------
  // Shim WASI minimo para as chamadas usadas pelo motor no navegador.
  // ---------------------------------------------------------------------
  function makeWasiShim(getMemory) {
    function mem() { return new DataView(getMemory().buffer); }
    function mem8() { return new Uint8Array(getMemory().buffer); }
    return {
      args_sizes_get(argcPtr, argvBufSizePtr) {
        mem().setUint32(argcPtr, 0, true);
        mem().setUint32(argvBufSizePtr, 0, true);
        return 0;
      },
      args_get() { return 0; },
      fd_close() { return 0; },
      fd_fdstat_get() { return 8; },
      fd_prestat_get() { return 8; },
      fd_prestat_dir_name() { return 8; },
      fd_read() { return 8; },
      fd_seek() { return 8; },
      fd_write(fd, iovsPtr, iovsLen, nwrittenPtr) {
        let total = 0;
        const view = mem();
        for (let i = 0; i < iovsLen; i++) {
          const base = iovsPtr + i * 8;
          const ptr = view.getUint32(base, true);
          const len = view.getUint32(base + 4, true);
          total += len;
          void mem8().slice(ptr, ptr + len); // descartado: a GUI nao usa stdout
        }
        view.setUint32(nwrittenPtr, total, true);
        return 0;
      },
      proc_exit(code) { throw new Error("O motor encerrou inesperadamente (codigo " + code + ")."); },
    };
  }

  let wasmExports = null;
  let wasmMemory = null;

  function lerCString(ptr, maxLen) {
    const bytes = new Uint8Array(wasmMemory.buffer);
    let end = ptr;
    const limite = maxLen ? ptr + maxLen : bytes.length;
    while (end < limite && bytes[end] !== 0) end++;
    return new TextDecoder("utf-8").decode(bytes.slice(ptr, end));
  }

  function escreverNomeNoScratch(indice, nome) {
    const base = wasmExports.motor_scratch_ptr();
    const bytes = new Uint8Array(wasmMemory.buffer);
    const slotBase = base + indice * NOME_MAX;
    bytes.fill(0, slotBase, slotBase + NOME_MAX);
    const codificado = new TextEncoder().encode(nome);
    bytes.set(codificado, slotBase);
  }

  function obterEstado() {
    const n = wasmExports.motor_estado_json();
    return JSON.parse(lerCString(wasmExports.motor_estado_ptr(), n));
  }
  function obterVar() {
    const n = wasmExports.motor_var_json();
    return JSON.parse(lerCString(wasmExports.motor_var_ptr(), n));
  }
  function obterStats() {
    const n = wasmExports.motor_estatisticas_json();
    return JSON.parse(lerCString(wasmExports.motor_estatisticas_ptr(), n));
  }

  // ---------------------------------------------------------------------
  // Carregamento do modulo (o .wasm vem embutido em base64 no proprio
  // HTML: a pagina funciona sem servidor e sem internet).
  // ---------------------------------------------------------------------
  async function carregarMotor() {
    const base64 = document.getElementById("motor-wasm-base64").textContent.trim();
    const binario = Uint8Array.from(atob(base64), (c) => c.charCodeAt(0));
    const memRef = { buffer: null };
    const importObj = { wasi_snapshot_preview1: makeWasiShim(() => wasmMemory) };
    const { instance } = await WebAssembly.instantiate(binario.buffer, importObj);
    wasmExports = instance.exports;
    wasmMemory = instance.exports.memory;
    if (typeof wasmExports._initialize === "function") wasmExports._initialize();
    void memRef;
  }

  // ---------------------------------------------------------------------
  // Utilitarios de carta / UI
  // ---------------------------------------------------------------------
  const SIMBOLO_NAIPE = { ouros: "♦", espadas: "♠", copas: "♥", paus: "♣" };
  const COR_NAIPE = { ouros: "vermelha", espadas: "preta", copas: "vermelha", paus: "preta" };

  function elementoCarta(carta, { pequena = false, dorso = false, animar = false } = {}) {
    const div = document.createElement("div");
    div.className = "carta" + (pequena ? " pequena" : "");
    if (dorso || !carta) {
      div.classList.add("dorso");
      return div;
    }
    div.classList.add(COR_NAIPE[carta.naipe]);
    if (carta.manilha) div.classList.add("manilha");
    if (carta.zap) div.classList.add("zap");
    if (animar) div.classList.add("carta-entra");
    const simbolo = SIMBOLO_NAIPE[carta.naipe] || "?";
    div.innerHTML =
      `<div class="canto superior">${carta.rotulo}<br>${simbolo}</div>` +
      `<div class="naipe-centro">${simbolo}</div>` +
      `<div class="canto inferior">${carta.rotulo}<br>${simbolo}</div>`;
    return div;
  }

  function nomeDaDupla(estado, dupla) {
    return estado.jogadores.filter((j) => j.dupla === dupla).map((j) => j.nome).join(" + ");
  }

  // ---------------------------------------------------------------------
  // Referencias de DOM
  // ---------------------------------------------------------------------
  const telaCadastro = document.getElementById("tela-cadastro");
  const telaJogo = document.getElementById("tela-jogo");
  const telaFim = document.getElementById("tela-fim");
  const formCadastro = document.getElementById("form-cadastro");
  const erroCadastro = document.getElementById("erro-cadastro");

  const elPlacar1 = document.getElementById("placar-d1");
  const elPlacar2 = document.getElementById("placar-d2");
  const elMaoRodada = document.getElementById("info-mao-rodada");
  const elValorMao = document.getElementById("info-valor-mao");
  const elVira = document.getElementById("vira-carta");
  const elCartasMesa = document.getElementById("cartas-na-mesa");
  const elAviso = document.getElementById("aviso-mesa");
  const elPainelVez = document.getElementById("painel-vez");
  const elQuemJoga = document.getElementById("quem-joga");
  const elLequeMao = document.getElementById("leque-mao");
  const assentosEl = [0, 1, 2, 3].map((i) => document.getElementById("assento-" + i));

  const botaoTruco = document.getElementById("botao-truco");
  const botaoSinal = document.getElementById("botao-sinal");
  const popoverSinal = document.getElementById("popover-sinal");
  const botaoVar = document.getElementById("botao-var");
  const botaoStats = document.getElementById("botao-stats");
  const botaoAjuda = document.getElementById("botao-ajuda");
  const botaoNovoJogo = document.getElementById("botao-novo-jogo-topo");
  const barraAcoes = document.getElementById("barra-acoes-wrapper");

  const overlayPronto = document.getElementById("overlay-pronto");
  const overlayProntoNome = document.getElementById("overlay-pronto-nome");
  const overlayProntoTexto = document.getElementById("overlay-pronto-texto");
  const botaoConfirmarPronto = document.getElementById("botao-confirmar-pronto");

  const overlayNegociacao = document.getElementById("overlay-negociacao");
  const negociacaoTexto = document.getElementById("negociacao-texto");
  const botaoAceitar = document.getElementById("botao-aceitar");
  const botaoCorrer = document.getElementById("botao-correr");
  const botaoAumentar = document.getElementById("botao-aumentar");

  const overlayResultado = document.getElementById("overlay-resultado");
  const resultadoTitulo = document.getElementById("resultado-titulo");
  const resultadoTexto = document.getElementById("resultado-texto");
  const botaoContinuar = document.getElementById("botao-continuar");

  const modalVar = document.getElementById("modal-var");
  const varConteudo = document.getElementById("var-conteudo");
  const modalStats = document.getElementById("modal-stats");
  const statsConteudo = document.getElementById("stats-conteudo");
  const modalAjuda = document.getElementById("modal-ajuda");

  const elTrofeuTexto = document.getElementById("fim-vencedor");
  const elFimPlacar = document.getElementById("fim-placar");
  const elFimMaoCara = document.getElementById("fim-mao-cara");
  const botaoJogarNovamente = document.getElementById("botao-jogar-novamente");

  let ultimaQtdMesa = -1;
  let ultimaMao = -1;

  function fecharTodosOsOverlaysSecundarios() {
    modalVar.classList.add("escondido");
    modalStats.classList.add("escondido");
    modalAjuda.classList.add("escondido");
    popoverSinal.classList.add("escondido");
  }

  // ---------------------------------------------------------------------
  // Renderizacao principal: le o estado do motor e desenha tudo.
  // ---------------------------------------------------------------------
  function renderizar() {
    const estado = obterEstado();

    elPlacar1.textContent = estado.placar1;
    elPlacar2.textContent = estado.placar2;
    elMaoRodada.textContent = `MÃO ${estado.mao} · RODADA ${estado.rodada}/3`;
    elValorMao.textContent =
      estado.valor_mao === 1 ? "VALE 1 PONTO" : `VALE ${estado.valor_mao} PONTOS!`;
    elAviso.textContent = estado.aviso || "";

    elVira.innerHTML = "";
    elVira.appendChild(elementoCarta(estado.vira, { pequena: true }));

    // assentos ao redor da mesa
    estado.jogadores.forEach((j, i) => {
      const el = assentosEl[i];
      el.classList.toggle("vez", !!j.vez);
      const dorsos = Array.from({ length: j.cartas_na_mao })
        .map(() => '<span class="mini-dorso"></span>')
        .join("");
      el.querySelector(".cartao-assento").innerHTML =
        `<div class="nome">${escaparHtml(j.nome)}</div>` +
        `<span class="dupla-tag d${j.dupla}">Dupla ${j.dupla}</span>` +
        `<div class="mini-cartas-dorso">${dorsos}</div>`;
    });

    // cartas jogadas na mesa nesta rodada
    const novaRodada = estado.mesa.length < ultimaQtdMesa || estado.mao !== ultimaMao;
    if (novaRodada) elCartasMesa.innerHTML = buildSlots();
    estado.mesa.forEach((entrada) => {
      const slot = elCartasMesa.querySelector(`[data-pos="${entrada.assento}"]`);
      if (slot && !slot.dataset.preenchido) {
        slot.innerHTML = "";
        slot.appendChild(elementoCarta(entrada.carta, { animar: true }));
        slot.dataset.preenchido = "1";
      }
    });
    ultimaQtdMesa = estado.mesa.length;
    ultimaMao = estado.mao;

    // painel do jogador da vez (visivel apenas quando a mao foi revelada)
    const jogadorVez = estado.jogadores.find((j) => j.vez);
    if (estado.fase === "jogando" && estado.visivel && jogadorVez && jogadorVez.mao) {
      elPainelVez.classList.remove("escondido");
      elQuemJoga.textContent = `Sua vez, ${jogadorVez.nome} (Dupla ${jogadorVez.dupla})`;
      elLequeMao.innerHTML = "";
      jogadorVez.mao.forEach((carta, idx) => {
        const el = elementoCarta(carta);
        el.classList.add("carta-jogavel");
        el.title = "Jogar " + carta.rotulo + " de " + carta.naipe;
        el.addEventListener("click", () => jogarCarta(el, idx + 1));
        elLequeMao.appendChild(el);
      });
    } else {
      elPainelVez.classList.add("escondido");
    }

    // botoes de acao
    botaoTruco.disabled = !estado.acoes.pode_pedir;
    botaoTruco.textContent = estado.acoes.pode_pedir
      ? `Pedir ${nomePedido(estado.acoes.pedido_possivel)}`
      : "Pedir aumento";
    botaoSinal.disabled = !estado.acoes.pode_sinal;

    // overlays conforme a fase
    overlayPronto.classList.toggle("escondido", !estado.acoes.precisa_confirmar);
    if (estado.acoes.precisa_confirmar) {
      const alvo =
        estado.fase === "negociacao"
          ? estado.negociacao.respondente
          : (jogadorVez ? jogadorVez.nome : "");
      overlayProntoNome.textContent = alvo;
      overlayProntoTexto.textContent =
        estado.fase === "negociacao"
          ? `${estado.negociacao.pediu} pediu ${estado.negociacao.nome_pedido}. Confira se é mesmo ${alvo} antes de tocar.`
          : "Confira se o dispositivo está com a pessoa certa antes de revelar as cartas.";
    }

    overlayNegociacao.classList.toggle(
      "escondido",
      !(estado.fase === "negociacao" && estado.visivel)
    );
    if (estado.fase === "negociacao" && estado.visivel) {
      const neg = estado.negociacao;
      negociacaoTexto.innerHTML =
        `<b>${escaparHtml(neg.pediu)}</b> (Dupla ${neg.dupla_pediu}) pediu <b>${neg.nome_pedido}</b>.<br>` +
        `${escaparHtml(neg.respondente)} (Dupla ${neg.respondente_dupla}), a mão passaria a valer <b>${neg.valor_pedido}</b> ponto(s).`;
      botaoAumentar.classList.toggle("escondido", !neg.pode_aumentar);
      if (neg.pode_aumentar) botaoAumentar.textContent = `Pedir ${neg.proximo_valor}`;
    }

    overlayResultado.classList.toggle(
      "escondido",
      !(estado.fase === "rodada_concluida" || estado.fase === "mao_concluida")
    );
    if (estado.fase === "rodada_concluida") {
      resultadoTitulo.textContent = "Fim da rodada";
      resultadoTexto.textContent =
        estado.resultado_rodada.dupla === 0
          ? "Rodada empatada."
          : `Dupla ${estado.resultado_rodada.dupla} venceu a rodada (${nomeDaDupla(estado, estado.resultado_rodada.dupla)}).`;
    } else if (estado.fase === "mao_concluida") {
      resultadoTitulo.textContent = "Fim da mão";
      resultadoTexto.textContent =
        `Dupla ${estado.resultado_mao.dupla} (${nomeDaDupla(estado, estado.resultado_mao.dupla)}) ` +
        `fez ${estado.resultado_mao.pontos} ponto(s). Placar: ${estado.placar1} x ${estado.placar2}.`;
    }

    if (estado.fase === "fim_partida") {
      telaJogo.classList.add("escondido");
      barraAcoes.classList.add("escondido");
      telaFim.classList.remove("escondido");
      fecharTodosOsOverlaysSecundarios();
      const vencedora = estado.fim.dupla_vencedora;
      elTrofeuTexto.textContent = `Dupla ${vencedora} venceu a partida! (${nomeDaDupla(estado, vencedora)})`;
      elFimPlacar.textContent = `Placar final: ${estado.fim.placar1_final} x ${estado.fim.placar2_final}`;
      elFimMaoCara.textContent =
        estado.fim.maior_mao > 0
          ? `Mão mais cara da partida: ${estado.fim.maior_mao} ponto(s), na mão ${estado.fim.numero_maior_mao}.`
          : "";
    }

    return estado;
  }

  function buildSlots() {
    return [0, 1, 2, 3].map((p) => `<div class="carta-mesa-pos" data-pos="${p}"></div>`).join("");
  }

  function escaparHtml(txt) {
    const d = document.createElement("div");
    d.textContent = txt;
    return d.innerHTML;
  }

  const NOMES_PEDIDO = { 3: "TRUCO", 6: "SEIS", 12: "DOZE" };
  function nomePedido(v) { return NOMES_PEDIDO[v] || ""; }

  // ---------------------------------------------------------------------
  // Acoes do jogador (cada uma chama o motor e depois re-renderiza)
  // ---------------------------------------------------------------------
  function jogarCarta(elCarta, indice) {
    elCarta.classList.add("carta-saindo");
    elCarta.style.pointerEvents = "none";
    setTimeout(() => {
      if (!wasmExports.motor_jogar_carta(indice)) {
        alert("Essa jogada não é válida agora.");
      }
      renderizar();
    }, 160);
  }

  botaoConfirmarPronto.addEventListener("click", () => {
    wasmExports.motor_confirmar_pronto();
    renderizar();
  });

  botaoTruco.addEventListener("click", () => {
    if (wasmExports.motor_pedir_aumento()) renderizar();
  });

  botaoAceitar.addEventListener("click", () => { wasmExports.motor_responder(0); renderizar(); });
  botaoCorrer.addEventListener("click", () => { wasmExports.motor_responder(1); renderizar(); });
  botaoAumentar.addEventListener("click", () => { wasmExports.motor_responder(2); renderizar(); });

  botaoSinal.addEventListener("click", () => {
    popoverSinal.classList.toggle("escondido");
  });
  popoverSinal.querySelectorAll("[data-gesto]").forEach((btn) => {
    btn.addEventListener("click", () => {
      const gesto = parseInt(btn.dataset.gesto, 10);
      wasmExports.motor_enviar_sinal(gesto);
      popoverSinal.classList.add("escondido");
      renderizar();
    });
  });

  botaoContinuar.addEventListener("click", () => {
    wasmExports.motor_continuar();
    renderizar();
  });

  botaoVar.addEventListener("click", () => {
    abrirVar();
  });
  let varIndiceAtual = 0;
  function abrirVar() {
    const lista = obterVar();
    varIndiceAtual = lista.length - 1;
    desenharVar(lista);
    modalVar.classList.remove("escondido");
  }
  function desenharVar(lista) {
    if (lista.length === 0) {
      varConteudo.innerHTML = "<p>O VAR ainda não tem nenhuma rodada registrada.</p>";
      return;
    }
    const r = lista[varIndiceAtual];
    let html = `<div class="linha-var-rodada ${r.em_andamento ? "andamento" : ""}">`;
    html += `<div class="titulo-var">Mão ${r.mao} · Rodada ${r.rodada}${r.em_andamento ? " (em andamento)" : ""}</div>`;
    r.jogadores.forEach((j) => {
      html += `<div class="jogada-var"><span>${escaparHtml(j.nome)}</span><span>${
        j.carta ? j.carta.rotulo + " de " + j.carta.naipe + (j.carta.manilha ? (j.carta.zap ? " (ZAP)" : " (manilha)") : "") : "ainda não jogou"
      }</span></div>`;
    });
    html += `<div class="negociacao-var">Manilha: ${r.manilha} · valor em jogo: ${r.pontos_em_jogo} ponto(s)</div>`;
    r.negociacoes.forEach((n) => {
      const verbo = n.resposta === "A" ? "aceitou" : n.resposta === "C" ? "correu" : "aumentou";
      html += `<div class="negociacao-var">${escaparHtml(n.pediu)} pediu ${n.valor}; ${escaparHtml(n.respondeu)} ${verbo}.</div>`;
    });
    if (!r.em_andamento) {
      html += `<div class="negociacao-var">${
        r.encerrada_por_fuga
          ? `Mão encerrada por fuga: Dupla ${r.dupla_vencedora} recebeu os pontos.`
          : r.dupla_vencedora === 0
          ? "Rodada empatada."
          : `Dupla vencedora da rodada: ${r.dupla_vencedora}.`
      }</div>`;
    }
    html += `</div>`;
    varConteudo.innerHTML = html;
    document.getElementById("var-posicao").textContent = `Rodada ${varIndiceAtual + 1} de ${lista.length}`;
  }
  document.getElementById("var-anterior").addEventListener("click", () => {
    const lista = obterVar();
    if (varIndiceAtual > 0) varIndiceAtual--;
    desenharVar(lista);
  });
  document.getElementById("var-proxima").addEventListener("click", () => {
    const lista = obterVar();
    if (varIndiceAtual < lista.length - 1) varIndiceAtual++;
    desenharVar(lista);
  });
  document.getElementById("var-fechar").addEventListener("click", () => modalVar.classList.add("escondido"));

  botaoStats.addEventListener("click", () => {
    const stats = obterStats();
    let html = `<table class="tabela-stats"><thead><tr>
      <th>Jogador</th><th>Partidas</th><th>Mãos</th><th>Pedidos</th><th>Recebidos</th><th>Fugas</th><th>Taxa de fuga</th>
      </tr></thead><tbody>`;
    stats.forEach((p) => {
      html += `<tr><td>${escaparHtml(p.nome)}</td><td>${p.partidas}</td><td>${p.maos_ganhas}</td>` +
        `<td>${p.pedidos}</td><td>${p.recebidos}</td><td>${p.corridos}</td>` +
        `<td>${p.tem_pedidos ? p.taxa_fuga.toFixed(1) + "%" : "sem pedidos recebidos"}</td></tr>`;
    });
    html += "</tbody></table>";
    statsConteudo.innerHTML = html;
    modalStats.classList.remove("escondido");
  });
  document.getElementById("stats-fechar").addEventListener("click", () => modalStats.classList.add("escondido"));

  botaoAjuda.addEventListener("click", () => modalAjuda.classList.remove("escondido"));
  document.getElementById("ajuda-fechar").addEventListener("click", () => modalAjuda.classList.add("escondido"));

  function irParaCadastro() {
    telaFim.classList.add("escondido");
    telaJogo.classList.add("escondido");
    barraAcoes.classList.add("escondido");
    fecharTodosOsOverlaysSecundarios();
    telaCadastro.classList.remove("escondido");
    formCadastro.reset();
    erroCadastro.textContent = "";
  }
  botaoNovoJogo.addEventListener("click", () => {
    if (confirm("Encerrar a partida atual e voltar ao cadastro?")) {
      wasmExports.motor_reiniciar();
      irParaCadastro();
    }
  });
  botaoJogarNovamente.addEventListener("click", () => {
    wasmExports.motor_reiniciar();
    irParaCadastro();
  });

  // ---------------------------------------------------------------------
  // Cadastro dos quatro jogadores
  // ---------------------------------------------------------------------
  formCadastro.addEventListener("submit", (ev) => {
    ev.preventDefault();
    const nomes = [0, 1, 2, 3].map((i) => document.getElementById("nome-jogador-" + i).value.trim());
    if (nomes.some((nome) => new TextEncoder().encode(nome).length >= NOME_MAX)) {
      erroCadastro.textContent = "Um dos nomes ficou muito longo. Use um nome mais curto.";
      return;
    }
    for (let i = 0; i < 4; i++) escreverNomeNoScratch(i, nomes[i]);
    wasmExports.motor_definir_semente(Date.now() & 0x7fffffff);
    const ok = wasmExports.motor_iniciar();
    if (!ok) {
      const estado = obterEstado();
      erroCadastro.textContent = estado.aviso || "Não foi possível iniciar a partida.";
      return;
    }
    telaCadastro.classList.add("escondido");
    telaFim.classList.add("escondido");
    telaJogo.classList.remove("escondido");
    barraAcoes.classList.remove("escondido");
    ultimaQtdMesa = -1;
    ultimaMao = -1;
    elCartasMesa.innerHTML = buildSlots();
    renderizar();
  });

  // ---------------------------------------------------------------------
  // Boot
  // ---------------------------------------------------------------------
  carregarMotor()
    .then(() => {
      document.getElementById("carregando").classList.add("escondido");
      telaCadastro.classList.remove("escondido");
    })
    .catch((erro) => {
      document.getElementById("carregando").textContent =
        "Não foi possível carregar o motor do jogo: " + erro.message;
      console.error(erro);
    });
})();
