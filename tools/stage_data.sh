#!/bin/sh
# Arma ux0_data/<slug>/ (= ux0:data/<slug>/ en la Vita) desde la .app del IPA, imitando el sandbox
# de iOS:
#   game_app/      el bundle (ejecutable + recursos), sin firma ni basura de macOS
#   rebase.bin     palabras con direcciones absolutas  } tools/ios_prep.py (análisis del Mach-O)
#   binds.bin      imports -> frameworks de Apple       }
#   nib.txt        MainWindow.nib precompilado (tools/nib_prep.py), si el juego tiene nib
#   controls.txt   botones -> toques (se crea con valores de ejemplo si no existe)
#   Documents/     partidas (no se toca)      logs/  <slug>_NNN.log (los escribe el loader)
# Además convierte la música .m4a/.mp3/.caf a .ogg (mismo nombre) para AudioFile (audio.c).
# Nada de esto se commitea. Uso: tools/stage_data.sh [ruta/a/X.app]
set -e
cd "$(dirname "$0")/.."
SLUG=$(sed -n 's/^#define PORT_SLUG "\(.*\)"/\1/p' source/port_config.h)
EXEC=$(sed -n 's/^#define PORT_EXEC "\([^"]*\)".*/\1/p' source/port_config.h)
APP="${1:-game_app}"
OUT=ux0_data/$SLUG
[ -f "$APP/$EXEC" ] || { echo "No encuentro $APP/$EXEC"; exit 1; }
mkdir -p "$OUT/game_app" "$OUT/Documents" "$OUT/logs"
rsync -a --delete --exclude '._*' --exclude '.DS_Store' --exclude '_CodeSignature' --exclude 'SC_Info' \
  --exclude 'embedded.mobileprovision' --exclude 'CodeResources' --exclude 'iTunesMetadata.plist' \
  --exclude '*.ogg.tmp' "$APP/" "$OUT/game_app/"
# PORT_ARCH opcional en port_config.h: deja solo ese slice de un binario universal (por defecto el loader
# y ios_prep.py usan armv7; armv6 = ARM puro, la ruta más validada)
ARCH=$(sed -n 's/^#define PORT_ARCH "\([a-z0-9]*\)".*/\1/p' source/port_config.h)
[ -n "$ARCH" ] && lipo -info "$OUT/game_app/$EXEC" | grep -q "are:" && lipo -thin "$ARCH" "$OUT/game_app/$EXEC" -output "$OUT/game_app/$EXEC"
python3 -I tools/ios_prep.py "$OUT/game_app/$EXEC" "$OUT" | head -12
[ -f "$APP/MainWindow.nib" ] && python3 -I tools/nib_prep.py "$APP/MainWindow.nib" "$OUT/nib.txt" > /dev/null || rm -f "$OUT/nib.txt"
# nibs de UIViewController (initWithNibName:): nibs/<nombre>.txt
mkdir -p "$OUT/nibs"
for n in "$APP"/*.nib; do
  [ -f "$n" ] || continue
  python3 -I tools/nib_prep.py "$n" "$OUT/nibs/$(basename "$n" .nib).txt" > /dev/null 2>&1 || echo "AVISO: nib $n sin convertir"
done
[ -f "$OUT/controls.txt" ] || cp tools/controls.example.txt "$OUT/controls.txt"
# Música: AAC/MP3/CAF -> Ogg Vorbis (AudioFileOpenURL busca <nombre>.ogg)
if command -v ffmpeg > /dev/null; then
  find "$OUT/game_app" -type f ! -name '._*' \( -name '*.m4a' -o -name '*.mp3' -o -name '*.caf' -o -name '*.aac' \) | while read -r f; do
    o="${f%.*}.ogg"
    if [ ! -f "$o" ]; then
      # encoder vorbis nativo de ffmpeg (el de Homebrew no trae libvorbis); solo estéreo
      enc() { ffmpeg -nostdin -loglevel error -i "$1" -vn -ac 2 -c:a vorbis -strict -2 -q:a 5 -f ogg "$o.tmp"; }
      # CAF IMA4 con chunk pakt de 0 paquetes (Ridge Racer): ffmpeg lo rechaza; sin el pakt calcula por tamaño
      enc "$f" 2>/dev/null && mv "$o.tmp" "$o" || {
        python3 -I -c "import sys;d=bytearray(open(sys.argv[1],'rb').read());i=d.find(b'pakt');d[i:i+4]=b'free' if i>0 else d[i:i+4];open(sys.argv[2],'wb').write(d)" "$f" "$o.caf"
        enc "$o.caf" && mv "$o.tmp" "$o" || { rm -f "$o.tmp"; echo "AVISO: no se pudo convertir $f"; }
        rm -f "$o.caf"; }
    fi
  done
else
  echo "AVISO: sin ffmpeg no hay música (.m4a -> .ogg)"
fi
echo "Listo: $OUT (subir con: python3 -I tools/deploy_data.py)"
