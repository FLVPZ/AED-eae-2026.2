"""Verifica carga na main, gravacao por mao e recuperacao em outra execucao."""
from pathlib import Path
import os
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FLAGS = ["gcc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-pedantic"]
NOMES = "Ana\nBia\nCaio\nDavi\n"


def executar(exe, pasta, entrada=NOMES + "1\n" * 400, sucesso=True):
    r = subprocess.run([str(exe)], cwd=pasta, input=entrada, text=True,
                       capture_output=True, timeout=15)
    assert (r.returncode == 0) == sucesso, (r.stdout, r.stderr)
    return r


def main():
    with tempfile.TemporaryDirectory() as pasta:
        pasta = Path(pasta)
        exe = pasta / "truco"
        unitario = pasta / "teste_disco"
        subprocess.run(FLAGS + [str(ROOT / "main.c"), "-o", str(exe)], check=True)
        subprocess.run(FLAGS + [str(ROOT / "tests/test_disco.c"), "-o", str(unitario)], check=True)
        unit = pasta / "unit"; unit.mkdir()
        subprocess.run([str(unitario)], cwd=unit, check=True)

        # A main recupera todos os jogadores, inclusive quem nao esta na mesa.
        arquivo = pasta / "caderneta.txt"
        original = "Ana;3;5;7;1;4\nVisitante;1;2;1;0;1\n"
        arquivo.write_text(original)
        r = executar(exe, pasta)
        assert "Caderneta recuperada: 2 jogador(es)." in r.stdout
        assert "25.00%" in r.stdout
        assert "FIM DE PARTIDA!" in r.stdout
        assert not r.stderr, r.stderr
        linhas = {l.split(";")[0]: l.split(";")[1:] for l in arquivo.read_text().splitlines()}
        assert len(linhas) == 5
        assert linhas["Ana"][0] == "4"
        assert int(linhas["Ana"][1]) >= 5
        assert linhas["Ana"][2:] == ["7", "1", "4"]
        assert linhas["Visitante"] == ["1", "2", "1", "0", "1"]
        hist = pasta / "historico.txt"
        registros = [l for l in hist.read_text().splitlines() if l]
        maos = re.findall(r"Dupla [12] leva a mao e faz (\d+) ponto", r.stdout)
        assert len(registros) == len(maos) and len(maos) > 0
        assert [int(l.split(";")[0]) for l in registros] == list(range(1, len(maos) + 1))
        r = executar(exe, pasta)
        assert "Caderneta recuperada: 5 jogador(es)." in r.stdout
        assert "25.00%" in r.stdout
        assert "Visitante;1;2;1;0;1" in arquivo.read_text()

        # Uma chamada de jogar_mao precisa gravar antes de encerrar a partida.
        driver = pasta / "uma_mao.c"
        driver.write_text('#define main main_original\n#include "' + str(ROOT / "main.c")
            + '"\n#undef main\nint main(void) {\n'
            'ArvAVL *cad=criarAVL(); tp_listad *v=inicializa_listad(); tp_fila mesa;\n'
            'char nomes[4][NOME_MAX]={"Ana","Bia","Caio","Davi"}; int placar[3]={0};\n'
            'inicializaFila(&mesa); montar_mesa(&mesa,nomes);\n'
            'for(int i=0;i<4;i++) acessar_jogador(cad,nomes[i]);\n'
            'acessar_jogador(cad,"Ana")->info.trucos_recebidos=6;\n'
            'srand(7); jogar_mao(&mesa,placar,1,v,cad);\n'
            'Destroi_listad(v); liberaABB(cad); return 0; }\n')
        uma_mao = pasta / "uma_mao"
        subprocess.run(FLAGS + [str(driver), "-o", str(uma_mao)], check=True)
        parcial = pasta / "parcial"; parcial.mkdir()
        r = executar(uma_mao, parcial, "1\n" * 30)
        assert "FIM DE PARTIDA!" not in r.stdout
        linha_ana = next(l for l in (parcial / "caderneta.txt").read_text().splitlines() if l.startswith("Ana;"))
        assert linha_ana.split(";")[-1] == "6"
        assert len([l for l in (parcial / "historico.txt").read_text().splitlines() if l]) == 1

        # Falha de leitura nao substitui um arquivo antigo/invalido por zeros.
        antigo = "Ana;3;5;7;1\n"
        arquivo.write_text(antigo)
        r = executar(exe, pasta, "", sucesso=False)
        assert "O arquivo foi preservado" in r.stderr
        assert arquivo.read_text() == antigo
        arquivo.unlink()

        # Nomes nao podem inserir separadores no formato salvo.
        r = executar(exe, pasta, "Ana;Silva\n" + NOMES + "1\n" * 400)
        assert "ponto e virgula" in r.stdout
        assert "Ana;Silva" not in arquivo.read_text()

        # A gravacao falha com clareza, sem interromper as regras da partida.
        (pasta / "caderneta.txt.tmp").mkdir()
        r = executar(exe, pasta)
        assert "FIM DE PARTIDA!" in r.stdout
        assert "Falha ao salvar a caderneta" in r.stderr
        assert "Nao foi possivel salvar a caderneta no encerramento" in r.stderr
        assert not (pasta / "caderneta.txt.tmp").is_file()
    print("OK: main, carga/reabertura, preservacao de fregueses, gravacao por mao e falhas de I/O.")


if __name__ == "__main__":
    main()
