from pathlib import Path
import subprocess
import tempfile
import re

ROOT=Path(__file__).resolve().parents[1]
FLAGS=['gcc','-std=c99','-Wall','-Wextra','-Werror','-pedantic']

def main():
    with tempfile.TemporaryDirectory() as d:
        pasta=Path(d); exe=pasta/'truco'; unit=pasta/'stats'
        subprocess.run(FLAGS+[str(ROOT/'main.c'),'-o',str(exe)],check=True)
        subprocess.run(FLAGS+[str(ROOT/'tests/test_estatisticas.c'),'-o',str(unit)],check=True)
        for aumentos in range(3):
            for resposta in ['A','C']:
                r=subprocess.run([str(unit),str(aumentos)],input='Z\n'+'T\nZ\n'*aumentos+resposta+'\n',text=True,capture_output=True,cwd=pasta,timeout=10)
                assert r.returncode==0,r.stderr
        for indice,comandos in enumerate(['1\n'*400,'T\nA\n1\n'*400,'T\nC\n1\n'*400,'T\nT\nA\n1\n'*400,'T\nT\nT\nA\n1\n'*400]):
            caso=pasta/str(indice); caso.mkdir()
            antes={n:[0]*5 for n in ['Ana','Bia','Caio','Davi']}
            for partida in range(2):
                r=subprocess.run([str(exe)],cwd=caso,input='Ana\nBia\nCaio\nDavi\n'+comandos,text=True,capture_output=True,timeout=15)
                assert r.returncode==0 and not r.stderr,r.stderr
                atuais={p[0]:list(map(int,p[1:])) for p in (l.split(';') for l in (caso/'caderneta.txt').read_text().splitlines())}
                resultados=re.findall(r'Dupla ([12]) leva a mao e faz (\d+) ponto',r.stdout)
                for n,dupla in [('Ana','1'),('Caio','1'),('Bia','2'),('Davi','2')]:
                    assert atuais[n][0]==antes[n][0]+1
                    assert atuais[n][1]==antes[n][1]+sum(v==dupla for v,_ in resultados)
                    assert 0<=atuais[n][3]<=atuais[n][4]
                pedidos=re.findall(r'\n(\w+) \(Dupla [12]\) pede (TRUCO|SEIS|DOZE)',r.stdout)
                recebidos=re.findall(r'\n\w+ \(Dupla [12]\) pede (?:TRUCO|SEIS|DOZE)\n.*?Resposta de (\w+) \(Dupla [12]\)',r.stdout,re.S)
                fugas=re.findall(r'(\w+) correu, Dupla',r.stdout)
                for n in atuais:
                    assert atuais[n][2]==antes[n][2]+sum(p==n for p,_ in pedidos)
                    assert atuais[n][4]==antes[n][4]+recebidos.count(n)
                    assert atuais[n][3]==antes[n][3]+fugas.count(n)
                maior=int(re.search(r'Mao mais cara da partida: (\d+)',r.stdout)[1])
                assert maior==max(int(p) for _,p in resultados)
                if pedidos:
                    assert re.search(r'(Ana|Bia|Caio|Davi) \(Dupla [12]\) pediu \d+;',r.stdout)
                antes=atuais
    print('OK: 6 negociacoes com entradas invalidas e 10 partidas com estatisticas persistidas/reabertas.')

if __name__=='__main__': main()
