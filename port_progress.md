# Asphalt Audi RS 3 — Progreso del Port (iOS Mach-O)

Documentación completa y arquitectura en `PORTING_PLAN.md`.

## Resumen de Estado

- **Juego:** Asphalt Audi RS 3 (v1.0.1)
- **Plataforma Original:** iOS (Mach-O ARMv7 32-bit, FAT Universal)
- **Motor:** Gameloft Asphalt Racing Engine (C++ / OpenGL ES 1.1 & 2.0 / Glitch)
- **Title ID:** `PSVAUDIRS`
- **Slug:** `asphalt-audirs3`
- **Build Actual:** `build/asphalt_audirs3.vpk` & `build/eboot.bin` en proceso de compilación y empaquetado.
- **Estado:** Scaffolding, análisis dinámico de entrada (`LC_UNIXTHREAD`) y loader completados; compilación en curso para pruebas en PS Vita física.

---

## Fases del Port

### Phase 1: Repo and tools ✅
- [x] IPA extraído (`asphalt-audirs3_extract/Payload/AsphaltAudiRS3.app`), symlink `game_app` funcional.
- [x] Binario descifrado verificado (`cryptid 0`), arquitectura ARMv7 seleccionada de FAT (armv6/armv7).
- [x] `rebase.bin` generado con 8529 palabras de relocalización (`docs/macho_report.txt`).
- [x] Scripts de compilación y empaquetado `build.sh` y `CMakeLists.txt` operativos.

### Phase 2: Decompilation and inventory ✅
- [x] Dumps generados en `decompiled/macho/`:
  - `frameworks.txt`: `OpenGLES`, `OpenAL`, `AudioToolbox`, `UIKit`, `Foundation`, `QuartzCore`, `CoreGraphics`, `CoreMotion`, etc.
  - `objc_classes.txt`: clases UIView y UIKit registradas.
  - `symbols.txt`: tabla de símbolos del motor de carreras Asphalt.
  - `imports.txt`: imports clasificados.
- [x] Funciones OpenGL ES vinculadas a vitaGL.

### Phase 3: Real engine analysis ✅
- [x] Motor identificado: **Gameloft Asphalt Racing Engine**:
  - Arquitectura: Edición especial centrada en el Audi RS 3 Sportback sobre el motor de Asphalt 6, pistas urbanas de Tokio y Bahamas.
  - Ciclo de vida: `LC_UNIXTHREAD` (`pc = 0x000d119c`) $\rightarrow$ inicialización de runtime C++ y UIKit $\rightarrow$ loop de renderizado OpenGL ES.
  - Audio: OpenAL y AudioQueue para SFX del motor 2.5L TFSI de 5 cilindros y música.
  - Entrada: Acelerómetro/giroscopio con controles táctiles configurables en pantalla.

### Phase 4: Loader and ObjC bootstrap ✅
- [x] `macho_load()` mapea segmentos en `MACHO_SLIDE = 0x98000000`, procesa `rebase.bin` y enlaza símbolos indirectos.
- [x] Detección de binario FAT universal e identificación de la slice ARMv7 con su punto de entrada `LC_UNIXTHREAD` (`pc = 0x000d119c`).
- [x] Trampolines `ml_align_call` en `asm.S` para realinear la pila de 4 a 8 bytes de forma segura.
- [x] Protección de símbolos de datos (`is_data_symbol`): asigna bloques de memoria cero (`make_unimpl_data`) a símbolos `_OBJC_CLASS_$_*`, `_OBJC_METACLASS_$_*`, `_kCF*` y vtables C++.
- [x] Shims POSIX y runtime C++ en `source/patch.c`: `open`, `read`, `write`, `lseek`, `fstat`, `remove`, `getenv`, `setenv`, `printf`, `fprintf`, `operator new`, `operator delete`.
- [x] Gestor de logs rotativos (`ux0:data/asphalt-audirs3/logs/asphalt-audirs3_NNN.log`).

