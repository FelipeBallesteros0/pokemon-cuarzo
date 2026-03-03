#!/usr/bin/env python3
"""
Build clean source previews and in-game door assets for the 2x2 Edificio Pokeball door.

What it does:
1) Creates clean editable PNGs from the source animation:
   - *_animacion_puerta_fuente_limpia_2x2.png (full animation sheet)
   - *_puerta_fuente_limpia_2x2.png (first 32x32 frame)
2) Generates in-game assets used by field_door.c:
   - graphics/door_anims/edificio_pokeball.png  (indexed, 16-color)
   - graphics/door_anims/edificio_pokeball.gbapal

The conversion is stable and avoids artifact-prone reordering.
"""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import List, Tuple

from PIL import Image


Rgb = Tuple[int, int, int]


def read_gbapal_colors(path: Path) -> List[Rgb]:
    raw = path.read_bytes()[:32]
    if len(raw) < 32:
        raise ValueError(f"Palette file too short: {path}")

    colors: List[Rgb] = []
    for i in range(0, 32, 2):
        v = raw[i] | (raw[i + 1] << 8)
        r = (v & 31) * 255 // 31
        g = ((v >> 5) & 31) * 255 // 31
        b = ((v >> 10) & 31) * 255 // 31
        colors.append((r, g, b))
    return colors


def write_clean_pngs(source_anim: Path, out_anim_clean: Path, out_door_clean: Path) -> None:
    img = Image.open(source_anim)
    out_anim_clean.parent.mkdir(parents=True, exist_ok=True)
    out_door_clean.parent.mkdir(parents=True, exist_ok=True)
    img.save(out_anim_clean)
    img.crop((0, 0, 32, 32)).save(out_door_clean)

def split_left_right_pngs(source_anim: Path, out_left: Path, out_right: Path) -> None:
    img = Image.open(source_anim).convert("RGBA")
    if img.size != (32, 96):
        raise ValueError(f"Expected 32x96 source sheet, got {img.size}")
    out_left.parent.mkdir(parents=True, exist_ok=True)
    out_right.parent.mkdir(parents=True, exist_ok=True)
    img.crop((0, 0, 16, 96)).save(out_left)
    img.crop((16, 0, 32, 96)).save(out_right)

def combine_left_right_pngs(source_left: Path, source_right: Path, out_combined: Path) -> None:
    left = Image.open(source_left).convert("RGBA")
    right = Image.open(source_right).convert("RGBA")
    if left.size != (16, 96) or right.size != (16, 96):
        raise ValueError(f"Expected 16x96 halves, got left={left.size} right={right.size}")
    out = Image.new("RGBA", (32, 96), (255, 0, 255, 0))
    out.paste(left, (0, 0))
    out.paste(right, (16, 0))
    out_combined.parent.mkdir(parents=True, exist_ok=True)
    out.save(out_combined)


def map_to_fixed_palette(source_anim: Path, palette_colors: List[Rgb], out_png: Path) -> None:
    src = Image.open(source_anim).convert("RGBA")
    out = Image.new("P", src.size)

    flat_pal: List[int] = []
    for r, g, b in palette_colors:
        flat_pal.extend([r, g, b])
    flat_pal.extend([0] * (768 - len(flat_pal)))
    out.putpalette(flat_pal)

    sp = src.load()
    dp = out.load()

    for y in range(src.height):
        for x in range(src.width):
            r, g, b, a = sp[x, y]
            if a < 128 or (r, g, b) == (255, 0, 255):
                dp[x, y] = 0
                continue

            best_i = 1
            best_d = 10**18
            for i, (pr, pg, pb) in enumerate(palette_colors[1:], start=1):
                d = (r - pr) * (r - pr) + (g - pg) * (g - pg) + (b - pb) * (b - pb)
                if d < best_d:
                    best_d = d
                    best_i = i
            dp[x, y] = best_i

    out_png.parent.mkdir(parents=True, exist_ok=True)
    out.save(out_png)


def map_to_source_palette(source_anim: Path, out_png: Path) -> None:
    src = Image.open(source_anim).convert("RGBA")
    rgb = Image.new("RGB", src.size, (0, 0, 0))
    rgb.paste(src, mask=src.split()[3])

    # Stable, no-dither quantization to avoid frame-to-frame speckle artifacts.
    q = rgb.quantize(colors=15, method=Image.Quantize.FASTOCTREE, dither=Image.Dither.NONE)
    qpal = q.getpalette()[:45]

    out = Image.new("P", src.size)
    flat_pal = [255, 0, 255] + qpal  # index 0 reserved for transparent key
    flat_pal.extend([0] * (768 - len(flat_pal)))
    out.putpalette(flat_pal)

    sp = src.load()
    qp = q.load()
    dp = out.load()

    for y in range(src.height):
        for x in range(src.width):
            r, g, b, a = sp[x, y]
            if a < 128 or (r, g, b) == (255, 0, 255):
                dp[x, y] = 0
            else:
                dp[x, y] = qp[x, y] + 1

    out_png.parent.mkdir(parents=True, exist_ok=True)
    out.save(out_png)


