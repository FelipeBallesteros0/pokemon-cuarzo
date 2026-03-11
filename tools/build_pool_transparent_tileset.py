#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import struct
from pathlib import Path
from typing import Dict, List, Sequence, Set, Tuple

from PIL import Image

TILE_BYTES_4BPP = 32
FRAME_W = 128
FRAME_H = 128
TILES_PER_ROW = 16
METATILES_PER_ROW = 8


def bgr15_to_rgb(v: int) -> Tuple[int, int, int]:
    return ((v & 31) * 255 // 31, ((v >> 5) & 31) * 255 // 31, ((v >> 10) & 31) * 255 // 31)


def rgb_to_bgr15(rgb: Tuple[int, int, int]) -> int:
    r, g, b = rgb
    r5 = (r * 31 + 127) // 255
    g5 = (g * 31 + 127) // 255
    b5 = (b * 31 + 127) // 255
    return (b5 << 10) | (g5 << 5) | r5


def decode_4bpp_tile(tiles_blob: bytes, tile_idx: int) -> List[int]:
    data = tiles_blob[tile_idx * TILE_BYTES_4BPP : (tile_idx + 1) * TILE_BYTES_4BPP]
    out: List[int] = []
    for b in data:
        out.append(b & 0xF)
        out.append((b >> 4) & 0xF)
    return out


def encode_4bpp_tile(indices64: Sequence[int]) -> bytes:
    out = bytearray(TILE_BYTES_4BPP)
    j = 0
    for i in range(0, 64, 2):
        out[j] = (indices64[i] & 0xF) | ((indices64[i + 1] & 0xF) << 4)
        j += 1
    return bytes(out)


def render_metatile_rgb_block(
    tiles_blob: bytes,
    palettes: Sequence[Sequence[Tuple[int, int, int]]],
    metatile_words: Sequence[int],
) -> bytes:
    # words: tl,tr,bl,br,top_tl,top_tr,top_bl,top_br
    rgba = bytearray()
    for quad in range(4):
        w = metatile_words[quad]
        tile_id = w & 0x3FF
        pal_id = (w >> 12) & 0xF
        tile = decode_4bpp_tile(tiles_blob, tile_id)
        pal = palettes[pal_id]
        # 8x8 tile -> rows
        for py in range(8):
            for px in range(8):
                idx = tile[py * 8 + px]
                rgb = pal[idx]
                rgba.extend(rgb)
    # rearrange from tile order (tl,tr,bl,br) to row-major 16x16 rgb block bytes
    block = bytearray(16 * 16 * 3)
    # quad offsets in output
    qoff = [(0, 0), (8, 0), (0, 8), (8, 8)]
    cursor = 0
    for q, (ox, oy) in enumerate(qoff):
        for py in range(8):
            for px in range(8):
                src = cursor + (py * 8 + px) * 3
                dst = ((oy + py) * 16 + (ox + px)) * 3
                block[dst : dst + 3] = rgba[src : src + 3]
        cursor += 8 * 8 * 3
    return bytes(block)


def load_palettes_from_dir(pal_dir: Path) -> List[List[Tuple[int, int, int]]]:
    out: List[List[Tuple[int, int, int]]] = []
    for i in range(16):
        vals = struct.unpack("<16H", (pal_dir / f"{i:02d}.gbapal").read_bytes())
        out.append([bgr15_to_rgb(v) for v in vals])
    return out


def build_reference_metatile_rgb_set(
    tileset_root: Path,
    is_primary: bool,
) -> Set[bytes]:
    tiles = (tileset_root / "tiles.4bpp").read_bytes()
    pals = load_palettes_from_dir(tileset_root / "palettes")
    metatiles = (tileset_root / "metatiles.bin").read_bytes()
    count = len(metatiles) // 16
    out: Set[bytes] = set()
    for i in range(count):
        words = struct.unpack("<8H", metatiles[i * 16 : (i + 1) * 16])
        if is_primary:
            # primary tile ids are local 0..511
            words = tuple((w & ~0x3FF) | (w & 0x3FF) for w in words)
        else:
            # secondary stores global tile ids; convert to local by subtracting 0x200 for rendering from local tiles blob
            converted = []
            for w in words:
                tid = w & 0x3FF
                if tid >= 0x200:
                    tid -= 0x200
                converted.append((w & ~0x3FF) | tid)
            words = tuple(converted)
        out.add(render_metatile_rgb_block(tiles, pals, words))
    return out


def extract_grass_metatile_primary(primary_root: Path, metatile_id: int = 0x001) -> Image.Image:
    tiles = (primary_root / "tiles.4bpp").read_bytes()
    pals = load_palettes_from_dir(primary_root / "palettes")
    meta_blob = (primary_root / "metatiles.bin").read_bytes()
    words = struct.unpack("<8H", meta_blob[metatile_id * 16 : (metatile_id + 1) * 16])

    img = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
    # tl,tr,bl,br
    quad_pos = [(0, 0), (8, 0), (0, 8), (8, 8)]
    for q, (ox, oy) in enumerate(quad_pos):
        w = words[q]
        tid = w & 0x3FF
        pal_id = (w >> 12) & 0xF
        tile = decode_4bpp_tile(tiles, tid)
        pal = pals[pal_id]
        for py in range(8):
            for px in range(8):
                idx = tile[py * 8 + px]
                r, g, b = pal[idx]
                img.putpixel((ox + px, oy + py), (r, g, b, 255))
    return img


def compose_frames(overlay_sheet: Image.Image, grass16: Image.Image) -> List[Image.Image]:
    frames: List[Image.Image] = []
    for i in range(8):
        fx = (i % 4) * FRAME_W
        fy = (i // 4) * FRAME_H
        overlay = overlay_sheet.crop((fx, fy, fx + FRAME_W, fy + FRAME_H)).convert("RGBA")
        base = Image.new("RGBA", (FRAME_W, FRAME_H), (0, 0, 0, 0))
        for y in range(0, FRAME_H, 16):
            for x in range(0, FRAME_W, 16):
                base.alpha_composite(grass16, (x, y))
        base.alpha_composite(overlay, (0, 0))
        frames.append(base.convert("RGB"))
    return frames


def quantize_frames_to_reserved_palette(frames_rgb: Sequence[Image.Image]) -> Tuple[List[Image.Image], List[Tuple[int, int, int]]]:
    # Reserve index 0 as dummy; use 15 visible colors from frame 0.
    q0 = frames_rgb[0].quantize(colors=15, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    p = q0.getpalette()[:45]
    p += [0] * (45 - len(p))
    # Build fixed palette image with slot 0 dummy black, slots 1..15 from quantized colors.
    pal_flat = [0, 0, 0]
    for i in range(15):
        pal_flat += [p[i * 3], p[i * 3 + 1], p[i * 3 + 2]]
    pal_flat += [0] * (768 - len(pal_flat))
    pal_img = Image.new("P", (16, 16))
    pal_img.putpalette(pal_flat)

    out_frames: List[Image.Image] = []
    for fr in frames_rgb:
        q = fr.quantize(palette=pal_img, dither=Image.Dither.NONE)
        data = list(q.getdata())
        # Ensure no visible index 0 remains.
        changed = False
        for i, v in enumerate(data):
            if v == 0:
                data[i] = 1
                changed = True
        if changed:
            q.putdata(data)
        q.putpalette(pal_flat)
        out_frames.append(q)

    palette_rgb: List[Tuple[int, int, int]] = []
    for i in range(16):
        palette_rgb.append((pal_flat[i * 3], pal_flat[i * 3 + 1], pal_flat[i * 3 + 2]))
    return out_frames, palette_rgb


def frame_to_tiles_blob(frame_p: Image.Image) -> bytes:
    data = list(frame_p.getdata())
    out = bytearray()
    for ty in range(0, FRAME_H, 8):
        for tx in range(0, FRAME_W, 8):
            tile = []
            for y in range(8):
                row = (ty + y) * FRAME_W + tx
                tile.extend(data[row : row + 8])
            out.extend(encode_4bpp_tile(tile))
    return bytes(out)


def metatile_rgb_from_frame(frame_rgb: Image.Image, metatile_idx: int) -> bytes:
    mx = metatile_idx % METATILES_PER_ROW
    my = metatile_idx // METATILES_PER_ROW
    x0 = mx * 16
    y0 = my * 16
    block = frame_rgb.crop((x0, y0, x0 + 16, y0 + 16))
    return block.tobytes()


def build_metatiles_bin(kept_indices: Sequence[int], tile_base: int, palette_slot: int) -> bytes:
    out = bytearray()
    for mi in kept_indices:
        mx = mi % METATILES_PER_ROW
        my = mi // METATILES_PER_ROW
        # tile index in 16x16 grid of 8x8 tiles
        tl_local = (my * 2) * TILES_PER_ROW + (mx * 2)
        tr_local = tl_local + 1
        bl_local = tl_local + TILES_PER_ROW
        br_local = bl_local + 1
        words = [
            (tile_base + tl_local) | (palette_slot << 12),
            (tile_base + tr_local) | (palette_slot << 12),
            (tile_base + bl_local) | (palette_slot << 12),
            (tile_base + br_local) | (palette_slot << 12),
            0,
            0,
            0,
            0,
        ]
        out.extend(struct.pack("<8H", *words))
    return bytes(out)


def write_palette_files(dst_pal_dir: Path, palette_rgb: Sequence[Tuple[int, int, int]], slot: int) -> None:
    dst_pal_dir.mkdir(parents=True, exist_ok=True)
    # write all 16 slots as zeros first, then set selected slot
    zero = struct.pack("<16H", *([0] * 16))
    for i in range(16):
        (dst_pal_dir / f"{i:02d}.gbapal").write_bytes(zero)
        (dst_pal_dir / f"{i:02d}.pal").write_text("JASC-PAL\n0100\n16\n" + "\n".join(["0 0 0"] * 16) + "\n")
    vals = [rgb_to_bgr15(c) for c in palette_rgb]
    (dst_pal_dir / f"{slot:02d}.gbapal").write_bytes(struct.pack("<16H", *vals))
    lines = ["JASC-PAL", "0100", "16"] + [f"{r} {g} {b}" for (r, g, b) in palette_rgb]
    (dst_pal_dir / f"{slot:02d}.pal").write_text("\n".join(lines) + "\n")


def main() -> None:
    ap = argparse.ArgumentParser(description="Build animated pool tileset from transparent overlay + grass background.")
    ap.add_argument("--input", required=True, type=Path, help="Transparent overlay sheet (expected 512x256, 8 frames)")
    ap.add_argument("--output", required=True, type=Path, help="Output secondary tileset directory")
    ap.add_argument("--primary-root", type=Path, default=Path("data/tilesets/primary/general"))
    ap.add_argument("--secondary-ref", type=Path, default=Path("data/tilesets/secondary/pueblo_ciendra_pool"))
    ap.add_argument("--grass-metatile", type=lambda x: int(x, 0), default=0x001)
    ap.add_argument("--tile-base", type=lambda x: int(x, 0), default=0x200)
    ap.add_argument("--palette-slot", type=lambda x: int(x, 0), default=6)
    args = ap.parse_args()

    overlay = Image.open(args.input).convert("RGBA")
    if overlay.size != (512, 256):
        raise SystemExit(f"expected input size 512x256, got {overlay.size}")

    grass = extract_grass_metatile_primary(args.primary_root, args.grass_metatile)
    frames_rgb = compose_frames(overlay, grass)
    frames_p, palette_rgb = quantize_frames_to_reserved_palette(frames_rgb)

    # Build reference metatile rgb set
    ref = set()
    ref |= build_reference_metatile_rgb_set(args.primary_root, is_primary=True)
    if args.secondary_ref.exists():
        ref |= build_reference_metatile_rgb_set(args.secondary_ref, is_primary=False)

    # Keep only unique metatiles not present in references
    kept: List[int] = []
    for mi in range(64):
        block = metatile_rgb_from_frame(frames_rgb[0], mi)
        if block not in ref:
            kept.append(mi)

    out_root = args.output
    (out_root / "anim" / "pool").mkdir(parents=True, exist_ok=True)
    (out_root / "palettes").mkdir(parents=True, exist_ok=True)

    # tiles.4bpp + tiles.png from frame 0
    tiles_blob = frame_to_tiles_blob(frames_p[0])
    (out_root / "tiles.4bpp").write_bytes(tiles_blob)
    frames_p[0].save(out_root / "tiles.png")

    # anim frames keep full 256 tiles to remain compatible with existing callback sizes
    for i, fr in enumerate(frames_p):
        (out_root / "anim" / "pool" / f"{i}.4bpp").write_bytes(frame_to_tiles_blob(fr))

    # metatiles from kept indices
    (out_root / "metatiles.bin").write_bytes(build_metatiles_bin(kept, args.tile_base, args.palette_slot))
    (out_root / "metatile_attributes.bin").write_bytes(struct.pack(f"<{len(kept)}H", *([0] * len(kept))))
    write_palette_files(out_root / "palettes", palette_rgb, args.palette_slot)

    report = {
        "input": str(args.input),
        "output": str(args.output),
        "kept_metatiles": len(kept),
        "dropped_metatiles": 64 - len(kept),
        "kept_indices": kept,
        "palette_slot": args.palette_slot,
        "tile_base": args.tile_base,
    }
    (out_root / "build_report.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