### Phase 5: Graphics (Build ✅, Prueba de hardware en progreso)
- [x] `vendor/vitaGL` compilado en modo `SOFTFP_ABI=1` con redirección de viewport float a VFP (`sceGxmSetViewport_sfp`).
- [x] Inicialización con `vglInitExtended` en resolución nativa 960x544.
- [x] Funciones OpenGL ES vinculadas en `source/gles.c`.
- [ ] Verificación en pantalla del modelo 3D detallado del Audi RS 3 y pistas.

### Phase 6: Touch ✅
- [x] Capa de entrada en `source/input.c` con escalado y traslación de coordenadas táctiles frontales a espacio lógico.
- [ ] Verificación de navegación en menú principal y opciones de control.

### Phase 7: Physical controls ✅
- [x] Esquema de mapeo diseñado:
  - Stick Analógico Izquierdo (LX) / Cruceta Izq-Der: Giro / Dirección del Audi RS 3.
  - ✕ (Cruz): Acelerar.
  - □ (Cuadrado): Freno / Derrape.
  - △ (Triángulo): Nitro / Boost.
  - ○ (Círculo): Freno de mano / Cámara.
  - Botón L: Mirar atrás.
  - Botón R: Nitro alternativo.
  - START: Pausa.
- [ ] Ajuste de sensibilidad y zonas muertas en hardware físico.

### Phase 8: Audio
- [x] Rutas de OpenAL y AudioQueue registradas.
- [x] Enrutamiento a `sceAudioOut` en `source/coreaudio.c`.
- [ ] Prueba del sonido característico de escape del Audi RS 3.

### Phase 9: Videos / FMV
- [x] Símbolos de MediaPlayer registrados.
- [ ] Salto seguro en caso de ausencia de vídeos.

### Phase 10: Assets, data, language and saves ✅
- [x] Datos de la app redirigidos a `ux0:data/asphalt-audirs3/AsphaltAudiRS3.app/`.
- [x] Partidas guardadas y configuración a `ux0:data/asphalt-audirs3/saves/`.
- [x] Detección de idioma del sistema mediante `sceAppUtilSystemParamGetInt`.
- [ ] Validación de guardado de récords de vuelta y tiempos.

### Phase 11: Performance ✅
- [x] Overclock configurado por defecto en `main.c` (CPU 444 MHz, GPU 222 MHz).
- [ ] Monitoreo de tasa de cuadros en hardware real.

### Phase 12: Configurator and options
- [ ] Opciones de configuración persistente para estilo de conducción y controles.

### Phase 13: Trophies
- [ ] Mapeo de logros locales o GameKit a `sceNpTrophy`.

### Phase 14: LiveArea and VPK ✅
- [x] Configuración de `param.sfo` con TITLEID `PSVAUDIRS`.
- [x] Assets de LiveArea listos.
- [x] Generación de `eboot.bin` y empaquetado VPK.

### Phase 15: Real-hardware testing and release (Siguiente paso)
- [ ] Desplegar datos: `psvita-toolkit deploy --data`
- [ ] Desplegar VPK: `psvita-toolkit deploy`
- [ ] Iteración build-run-triage con logs en consola física.

---

## Hosttest ARM32 (Docker armel, sin consola) - 2026-11-19

Resultado: el juego llega al **splash/título** ("ASPHALT RS3 / Touch to continue / (c) 2011 Gameloft") en el FBO del juego
(glReadPixels -> `build/hosttest-data/evidence/fbo_2.png`, log en `evidence/hosttest_run.log`). Los vídeos de arranque
(`LOGO_Gameloft_480x320.m4v`, `RS3_Emomodul_HD_iPhone.m4v`) se localizan y se "reproducen" (sceAvPlayer falso los salta).
`sh hosttest/run.sh build && HOSTTEST_FRAME_S=4 sh hosttest/run.sh run 110` (imágenes `hosttest-*` propias).
2026-11-23: el compuesto final 960x544 ya muestra el título girado a paisaje (`evidence/screen_title.png`). Salía negro
porque el juego (GLES2) deja activos los atributos genéricos 0-2 y en el perfil de compatibilidad de Mesa el atributo 0
sustituye a `glVertexPointer`: `gles_present` los desactiva durante el blit y los restaura. No se llegó a menús/juego.

