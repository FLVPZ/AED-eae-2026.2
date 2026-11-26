"""Verifica a interface por redirecionamento e em um terminal real (PTY)."""
from pathlib import Path
import os
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def testar_terminal(exe, largura=80, sem_cor=False, simples=False):
    import fcntl
    import pty
    import select
    import struct
    import termios
    import time

    master, slave = pty.openpty()
    fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 28, largura, 0, 0))
    ambiente = {k: v for k, v in os.environ.items() if k != "NO_COLOR"}
    ambiente["TERM"] = "dumb" if simples else "xterm-256color"
    if sem_cor:
        ambiente["NO_COLOR"] = "1"
    processo = subprocess.Popen([str(exe)], stdin=slave, stdout=slave,
                               stderr=slave, env=ambiente)
    os.close(slave)

    def ler_ate(marcador):
        resposta = b""
        limite = time.monotonic() + 5
        while marcador not in resposta:
            restante = limite - time.monotonic()
            assert restante > 0, resposta.decode(errors="replace")
            if select.select([master], [], [], restante)[0]:
                resposta += os.read(master, 65536)
        return resposta

    try:
        ler_ate(b"Nome do Jogador 1")
        os.write(master, b"Ana\nBia\nCaio\nDavi\n")
        if not simples:
            ler_ate(b"Abrir a mesa [Enter]")
            os.write(master, b"\n")
        resposta = ler_ate(b"Sua escolha: ")
        assert b"Vez de Ana" in resposta
        if simples:
            assert b"\x1b[" not in resposta
        else:
            assert b"\x1b[2J\x1b[H" in resposta
            cores = re.findall(rb"\x1b\[[0-9;]*m", resposta)
            assert bool(cores) != sem_cor
        if largura < 68:
            assert b"+------------------+" not in resposta
        else:
            assert b"+------------------+" in resposta

        # Um sinal, cancelamento, consulta vazia e erro nao passam a vez.
        os.write(master, b"S\n")
        ler_ate(b"Escolha: ")
        os.write(master, b"1\n")
        resposta = ler_ate(b"Sua escolha: ")
        assert b"Vez de Ana" in resposta and b"Sinal:" in resposta
        os.write(master, b"S\n")
        ler_ate(b"Escolha: ")
        os.write(master, b"0\n")
        resposta = ler_ate(b"Sua escolha: ")
        assert b"Vez de Ana" in resposta and b"Sinal cancelado" in resposta
        os.write(master, b"V\n")
        resposta = ler_ate(b"Sua escolha: ")
        assert b"Vez de Ana" in resposta and b"ainda nao tem" in resposta
        os.write(master, b"Z\n")
        resposta = ler_ate(b"Sua escolha: ")
        assert b"Vez de Ana" in resposta and b"Opcao invalida" in resposta
    finally:
        processo.terminate()
        processo.wait(timeout=5)
        os.close(master)


def main():
    with tempfile.TemporaryDirectory() as pasta:
        exe = Path(pasta) / "truco"
        subprocess.run(["gcc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                        "-pedantic", str(ROOT / "main.c"), "-o", str(exe)], check=True)
        r = subprocess.run([str(exe)], input="Ana\nBia\nCaio\nDavi\n" + "1\n" * 400,
                           text=True, capture_output=True, timeout=15, cwd=pasta)
        assert r.returncode == 0, r.stderr
        assert "FIM DE PARTIDA!" in r.stdout
        assert "\x1b[" not in r.stdout
        assert "Abrir a mesa [Enter]" not in r.stdout
        assert "Personalizar" not in r.stdout
        assert not (Path(pasta) / "sinais.txt").exists()
        if os.name != "nt":
            testar_terminal(exe)
            testar_terminal(exe, sem_cor=True)
            testar_terminal(exe, largura=40)
            testar_terminal(exe, simples=True)
    print("OK: terminal, cores opcionais, modo compacto e vez preservada.")


if __name__ == "__main__":
    main()
