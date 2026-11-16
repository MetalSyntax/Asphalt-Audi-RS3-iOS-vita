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
