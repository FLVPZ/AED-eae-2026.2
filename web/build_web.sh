#!/usr/bin/env bash
# Compila motor_web.c com os modulos de include/ reaproveitados
# para WebAssembly com o WASI SDK, e monta um unico arquivo index.html
# autocontido (CSS + JS + o .wasm embutido em base64) em web/dist/.
#
# Uso:
#   WASI_SDK_PATH=/caminho/para/wasi-sdk ./build_web.sh
#   ZIG=/caminho/para/zig ./build_web.sh
#
# O WASI SDK (clang + sysroot wasm32-wasi) pode ser baixado em:
#   https://github.com/WebAssembly/wasi-sdk/releases
# Alternativa: Zig com suporte ao alvo wasm32-wasi.
#
# O arquivo final (web/dist/index.html) nao precisa de compilador nem de
# servidor: basta abrir com dois cliques em qualquer navegador moderno.
set -euo pipefail
cd "$(dirname "$0")"

WASI_SDK_PATH="${WASI_SDK_PATH:-/opt/wasi-sdk}"
CLANG="$WASI_SDK_PATH/bin/clang"
SYSROOT="$WASI_SDK_PATH/share/wasi-sysroot"

mkdir -p dist

echo "==> Compilando motor_web.c para WebAssembly (wasm32-wasi)..."
OPCOES=(-std=c99 -Oz -mexec-model=reactor -Wl,--export-dynamic -Wl,--strip-all)
if [ -x "$CLANG" ]; then
  "$CLANG" --target=wasm32-wasi --sysroot="$SYSROOT" \
    "${OPCOES[@]}" motor_web.c -o dist/motor.wasm
elif [ -n "${ZIG:-}" ] || command -v zig >/dev/null 2>&1; then
  "${ZIG:-zig}" cc -target wasm32-wasi \
    "${OPCOES[@]}" motor_web.c -o dist/motor.wasm
else
  echo "Nao encontrei o WASI SDK nem o Zig. Configure WASI_SDK_PATH ou ZIG." >&2
  exit 1
fi

echo "==> Montando index.html autocontido..."
python3 - "$@" <<'PYEOF'
import base64, pathlib

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