def write_png_palette_to_gbapal(png_path: Path, out_gbapal: Path) -> None:
    img = Image.open(png_path).convert("P")
    pal = img.getpalette()
    if pal is None:
        raise ValueError(f"PNG has no palette: {png_path}")

    raw = bytearray()
    for i in range(16):
        r, g, b = pal[i * 3 : i * 3 + 3]
        rr = min(31, round(r * 31 / 255))
        gg = min(31, round(g * 31 / 255))
        bb = min(31, round(b * 31 / 255))
        val = (bb << 10) | (gg << 5) | rr
        raw.append(val & 0xFF)
        raw.append((val >> 8) & 0xFF)

    out_gbapal.parent.mkdir(parents=True, exist_ok=True)
    out_gbapal.write_bytes(bytes(raw))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--source-anim",
        type=Path,
        default=Path("/mnt/c/Users/User/Desktop/pokemon_esmeralda/tiles/edificio_pokeball/edificio_pokeball_animacion_puertas2.png"),
    )
    parser.add_argument(
        "--tileset-palette",
        type=Path,
        default=Path("data/tilesets/secondary/edificio_pokeball/palettes/08.gbapal"),
    )
    parser.add_argument(
        "--out-anim-clean",
        type=Path,
        default=Path("/mnt/c/Users/User/Desktop/pokemon_esmeralda/tiles/edificio_pokeball/edificio_pokeball_animacion_puerta_fuente_limpia_2x2.png"),
    )
    parser.add_argument(
        "--out-door-clean",
        type=Path,
        default=Path("/mnt/c/Users/User/Desktop/pokemon_esmeralda/tiles/edificio_pokeball/edificio_pokeball_puerta_fuente_limpia_2x2.png"),
    )
    parser.add_argument(
        "--out-game-png",
        type=Path,
        default=Path("graphics/door_anims/edificio_pokeball.png"),
    )
    parser.add_argument(
        "--out-game-gbapal",
        type=Path,
        default=Path("graphics/door_anims/edificio_pokeball.gbapal"),
    )
    parser.add_argument(
        "--source-left",
        type=Path,
        default=None,
        help="Optional 16x96 left half. If set with --source-right, they are combined and used as source animation.",
    )
    parser.add_argument(
        "--source-right",
        type=Path,
        default=None,
        help="Optional 16x96 right half. If set with --source-left, they are combined and used as source animation.",
    )
    parser.add_argument(
        "--out-left-clean",
        type=Path,
        default=Path("/mnt/c/Users/User/Desktop/pokemon_esmeralda/tiles/edificio_pokeball/edificio_pokeball_animacion_puerta_izquierda_2x2.png"),
    )
    parser.add_argument(
        "--out-right-clean",
        type=Path,
        default=Path("/mnt/c/Users/User/Desktop/pokemon_esmeralda/tiles/edificio_pokeball/edificio_pokeball_animacion_puerta_derecha_2x2.png"),
    )
    parser.add_argument(
        "--palette-mode",
        choices=["source", "tileset"],
        default="source",
        help="source: derive stable 15-color palette from source PNG; tileset: force tileset palette file",
    )
    args = parser.parse_args()

    if (args.source_left is None) != (args.source_right is None):
        raise ValueError("Use both --source-left and --source-right together, or neither.")

    effective_source = args.source_anim
    if args.source_left is not None and args.source_right is not None:
        combine_left_right_pngs(args.source_left, args.source_right, args.source_anim)

    if not effective_source.exists():
        raise FileNotFoundError(f"Source animation not found: {effective_source}")
    if not args.tileset_palette.exists():
        raise FileNotFoundError(f"Tileset palette not found: {args.tileset_palette}")

    write_clean_pngs(effective_source, args.out_anim_clean, args.out_door_clean)
    split_left_right_pngs(effective_source, args.out_left_clean, args.out_right_clean)

    if args.palette_mode == "tileset":
        palette_colors = read_gbapal_colors(args.tileset_palette)
        map_to_fixed_palette(effective_source, palette_colors, args.out_game_png)
        args.out_game_gbapal.parent.mkdir(parents=True, exist_ok=True)
        args.out_game_gbapal.write_bytes(args.tileset_palette.read_bytes()[:32])
    else:
        map_to_source_palette(effective_source, args.out_game_png)
        write_png_palette_to_gbapal(args.out_game_png, args.out_game_gbapal)

    print(f"clean_anim={args.out_anim_clean}")
    print(f"clean_door={args.out_door_clean}")
    print(f"left_anim={args.out_left_clean}")
    print(f"right_anim={args.out_right_clean}")
    print(f"game_png={args.out_game_png}")
    print(f"game_gbapal={args.out_game_gbapal}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
