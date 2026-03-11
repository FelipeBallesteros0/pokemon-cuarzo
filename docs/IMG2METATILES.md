# img2metatiles

Herramienta determinística para convertir una imagen a:
- `tiles.4bpp`
- `metatiles.bin`
- `metatile_attributes.bin`

También puede aplicar los archivos directamente a un tileset secundario para usarlo en Porymap.

Archivo:
- `tools/img2metatiles.py`
- `tools/img2metatiles_gui.py` (interfaz gráfica)

## Requisitos
- Python 3
- Pillow (`PIL`)

## Uso básico
```bash
cd /home/user/decomps/pokeemerald-expansion-clean
python3 tools/img2metatiles.py \
  --input /ruta/a/imagen.png
```

Salida por defecto en:
- `build/img2metatiles_out/`

## Uso sin comandos (GUI)
Ubuntu/Linux:
```bash
./tools/run_img2metatiles_gui.sh
```

Windows:
- Ejecuta `tools\\run_img2metatiles_gui.bat`

En la GUI:
- seleccionas PNG de entrada
- opcionalmente seleccionas `Background Metatile 16x16`
- ajustas parámetros (tile base, paleta, animación, etc.)
- pulsas `Generate Tileset`

## Parámetros importantes
- `--mode keep-order|dedup`
  - `keep-order`: conserva el orden exacto (izq->der, arriba->abajo).
  - `dedup`: elimina duplicados de tiles y metatiles de forma estable.
- `--tile-base 0x200`
  - Base de tile ID que se escribe en `metatiles.bin`.
  - Para secondary tileset normalmente `0x200`.
- `--palette-slot 6`
  - Slot de paleta que se codifica en cada metatile.
- `--metatile-attr 0x0000`
  - Atributo base para todos los metatiles.
- `--bg-metatile-png /ruta/fondo16x16.png`
  - PNG opcional de `16x16` que se repite como fondo detrás de píxeles transparentes.
  - Útil para flores/objetos animados sin fondo en la imagen original.

## Aplicar directo a un secondary tileset
```bash
python3 tools/img2metatiles.py \
  --input /mnt/c/Users/User/Desktop/pokemon_esmeralda/tiles/TILES/pool.png \
  --mode keep-order \
  --tile-base 0x200 \
  --palette-slot 6 \
  --apply-to data/tilesets/secondary/pueblo_ciendra_pool \
  --write-palette
```

Esto escribe en el tileset destino:
- `tiles.4bpp`
- `tiles.png`
- `metatiles.bin`
- `metatile_attributes.bin`
- `palettes/06.gbapal` y `palettes/06.pal` (si usas `--write-palette`)

## Modo animación (ejemplo pileta 8 frames)
Si tu PNG contiene varios frames en grilla (como `pool.png` 512x256 = 4x2 frames de 128x128):
```bash
python3 tools/img2metatiles.py \
  --input /mnt/c/Users/User/Desktop/pokemon_esmeralda/tiles/TILES/pool.png \
  --mode dedup \
  --tile-base 0x200 \
  --palette-slot 6 \
  --apply-to data/tilesets/secondary/pueblo_ciendra_pool \
  --write-palette \
  --quantize \
  --anim \
  --frames-x 4 \
  --frames-y 2 \
  --frame-width 128 \
  --frame-height 128 \
  --anim-name pool
```

En este modo:
- El frame 0 se usa para `tiles.4bpp` y `metatiles.bin`.
- Se exportan los 8 frames a `anim/pool/0.4bpp` ... `7.4bpp`.

## Determinismo
- La herramienta no usa procesos aleatorios.
- Con la misma imagen y parámetros, genera los mismos bytes.

## Notas
- La imagen debe ser divisible por 16 en ancho y alto.
- Recomendado usar PNG indexado (`mode P`) con máximo 16 colores.
- Si no está indexada, puedes forzar con `--quantize`.
- Límite técnico del metatile word: tile ID de 10 bits (`0x000..0x3FF`).
  - Si usas `--tile-base 0x200`, el máximo práctico son 512 tiles únicos.
  - Para imágenes grandes, usa `--mode dedup` o reduce la imagen.
