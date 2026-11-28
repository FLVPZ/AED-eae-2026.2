#!/usr/bin/env bash
# Compila include/*.c (o motor em C, reaproveitado sem reescrever as regras)
# para WebAssembly com o WASI SDK, e monta um unico arquivo index.html
# autocontido (CSS + JS + o .wasm embutido em base64) em web/dist/.
#
# Uso:
#   WASI_SDK_PATH=/caminho/para/wasi-sdk ./build_web.sh
#
# O WASI SDK (clang + sysroot wasm32-wasi) pode ser baixado em:
#   https://github.com/WebAssembly/wasi-sdk/releases
# Este projeto foi compilado e testado com a wasi-sdk-24.
#
# O arquivo final (web/dist/index.html) nao precisa de compilador nem de
# servidor: basta abrir com dois cliques em qualquer navegador moderno.
set -euo pipefail
cd "$(dirname "$0")"

WASI_SDK_PATH="${WASI_SDK_PATH:-/opt/wasi-sdk}"
CLANG="$WASI_SDK_PATH/bin/clang"
SYSROOT="$WASI_SDK_PATH/share/wasi-sysroot"

if [ ! -x "$CLANG" ]; then
  echo "Nao encontrei o clang do wasi-sdk em: $CLANG" >&2
  echo "Baixe o wasi-sdk (veja o README) e informe o caminho em WASI_SDK_PATH." >&2
  exit 1
fi

mkdir -p dist

echo "==> Compilando motor_web.c para WebAssembly (wasm32-wasi)..."
"$CLANG" --target=wasm32-wasi --sysroot="$SYSROOT" \
  -Oz -Wl,--export-dynamic -Wl,--allow-undefined -Wl,--strip-all \
  motor_web.c -o dist/motor.wasm

echo "==> Montando index.html autocontido..."
python3 - "$@" <<'PYEOF'
import base64, pathlib

aqui = pathlib.Path(__file__).resolve().parent if "__file__" in dir() else pathlib.Path(".")
raiz = pathlib.Path("web")
if not raiz.exists():
    raiz = pathlib.Path(".")

css = (raiz / "style.css").read_text(encoding="utf-8")
js = (raiz / "app.js").read_text(encoding="utf-8")
wasm_bytes = (raiz / "dist" / "motor.wasm").read_bytes()
wasm_b64 = base64.b64encode(wasm_bytes).decode("ascii")

modelo = (raiz / "index_template.html").read_text(encoding="utf-8")
final = modelo.replace("__CSS__", css).replace("__JS__", js).replace("__WASM_BASE64__", wasm_b64)

destino = raiz / "dist" / "index.html"
destino.write_text(final, encoding="utf-8")
print(f"Gerado: {destino} ({len(final)/1024:.0f} KB)")
PYEOF

echo "==> Pronto. Abra web/dist/index.html em qualquer navegador."
