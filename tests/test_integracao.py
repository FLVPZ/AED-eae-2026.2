"""Compila e verifica cadastro, caderneta, murais e encerramento da partida."""
from pathlib import Path
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def jogar(executavel, entrada):
    resultado = subprocess.run(
        [str(executavel)], input=entrada, text=True, capture_output=True, timeout=15
    )
    assert resultado.returncode == 0, resultado.stderr
    return resultado.stdout


def main():
    with tempfile.TemporaryDirectory() as pasta:
        executavel = Path(pasta) / "truco"
        subprocess.run(
            ["gcc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-pedantic",
             str(ROOT / "main.c"), "-o", str(executavel)], check=True
        )
        # Nomes em ordem diferente da alfabetica; partida inteira sem apostas.
        nomes = "Davi\nBia\nCaio\nAna\n"
        saida = jogar(executavel, nomes + "1\n" * 400)
        assert "FIM DE PARTIDA!" in saida
        assert saida.count("MURAL DO BAR -") == 2
        assert saida.count("MURAL DA VERGONHA -") == 2
        caderneta = saida.split("CADERNETA DO BAR - ordem alfabetica")[1]
        caderneta = caderneta.split("Estatisticas ainda")[0]
        posicoes = [caderneta.index("  " + nome)
                    for nome in ["Ana", "Bia", "Caio", "Davi"]]
        assert posicoes == sorted(posicoes)
        assert saida.count("sem pedidos recebidos") == 8
        assert saida.count("0 mao(s) ganha(s)") == 8

        # Novo nome deve ser pedido ao encontrar vazio, duplicado ou longo.
        entrada = "\nDavi\nDavi\nBia\n" + "x" * 40 + "\nCaio\nAna\n"
        saida = jogar(executavel, entrada + "1\n" * 400)
        assert saida.count("Informe um nome nao vazio") == 2
        assert "Nome muito longo" in saida
        assert "FIM DE PARTIDA!" in saida

        # O limite de 29 caracteres e aceito; fechamento no cadastro e seguro.
        nome_limite = "N" * 29
        saida = jogar(executavel, nome_limite + "\nBia\nCaio\nAna\n" + "1\n" * 400)
        assert "  " + nome_limite in saida
        assert "Nome muito longo" not in saida
        assert "FIM DE PARTIDA!" in saida
        assert "FIM DE PARTIDA!" not in jogar(executavel, "Ana\n")
        assert "FIM DE PARTIDA!" not in jogar(executavel, "")
    print("OK: cadastro validado, lista alfabetica, murais iniciais/finais e partida completa.")


if __name__ == "__main__":
    main()