Cambios de fondo:
- **Runtime sustituido por la familia común Gameloft iOS** (copiado de Gangstar-Rio-iOS-vita: foundation/uikit/objc/darwin/
  gles/openal/movie/coretext/ios5 + `port_config.h`, `tools/ios_prep.py`, `nib_prep.py`, `gen_cxx.py`). El andamiaje anterior
  (2400 líneas, sin `objc_msgSend`, sin nib, sin binds) no podía arrancar el juego; sigue en el historial de git.
- `ux0_data/asphalt-audirs3/` ahora: `game_app/` (la .app), `rebase.bin`, `binds.bin`, `nib.txt` (MainWindow.nib ->
  `Asphalt6AppDelegate` + `EAGLView`), `Documents/`, `logs/`. Se regenera con `tools/stage_data.sh` / `ios_prep.py --arch armv7`.
- `tools/ios_prep.py` (binario iOS 4.2 no PIE, sin símbolos, con LC_DYLD_INFO): (1) mezcla los binds/rebases exactos de
  LC_DYLD_INFO (superclase de las clases ObjC: sin ello `-[EAGLView layer]` no existía); (2) literales que apuntan al interior de
  `__cstring` (ld funde sufijos de cadenas); (3) funciones huérfanas guardadas como callback (sin prólogo) se escanean y sus
  literales se rebasean; (4) vtables con entradas hoja/thunks, ranuras nulas y la marca `0x6f75746c`.
- El entry de LC_UNIXTHREAD era el del slice armv6 en la doc anterior (0xd119c); armv7 = 0xd051c, `main` = 0xd4fc8.
- `foundation.c`: `NSBundle executablePath` (ruta estilo iOS; el juego la corta para su chdir), `NSURL initFileURLWithPath:`,
  `NSString rangeOfString:options:`.
- `darwin.c`: `pthread_cond_t` de Darwin son 28 bytes y la de glibc 48: pisaba `count/runnable` del Thread de Gameloft
  (NULL deref en 0x594BB4); ahora la cond va en heap. `darwin_path`: ruta que termina en `.app` = bundle; bajo `HOSTTEST` rutas
  `ux0:` absolutas y `/game/game_app/` (getcwd tras chdir por symlink) -> bundle.
- `gles.c`: vuelca el shader que falla al compilar (diagnóstico). Hosttest: `gles2_fake.c` limpia el `#endif]` de
  `file000351.dat` (PowerVR lo ignora, Mesa no; en la Vita habrá que comprobar si shacccg lo tolera).
- `CMakeLists.txt` (Vita) alineado con Gangstar (fuentes y librerías). **No compilado para la Vita**, solo hosttest.

## Consola, log 002 + dump (2026-11-23)

- **Crash `UNDEFINED_INSTRUCTION` en 0x984788b8 (Mach-O 0x4788b8)**: `swp` de ARMv6 en el pool de spinlocks de libstdc++
  (26 sitios, 0x478364..0x478c30); el Cortex-A9 de la Vita no ejecuta SWP. `patch.c` los cambia por un `B` a stubs
  LDREX/STREX + DMB en la arena de thunks (`macho_alloc_code`). El hosttest (qemu) sí ejecuta SWP: por eso no salía ahí.
- Videos: `sceIoOpen("data/x.m4v")` relativo daba 0x80010016 (sceIo ignora el chdir de newlib): `movie_play` lo hace absoluto.
- Logo de Gameloft abajo a la derecha: `Default-Landscape.png` (iPad) lo trae así; `loading_image` lo recorta y lo centra.
- Carga (ideas de Gangstar Rio): caché negativa en `d_fopen` para el bundle (glsl.config se pedía 2-6 veces por shader),
  `-O3` y `CMAKE_C_FLAGS_DEBUG -O3` (el preset debug metía -O0), vitaGL `BUILD_ALWAYS` + `make clean`.
