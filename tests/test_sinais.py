"""Testa personalizacao e persistencia em processos separados, sem dados reais."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
NOMES = "Ana\nBia\nCaio\nDavi\n"


def rodar(exe, pasta, comandos):
    r = subprocess.run([str(exe)], cwd=pasta, input=NOMES + comandos,
                       text=True, capture_output=True, timeout=15)
    assert r.returncode == 0, r.stderr
    assert "FIM DE PARTIDA!" in r.stdout
    return r.stdout


def main():
    with tempfile.TemporaryDirectory() as pasta:
        pasta = Path(pasta)
        exe = pasta / "truco"
        teste = pasta / "teste_arquivo"
        flags = ["gcc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-pedantic"]
        subprocess.run(flags + [str(ROOT / "main.c"), "-o", str(exe)], check=True)
        subprocess.run(flags + [str(ROOT / "tests/test_sinais_arquivo.c"),
                                "-o", str(teste)], check=True)
        subprocess.run([str(teste), str(pasta / "teste.txt")], check=True)

        # Editar e depois cancelar nao grava nada.
        saida = rodar(exe, pasta, "S\nP\nrascunho\n0\n1\n" + "1\n" * 400)
        assert "[SINAL] Ana pisca ;) para Caio." in saida
        arquivo = pasta / "sinais.txt"
        assert not arquivo.exists()

        # Tres gestos personalizados; sinal nao passa a vez nem joga carta.
        saida = rodar(exe, pasta, "S\nP\nlevanta o copo\nbate palma\ncoça o nariz\n1\n"
                      + "1\n" * 400)
        assert "Sinais salvos" in saida
        assert "[SINAL] Ana levanta o copo para Caio." in saida
        depois = saida.split("[SINAL] Ana levanta o copo para Caio.", 1)[1]
        assert depois.index("Vez de Ana") < depois.index("Vez de Bia")
        assert arquivo.exists()

        # Outro processo recupera o mesmo nome; Enter preserva os outros sinais.
        saida = rodar(exe, pasta, "S\nP\nnovo gesto\n\n\n2\n" + "1\n" * 400)
        assert "Sinais de Ana recuperados." in saida
        assert "[SINAL] Ana bate palma para Caio." in saida
        saida = rodar(exe, pasta, "S\n1\n" + "1\n" * 400)
        assert "[SINAL] Ana novo gesto para Caio." in saida

        # Nome de outro jogador nao deve herdar os sinais de Ana.
        assert "Sinais de Bia recuperados." not in saida
        assert "Sinais de Caio recuperados." not in saida

        # Personalizacao fica na fila mesmo se a mao acabar antes de jogar carta.
        driver = pasta / "teste_fila.c"
        driver.write_text(
            '#define main main_do_jogo\n#include "' + str(ROOT / "main.c") + '"\n'
            '#undef main\n#include <assert.h>\nint main(void) {\n'
            'tp_fila mesa; char nomes[4][NOME_MAX] = {"Ana","Bia","Caio","Davi"};\n'
            'inicializaFila(&mesa); montar_mesa(&mesa, nomes); criativo_carregar_sinais(&mesa);\n'
            'tp_jogador j; posicao_do_jogador(mesa,0,&j);\n'
            'criativo_enviar_sinal(mesa,&j); atualizaPrimeiroFila(&mesa,j);\n'
            'tp_jogador lido; posicao_do_jogador(mesa,0,&lido);\n'
            'assert(strcmp(lido.nome,"Ana")==0);\n'
            'assert(strcmp(lido.sinais[0],"teste fila")==0); return 0; }\n'
        )
        exe_fila = pasta / "teste_fila"
        subprocess.run(flags + [str(driver), "-o", str(exe_fila)], check=True)
        r = subprocess.run([str(exe_fila)], cwd=pasta, input="P\nteste fila\n\n\n0\n",
                           text=True, capture_output=True, timeout=10)
        assert r.returncode == 0, r.stderr

        # Caminho indisponivel para gravacao (diretorio no lugar do arquivo).
        arquivo.unlink()
        arquivo.mkdir()
        saida = rodar(exe, pasta, "S\nP\nnovo\n\n\n1\n" + "1\n" * 400)
        assert "Nao foi possivel salvar. Seus sinais anteriores foram mantidos." in saida
        assert "[SINAL] Ana pisca ;) para Caio." in saida
    print("OK: personalizacao, cancelamento, reabertura, vez preservada e erro de gravacao.")


if __name__ == "__main__":
    main()