- Hosttest: título a los ~22 s; `run.sh` por defecto 120 s con `timeout -k 10` (una corrida quedó colgada en el
  volcado de pilas del SIGTERM); barra de carga activa también en el hosttest (`evidence/loading_logo.png`).
- Log 005 (consola): el parche `swp` funciona (26 sitios). La pantalla de carga tapaba la de Audi en cada carga: ahora
  `Default-Landscape.png` (logo abajo a la derecha, como iOS) solo hasta el primer frame del juego, sin barra ni
  `load_count.txt`. Pendiente: videos "sin memoria para frames (1 MB): 0x80024302"; el juego presenta con 0 draws.

## Log 006 — primer ingame; crash PREFETCH_ABORT pc 0x00249edc
- Causa: tabla de structs {id, fn, ...} en __data (0x74a0f0, paso 20) con punteros a funciones hoja sin prólogo
  (`mov r0,#3; bx lr`). La heurística de `tools/ios_prep.py` las dejó como `data->text-nofunc` (sin rebase): el
  `blx r2` saltó a la dirección sin slide. Fix: un data->text dudoso se acepta si la misma columna del struct
  (paso 8..32) ya tiene un puntero a código aceptado y el destino es ARM AL tras un retorno/salto. +303 rebases
  (vtables y punteros a miembro), 0 quitados. `rebase.bin` regenerado y subido.
- Extras de Ferrari GT: menú de video SELECT+START (`options.c`, `font8x8.c`): tamaño (aspecto/pantalla completa),
  filtro sharp bilinear, MSAA 2x/4x (al reiniciar), rotación; se guarda en `options.txt` (reemplaza rotation.txt).
  Probado en hosttest (abre/cierra, sharp bilinear 320x480 x2).
- Pendiente: errores gráficos en los autos (falta captura). Faltan en los .dat las texturas
  `*_Car_Body_Reflection_ForShader.png` (el juego prueba variantes); películas: sin memoria para frames.

## Log 007 — mismo crash (tabla de estados 0x74a0f0, entradas 22 y 30) + autos con bloques + sin sonido
- Crash pc 0x00249fec: dos handlers más de la tabla quedaron sin rebase porque la palabra anterior al destino es un
  literal que el escaneo no registró. `ios_prep.py`: palabra previa que no es instrucción ARM AL cuenta como fin de
  función. +2 rebases (exactamente esas dos); la tabla de 40 estados queda completa.
- Sonido: `coreaudio.c` (AudioUnit RemoteIO -> sceAudioOut, copia probada de Gangstar Rio) no se compilaba; el juego
  mezcla todo (música y efectos) por ahí. Agregado a CMake e imports (hilo en núcleos 1-2, CPU_HELPERS). Hosttest:
  "AudioUnit: 44100 Hz, 2 channels, 16 bits".
- Autos con bloques (captura 2026-11-23-153604): se quitó DRAW_SPEEDHACK=2 de vitaGL (la GPU leía sin copia los
  arrays de vértices que el motor reusa por pieza). Hipótesis por analogía con Asphalt 6 (Bug #030); confirmar en
  consola. Descartado: precisión mediump (vitaGL no usa half), offsets de mips PVRTC, sesgo de texture2D.

## Log 008 — sin crash, sonido OK; autos todavía con bloques; controles
- DRAW_SPEEDHACK no era la causa (y bajaba de ~55 a ~30 fps): restaurado `DRAW_SPEEDHACK=2`.
- Hosttest como referencia: decodificador PVRTC 4bpp por software en `hosttest/vitagl_fake.c` (Mesa no tiene PVRTC;
  antes pista y reflejos salían negros) + taps del log 008 convertidos a pantalla para llegar a carrera en <120 s.
  En el hosttest el auto se ve bien (rojo claro brillante) => el fallo es propio de vitaGL/GXM.
- Causa encontrada: el shader del auto (`file000467` VS + `file000157` FS) declara 11 varyings y GXM tiene 10
  TEXCOORD; vitaGL (NO_DEBUG oculta el "TEXCOORD overflow") metía el 11.º en TEXCOORD9 junto con vCoord0 (UV de la
  carrocería) => trozos del atlas (asientos) y bandas. Es el único shader del juego con >10. Fix en `gles_init`:
  `vglAddSemanticBindingHint` de vLightAmbient/vLightDiffuse a COLOR (colores [0,1]) => 9 TEXCOORD.
  La caché de vitaGL se indexa por el GLSL original: ahora vive en `ux0:data/shader_cache/PSVAUDIRS-sem1`
  (subir el sufijo si cambian los bindings). La vieja `ux0:data/shader_cache/PSVAUDIRS` se puede borrar.
- vitaGL `change_cull_mode`: glCullFace(GL_FRONT) no dibujaba nada (no_polygons_mode); ahora = BACK con winding
  invertido. El juego no usa GL_FRONT (verificado), pero queda corregido.
- Controles: `controls.txt` por defecto (R gas, L/□ freno, X nitro, ○ gas alt., START pausa; coordenadas sacadas de
  la captura) + remapeo en el menú SELECT+START (sección CONTROLS, izq/der cambia el botón, se guarda en
  controls.txt con la etiqueta de la acción). Cruceta izq/der = inclinación digital si no está asignada.
  MAX_BIND 8 (uikit MAX_TOUCH 11). Probado en hosttest: los botones tocan EAGLView y el menú se ve bien.

## Log 011 — en carrera; autos oscuros, sin reflejos, carrocería transparente en el choque
- GLES2 (initWithAPI:2): los TRACE de FFP del log no muestran el estado real del shader.
- El hint COLOR del log 008 recorta las luces a [0,1] con poca precisión; además el FS `file000157` escribe sobre
  sus varyings y calcula `vCoord1` en un varying que el VS no emite.
- Fix: `g_glShaderSource` reemplaza el par `file000467`/`file000157` (firma por texto) por una versión con la misma
  matemática: luces y DiffuseColor como uniforms del FS (vitaGL los encuentra por nombre), 6 varyings, sin escribir
  varyings. Se quitaron los `vglAddSemanticBindingHint`; caché nueva `ux0:data/shader_cache/PSVAUDIRS-sem2`
  (la `-sem1` se puede borrar). Validado con glslangValidator; falta confirmar en consola.

## Logs 012-013 — media carrocería invisible / carrocería rayada al chocar
- El reemplazo de 157/467 no aplica: en carrera la carrocería es el programa `file000044` VS + `file000025` FS
  ("minReflection"), un solo draw de 5874 índices (GL_UNSIGNED_INT) con arrays en RAM (sin VBO, stride 32).
- El doble draw con glFrontFace CW/CCW del showroom es el reflejo en el piso, no la mitad espejada.
- Log 014 (sin SAMPLERS/DRAW_SPEEDHACK, igual) descarta la lectura tardía de vértices; se quitó la copia `draw_copied`.
- Causa: en carrera la carrocería es `file000345` (programa 32) con posición+normal intercaladas (stride 24) y UV en
  otro array (stride 32, ~15 MB más allá). vitaGL sin STRICT_DRAW_COMPLIANCE decidía "empaquetado" mirando solo
  attr_map[1] y leía las UV como parte del stream 0 => UV basura. Fix en `vendor/vitaGL/source/custom_shaders.c`
  (`vgl_attribs_interleaved`): todos los atributos activos dentro de la fila del stream 0 y con su stride.
  Diagnóstico T2/SH/PROG en el log (temporal).
