#!/usr/bin/env python3
"""
Deterministic PNG -> tiles/metatiles converter for pokeemerald-expansion workflows.

Features:
- Deterministic ordering with two modes:
  - keep-order: preserve every 8x8 tile in scanline order (no dedup)
  - dedup: stable dedup for 8x8 tiles and 16x16 metatiles (first occurrence wins)
- Exports:
  - tiles.4bpp
  - metatiles.bin
  - metatile_attributes.bin
  - tiles.png (tilesheet preview)
  - report.json
- Optional direct apply to an existing secondary tileset directory.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Sequence, Tuple

from PIL import Image

TILE_SIZE = 8
METATILE_SIZE = 16
MAX_COLORS = 16
MAX_TILE_ID = 0x3FF  # 10-bit tile index in metatile words


@dataclass(frozen=True)
class BuildConfig:
    mode: str
    tile_base: int
    palette_slot: int
    attr: int
    metatile_base: int


@dataclass(frozen=True)
class AnimConfig:
    enabled: bool
    frames_x: int
    frames_y: int
    frame_w: int
    frame_h: int
    anim_name: str


def fail(msg: str) -> None:
    print(f"error: {msg}", file=sys.stderr)
    raise SystemExit(1)


def ensure_indexed_png(path: Path, quantize: bool) -> Image.Image:
    img = Image.open(path)
    if img.mode != "P":
        if not quantize:
            fail(
                f"{path} is mode '{img.mode}', expected indexed PNG mode 'P'. "
                "Use --quantize to force conversion."
            )
        img = img.convert("RGB").quantize(colors=MAX_COLORS, dither=Image.Dither.NONE)
    if img.width % METATILE_SIZE != 0 or img.height % METATILE_SIZE != 0:
        fail(
            f"image size must be divisible by {METATILE_SIZE}. "
            f"Got {img.width}x{img.height}"
        )
    used = set(img.tobytes())
    if len(used) > MAX_COLORS:
        fail(f"image uses {len(used)} color indices; max allowed is {MAX_COLORS}")
    return img


def ensure_indexed_image(img: Image.Image, quantize: bool) -> Image.Image:
    if img.mode != "P":
        if not quantize:
            fail(
                f"input image mode is '{img.mode}', expected indexed mode 'P'. "
                "Use --quantize to force conversion."
            )
        img = img.convert("RGB").quantize(colors=MAX_COLORS, dither=Image.Dither.NONE)
    if img.width % METATILE_SIZE != 0 or img.height % METATILE_SIZE != 0:
        fail(
            f"image size must be divisible by {METATILE_SIZE}. "
            f"Got {img.width}x{img.height}"
        )
    used = set(img.tobytes())
    if len(used) > MAX_COLORS:
        fail(f"image uses {len(used)} color indices; max allowed is {MAX_COLORS}")
    return img


def composite_with_bg_tile(fg_path: Path, bg_tile_path: Path) -> Image.Image:
    fg = Image.open(fg_path).convert("RGBA")
    bg_tile = Image.open(bg_tile_path).convert("RGBA")
    if bg_tile.size != (16, 16):
        fail(f"background metatile PNG must be 16x16, got {bg_tile.size}")
    bg = Image.new("RGBA", fg.size, (0, 0, 0, 0))
    for y in range(0, fg.height, 16):
        for x in range(0, fg.width, 16):
            bg.alpha_composite(bg_tile, (x, y))
    bg.alpha_composite(fg, (0, 0))
    return bg


def _parse_jasc_pal(path: Path) -> List[Tuple[int, int, int]]:
    lines = [ln.strip() for ln in path.read_text(encoding="utf-8").splitlines() if ln.strip()]
    if len(lines) < 3 or lines[0] != "JASC-PAL":
        fail(f"invalid JASC-PAL file: {path}")
    try:
        count = int(lines[2])
    except ValueError:
        fail(f"invalid JASC-PAL color count: {path}")
    colors: List[Tuple[int, int, int]] = []
    for ln in lines[3 : 3 + count]:
        parts = ln.split()
        if len(parts) != 3:
            fail(f"invalid JASC-PAL color line in {path}: {ln}")
        r, g, b = (int(parts[0]), int(parts[1]), int(parts[2]))
        colors.append((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b))))
    if not colors:
        fail(f"empty JASC-PAL file: {path}")
    return colors[:16]


def _parse_gbapal(path: Path) -> List[Tuple[int, int, int]]:
    data = path.read_bytes()
    if len(data) < 32:
        fail(f"invalid gbapal size (expected >=32 bytes): {path}")
    colors: List[Tuple[int, int, int]] = []
    for i in range(16):
        v = int.from_bytes(data[i * 2 : i * 2 + 2], "little")
        r = (v & 0x1F) * 255 // 31
        g = ((v >> 5) & 0x1F) * 255 // 31
        b = ((v >> 10) & 0x1F) * 255 // 31
        colors.append((r, g, b))
    return colors


def _load_palette_sidecar(img_path: Path) -> List[Tuple[int, int, int]] | None:
    pal = img_path.with_suffix(".pal")
    gbapal = img_path.with_suffix(".gbapal")
    if pal.exists():
        return _parse_jasc_pal(pal)
    if gbapal.exists():
        return _parse_gbapal(gbapal)
    return None


def _load_bg_tile_for_preview(bg_tile_path: Path) -> Image.Image:
    bg = Image.open(bg_tile_path)
    sidecar = _load_palette_sidecar(bg_tile_path)
    if sidecar and bg.mode == "P":
        raw = []
        for r, g, b in sidecar[:16]:
            raw.extend([r, g, b])
        raw += [0] * (768 - len(raw))
        bg = bg.copy()
        bg.putpalette(raw)
    return bg.convert("RGBA")


def composite_with_bg_tile_preview(fg_path: Path, bg_tile_path: Path) -> Image.Image:
    fg = Image.open(fg_path).convert("RGBA")
    bg_tile = _load_bg_tile_for_preview(bg_tile_path)
    if bg_tile.size != (16, 16):
        fail(f"background metatile PNG must be 16x16, got {bg_tile.size}")
    bg = Image.new("RGBA", fg.size, (0, 0, 0, 0))
    for y in range(0, fg.height, 16):
        for x in range(0, fg.width, 16):
            bg.alpha_composite(bg_tile, (x, y))
    bg.alpha_composite(fg, (0, 0))
    return bg


def image_has_alpha(img: Image.Image) -> bool:
    if img.mode in ("RGBA", "LA"):
        return True
    return img.mode == "P" and "transparency" in img.info


def apply_transparent_key(img: Image.Image, key_rgb: Tuple[int, int, int]) -> Image.Image:
    rgba = img.convert("RGBA")
    src = list(rgba.getdata())
    out: List[Tuple[int, int, int]] = []
    for r, g, b, a in src:
        if a == 0:
            out.append(key_rgb)
        else:
            out.append((r, g, b))
    rgb = Image.new("RGB", rgba.size)
    rgb.putdata(out)
    return rgb


def frames_rgb_to_exact_indexed(
    frames_rgb: Sequence[Image.Image],
    reserve_index0: Tuple[int, int, int] | None = None,
) -> Tuple[List[Image.Image], List[Tuple[int, int, int]]]:
    palette: List[Tuple[int, int, int]] = []
    color_to_idx: Dict[Tuple[int, int, int], int] = {}

    if reserve_index0 is not None:
        palette.append(reserve_index0)
        color_to_idx[reserve_index0] = 0

    # Stable first-seen ordering across frames/pixels.
    for fr in frames_rgb:
        for c in fr.getdata():
            if c not in color_to_idx:
                if len(palette) >= 16:
                    fail(
                        "lock-input-palette: combined frames use more than 16 colors "
                        f"({len(palette) + 1}). Reduce colors in source image."
                    )
                color_to_idx[c] = len(palette)
                palette.append(c)

    if len(palette) < 16:
        palette += [(0, 0, 0)] * (16 - len(palette))

    pal_flat: List[int] = []
    for r, g, b in palette[:16]:
        pal_flat.extend([r, g, b])
    pal_flat += [0] * (768 - len(pal_flat))

    out_frames: List[Image.Image] = []
    for fr in frames_rgb:
        idx_data = [color_to_idx[c] for c in fr.getdata()]
        p = Image.new("P", fr.size)
        p.putdata(idx_data)
        p.putpalette(pal_flat)
        out_frames.append(p)

    return out_frames, palette[:16]


def get_palette_rgb(img: Image.Image) -> List[Tuple[int, int, int]]:
    raw = img.getpalette() or []
    raw = raw[: 3 * MAX_COLORS] + [0] * max(0, 3 * MAX_COLORS - len(raw))
    out: List[Tuple[int, int, int]] = []
    for i in range(MAX_COLORS):
        out.append((raw[i * 3], raw[i * 3 + 1], raw[i * 3 + 2]))
    return out


def rgb_to_bgr15(rgb: Tuple[int, int, int]) -> int:
    r, g, b = rgb
    r5 = (r * 31 + 127) // 255
    g5 = (g * 31 + 127) // 255
    b5 = (b * 31 + 127) // 255
    return (b5 << 10) | (g5 << 5) | r5


def gba_palette_bytes(palette_rgb: Sequence[Tuple[int, int, int]]) -> bytes:
    vals = [rgb_to_bgr15(c) for c in palette_rgb[:MAX_COLORS]]
    vals += [0] * (MAX_COLORS - len(vals))
    return struct.pack("<16H", *vals)


def jasc_pal_text(palette_rgb: Sequence[Tuple[int, int, int]]) -> str:
    lines = ["JASC-PAL", "0100", str(MAX_COLORS)]
    for i in range(MAX_COLORS):
        r, g, b = palette_rgb[i]
        lines.append(f"{r} {g} {b}")
    return "\n".join(lines) + "\n"


def crop_tile(img: Image.Image, x: int, y: int) -> bytes:
    tile = img.crop((x, y, x + TILE_SIZE, y + TILE_SIZE))
    return bytes(tile.getdata())


def encode_tile_4bpp(tile_idx_bytes: bytes) -> bytes:
    # Tile data is row-major indexes (64 bytes). Pack two 4-bit pixels per byte.
    if len(tile_idx_bytes) != 64:
        fail(f"internal error: tile byte length is {len(tile_idx_bytes)}, expected 64")
    out = bytearray(32)
    j = 0
    for i in range(0, 64, 2):
        a = tile_idx_bytes[i] & 0xF
        b = tile_idx_bytes[i + 1] & 0xF
        out[j] = a | (b << 4)
        j += 1
    return bytes(out)


def pack_metatile_word(tile_id: int, palette_slot: int) -> int:
    if tile_id < 0 or tile_id > MAX_TILE_ID:
        fail(f"tile id out of range for metatile word: {tile_id:#x}")
    if palette_slot < 0 or palette_slot > 0xF:
        fail(f"palette slot must be 0..15, got {palette_slot}")
    # hflip/vflip = 0, priority handled elsewhere
    return tile_id | (palette_slot << 12)


def tile_hash(tile_idx_bytes: bytes) -> str:
    return hashlib.sha1(tile_idx_bytes).hexdigest()


def metatile_hash(tile_ids_local: Tuple[int, int, int, int]) -> str:
    return hashlib.sha1(struct.pack("<4H", *tile_ids_local)).hexdigest()


def build_tiles_and_metatiles_with_layout(
    img: Image.Image,
    cfg: BuildConfig,
    frames_for_dedup: Sequence[Image.Image] | None = None,
) -> Tuple[
    List[bytes],
    List[Tuple[int, int, int, int]],
    int,
    int,
    List[Tuple[int, int]],
    List[List[Tuple[int, int]]],
]:
    # Extract 8x8 tiles in deterministic scan order from base frame.
    all_tiles: List[bytes] = []
    tile_coords: List[Tuple[int, int]] = []
    for y in range(0, img.height, TILE_SIZE):
        for x in range(0, img.width, TILE_SIZE):
            all_tiles.append(crop_tile(img, x, y))
            tile_coords.append((x, y))

    tile_map: Dict[str, int] = {}
    unique_tiles: List[bytes] = []
    tile_index_by_pos: List[int] = []

    first_pos_by_unique: List[Tuple[int, int]] = []
    positions_by_unique: List[List[Tuple[int, int]]] = []
    if cfg.mode == "keep-order":
        unique_tiles = all_tiles[:]
        tile_index_by_pos = list(range(len(all_tiles)))
        first_pos_by_unique = tile_coords[:]
        positions_by_unique = [[xy] for xy in tile_coords]
    else:
        # For animated sheets, dedup must consider all frames to avoid
        # collapsing tiles that diverge later and causing bad animation frames.
        if frames_for_dedup:
            for fr in frames_for_dedup:
                if fr.size != img.size:
                    fail(
                        "internal error: frames_for_dedup size mismatch "
                        f"(expected {img.size}, got {fr.size})"
                    )
        for i, t in enumerate(all_tiles):
            if frames_for_dedup:
                x, y = tile_coords[i]
                sig = tuple(crop_tile(fr, x, y) for fr in frames_for_dedup)
                h = hashlib.sha1(b"".join(sig)).hexdigest()
            else:
                h = tile_hash(t)
            if h not in tile_map:
                tile_map[h] = len(unique_tiles)
                unique_tiles.append(t)
                first_pos_by_unique.append(tile_coords[i])
                positions_by_unique.append([tile_coords[i]])
            tile_index_by_pos.append(tile_map[h])
            positions_by_unique[tile_map[h]].append(tile_coords[i])

    tiles_per_row = img.width // TILE_SIZE
    metatiles_all: List[Tuple[int, int, int, int]] = []
    for my in range(0, img.height // METATILE_SIZE):
        for mx in range(0, img.width // METATILE_SIZE):
            tx = mx * 2
            ty = my * 2
            tl = tile_index_by_pos[(ty * tiles_per_row) + tx]
            tr = tile_index_by_pos[(ty * tiles_per_row) + tx + 1]
            bl = tile_index_by_pos[((ty + 1) * tiles_per_row) + tx]
            br = tile_index_by_pos[((ty + 1) * tiles_per_row) + tx + 1]
            metatiles_all.append((tl, tr, bl, br))

    if cfg.mode == "dedup":
        metatile_map: Dict[str, int] = {}
        unique_metatiles: List[Tuple[int, int, int, int]] = []
        for mt in metatiles_all:
            h = metatile_hash(mt)
            if h not in metatile_map:
                metatile_map[h] = len(unique_metatiles)
                unique_metatiles.append(mt)
        metatiles_all = unique_metatiles

    return (
        unique_tiles,
        metatiles_all,
        img.width // METATILE_SIZE,
        img.height // METATILE_SIZE,
        first_pos_by_unique,
        positions_by_unique,
    )


def build_tiles_and_metatiles(
    img: Image.Image, cfg: BuildConfig
) -> Tuple[List[bytes], List[Tuple[int, int, int, int]], int, int]:
    tiles, metatiles, mt_w, mt_h, _, _ = build_tiles_and_metatiles_with_layout(img, cfg)
    return tiles, metatiles, mt_w, mt_h


def frame_tiles_from_layout(frame: Image.Image, first_pos_by_unique: Sequence[Tuple[int, int]]) -> List[bytes]:
    return [crop_tile(frame, x, y) for (x, y) in first_pos_by_unique]


def validate_anim_layout(
    base_tiles: Sequence[bytes],
    frames: Sequence[Image.Image],
    first_pos_by_unique: Sequence[Tuple[int, int]],
    enabled: bool,
) -> None:
    if not enabled:
        return
    if len(first_pos_by_unique) != len(base_tiles):
        fail(
            "validation failed: layout tile count mismatch "
            f"(layout={len(first_pos_by_unique)} base={len(base_tiles)})"
        )
    frame0_tiles = frame_tiles_from_layout(frames[0], first_pos_by_unique)
    if frame0_tiles != list(base_tiles):
        fail(
            "validation failed: animation tile order mismatch against base frame. "
            "This would produce wrong tiles in-game."
        )


def validate_dedup_anim_consistency(
    frames: Sequence[Image.Image],
    positions_by_unique: Sequence[Sequence[Tuple[int, int]]],
    enabled: bool,
    mode: str,
) -> None:
    if not enabled or mode != "dedup":
        return
    issues: List[str] = []
    for frame_idx, fr in enumerate(frames):
        for uidx, group in enumerate(positions_by_unique):
            if len(group) <= 1:
                continue
            x0, y0 = group[0]
            ref = crop_tile(fr, x0, y0)
            for x, y in group[1:]:
                if crop_tile(fr, x, y) != ref:
                    issues.append(
                        f"frame={frame_idx}, dedup_tile={uidx}, posA=({x0},{y0}), posB=({x},{y})"
                    )
    if issues:
        preview = "\n".join(f"  - {s}" for s in issues[:20])
        extra = "" if len(issues) <= 20 else f"\n  ... y {len(issues) - 20} conflictos mas"
        fail(
            "animation validation failed: dedup collapsed tiles that diverge across animation frames.\n"
            f"Conflictos detectados: {len(issues)}\n"
            f"{preview}{extra}\n"
            "Use mode=keep-order for this animated sheet."
        )


def split_animation_frames(img: Image.Image, anim_cfg: AnimConfig) -> List[Image.Image]:
    if not anim_cfg.enabled:
        return [img]
    expected_w = anim_cfg.frames_x * anim_cfg.frame_w
    expected_h = anim_cfg.frames_y * anim_cfg.frame_h
    if img.width != expected_w or img.height != expected_h:
        fail(
            "image size does not match animation grid: "
            f"expected {expected_w}x{expected_h} from "
            f"{anim_cfg.frames_x}x{anim_cfg.frames_y} frames of "
            f"{anim_cfg.frame_w}x{anim_cfg.frame_h}, got {img.width}x{img.height}"
        )
    frames: List[Image.Image] = []
    for fy in range(anim_cfg.frames_y):
        for fx in range(anim_cfg.frames_x):
            x = fx * anim_cfg.frame_w
            y = fy * anim_cfg.frame_h
            frames.append(img.crop((x, y, x + anim_cfg.frame_w, y + anim_cfg.frame_h)))
    return frames


def build_preview_source_image(
    input_path: Path,
    bg_metatile_png: Path | None,
) -> Image.Image:
    src = Image.open(input_path)
    if bg_metatile_png is None:
        return src.convert("RGBA")
    if not bg_metatile_png.exists():
        fail(f"--bg-metatile-png not found: {bg_metatile_png}")
    return composite_with_bg_tile_preview(input_path, bg_metatile_png).convert("RGBA")


def remap_visible_index_zero(
    frames: Sequence[Image.Image],
    palette_rgb: Sequence[Tuple[int, int, int]],
) -> Tuple[List[Image.Image], List[Tuple[int, int, int]], int, int]:
    # In BG tiles, palette index 0 is transparent. This remaps any visible index-0
    # pixels to a non-zero slot while preserving color via palette relocation.
    used = set()
    for f in frames:
        used.update(f.getdata())
    if 0 not in used:
        return list(frames), list(palette_rgb), 0, 0

    free = [i for i in range(1, 16) if i not in used]
    target = free[0] if free else 15

    remapped_frames: List[Image.Image] = []
    changed_px = 0
    for f in frames:
        g = f.copy()
        data = list(g.getdata())
        frame_changes = 0
        for i, px in enumerate(data):
            if px == 0:
                data[i] = target
                frame_changes += 1
        if frame_changes:
            g.putdata(data)
            changed_px += frame_changes
        remapped_frames.append(g)

    pal = list(palette_rgb[:16])
    if len(pal) < 16:
        pal += [(0, 0, 0)] * (16 - len(pal))
    old0 = pal[0]
    pal[target] = old0
    pal[0] = pal[1]
    return remapped_frames, pal, target, changed_px


def tiles_to_4bpp_blob(tiles: Sequence[bytes]) -> bytes:
    out = bytearray()
    for tile in tiles:
        out.extend(encode_tile_4bpp(tile))
    return bytes(out)


def tilesheet_png(tiles: Sequence[bytes], palette_rgb: Sequence[Tuple[int, int, int]]) -> Image.Image:
    if not tiles:
        return Image.new("P", (8, 8))
    cols = 16
    rows = math.ceil(len(tiles) / cols)
    img = Image.new("P", (cols * 8, rows * 8))
    flat_pal: List[int] = []
    for r, g, b in palette_rgb[:MAX_COLORS]:
        flat_pal.extend([r, g, b])
    flat_pal += [0] * (768 - len(flat_pal))
    img.putpalette(flat_pal)
    for i, t in enumerate(tiles):
        tx = (i % cols) * 8
        ty = (i // cols) * 8
        tile_img = Image.frombytes("P", (8, 8), t)
        tile_img.putpalette(flat_pal)
        img.paste(tile_img, (tx, ty))
    return img


def metatiles_bin(
    metatiles: Sequence[Tuple[int, int, int, int]],
    palette_slot: int,
    tile_base: int,
) -> bytes:
    out = bytearray()
    for tl_l, tr_l, bl_l, br_l in metatiles:
        tl = tile_base + tl_l
        tr = tile_base + tr_l
        bl = tile_base + bl_l
        br = tile_base + br_l
        words = [
            pack_metatile_word(tl, palette_slot),
            pack_metatile_word(tr, palette_slot),
            pack_metatile_word(bl, palette_slot),
            pack_metatile_word(br, palette_slot),
            0,
            0,
            0,
            0,
        ]
        out.extend(struct.pack("<8H", *words))
    return bytes(out)


def metatile_attributes_bin(count: int, attr: int) -> bytes:
    if attr < 0 or attr > 0xFFFF:
        fail(f"metatile attribute must be 0..0xFFFF, got {attr:#x}")
    return struct.pack(f"<{count}H", *([attr] * count))


def write_files(
    out_dir: Path,
    tiles_blob: bytes,
    metatiles_blob: bytes,
    attrs_blob: bytes,
    tiles_png: Image.Image,
    palette_rgb: Sequence[Tuple[int, int, int]],
    report: dict,
) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "tiles.4bpp").write_bytes(tiles_blob)
    (out_dir / "metatiles.bin").write_bytes(metatiles_blob)
    (out_dir / "metatile_attributes.bin").write_bytes(attrs_blob)
    tiles_png.save(out_dir / "tiles.png")
    (out_dir / "palette.gbapal").write_bytes(gba_palette_bytes(palette_rgb))
    (out_dir / "palette.pal").write_text(jasc_pal_text(palette_rgb), encoding="utf-8")
    (out_dir / "report.json").write_text(
        json.dumps(report, ensure_ascii=True, indent=2) + "\n", encoding="utf-8"
    )


def write_anim_frames(
    out_dir: Path,
    frame_tiles: Sequence[Sequence[bytes]],
    preview_frames: Sequence[Image.Image],
    anim_cfg: AnimConfig,
) -> None:
    if not anim_cfg.enabled:
        return
    anim_dir = out_dir / "anim" / anim_cfg.anim_name
    anim_dir.mkdir(parents=True, exist_ok=True)
    preview_dir = out_dir / "anim_preview" / anim_cfg.anim_name
    preview_dir.mkdir(parents=True, exist_ok=True)
    for i, tiles in enumerate(frame_tiles):
        (anim_dir / f"{i}.4bpp").write_bytes(tiles_to_4bpp_blob(tiles))
    for i, fr in enumerate(preview_frames):
        fr.save(preview_dir / f"{i}.png")


def apply_to_tileset(
    apply_dir: Path,
    tiles_blob: bytes,
    metatiles_blob: bytes,
    attrs_blob: bytes,
    tiles_png: Image.Image,
    palette_rgb: Sequence[Tuple[int, int, int]],
    palette_slot: int,
    write_palette: bool,
) -> None:
    if not apply_dir.exists():
        fail(f"--apply-to directory does not exist: {apply_dir}")
    (apply_dir / "tiles.4bpp").write_bytes(tiles_blob)
    (apply_dir / "metatiles.bin").write_bytes(metatiles_blob)
    (apply_dir / "metatile_attributes.bin").write_bytes(attrs_blob)
    tiles_png.save(apply_dir / "tiles.png")
    if write_palette:
        pal_dir = apply_dir / "palettes"
        pal_dir.mkdir(parents=True, exist_ok=True)
        slot = f"{palette_slot:02d}"
        (pal_dir / f"{slot}.gbapal").write_bytes(gba_palette_bytes(palette_rgb))
        (pal_dir / f"{slot}.pal").write_text(jasc_pal_text(palette_rgb), encoding="utf-8")


def apply_anim_to_tileset(
    apply_dir: Path,
    frame_tiles: Sequence[Sequence[bytes]],
    anim_cfg: AnimConfig,
) -> None:
    if not anim_cfg.enabled:
        return
    anim_dir = apply_dir / "anim" / anim_cfg.anim_name
    anim_dir.mkdir(parents=True, exist_ok=True)
    for i, tiles in enumerate(frame_tiles):
        (anim_dir / f"{i}.4bpp").write_bytes(tiles_to_4bpp_blob(tiles))


def parse_int(value: str) -> int:
    return int(value, 0)


def main() -> None:
    parser = argparse.ArgumentParser(description="Deterministic PNG -> tiles/metatiles tool")
    parser.add_argument("--input", required=True, type=Path, help="Indexed PNG input")
    parser.add_argument(
        "--out-dir",
        type=Path,
        default=Path("build/img2metatiles_out"),
        help="Output folder for generated artifacts",
    )
    parser.add_argument(
        "--mode",
        choices=["keep-order", "dedup"],
        default="keep-order",
        help="Deterministic conversion mode",
    )
    parser.add_argument(
        "--tile-base",
        type=parse_int,
        default=0x200,
        help="Tile index base written into metatiles.bin (default: 0x200)",
    )
    parser.add_argument(
        "--palette-slot",
        type=parse_int,
        default=6,
        help="Palette slot for generated metatile words (0..15)",
    )
    parser.add_argument(
        "--metatile-attr",
        type=parse_int,
        default=0x0000,
        help="Attribute value for each generated metatile entry",
    )
    parser.add_argument(
        "--apply-to",
        type=Path,
        default=None,
        help="Apply generated files directly to a tileset directory",
    )
    parser.add_argument(
        "--write-palette",
        action="store_true",
        help="When using --apply-to, also write palettes/<slot>.gbapal and .pal",
    )
    parser.add_argument(
        "--quantize",
        action="store_true",
        help="Force quantization to indexed 16-color image if input is not mode 'P'",
    )
    parser.add_argument(
        "--bg-metatile-png",
        type=Path,
        default=None,
        help="Optional 16x16 PNG tiled as background behind transparent input pixels",
    )
    parser.add_argument(
        "--transparent-key",
        choices=["magenta", "none"],
        default="magenta",
        help="When input has alpha and no background is provided, reserve index 0 for this key color",
    )
    parser.add_argument(
        "--anim",
        action="store_true",
        help="Treat input as animation sheet and export anim frames",
    )
    parser.add_argument("--frames-x", type=int, default=4, help="Animation frames per row")
    parser.add_argument("--frames-y", type=int, default=2, help="Animation frame rows")
    parser.add_argument(
        "--frame-width",
        type=int,
        default=128,
        help="Width of each animation frame in pixels",
    )
    parser.add_argument(
        "--frame-height",
        type=int,
        default=128,
        help="Height of each animation frame in pixels",
    )
    parser.add_argument(
        "--anim-name",
        default="pool",
        help="Animation folder name (written to anim/<name>/N.4bpp)",
    )
    parser.add_argument(
        "--avoid-index-zero",
        action=argparse.BooleanOptionalAction,
        default=True,
        help="Remap visible palette index 0 to a non-zero slot (default: enabled)",
    )
    parser.add_argument(
        "--lock-input-palette",
        action=argparse.BooleanOptionalAction,
        default=False,
        help="Preserve exact input RGB colors (no palette re-optimization/quantize remap)",
    )
    args = parser.parse_args()

    src_img = Image.open(args.input)
    reserved_key_rgb = None
    if args.bg_metatile_png is not None:
        if not args.bg_metatile_png.exists():
            fail(f"--bg-metatile-png not found: {args.bg_metatile_png}")
        composed = composite_with_bg_tile(args.input, args.bg_metatile_png)
        img = ensure_indexed_image(composed, quantize=args.quantize)
    elif args.transparent_key != "none" and image_has_alpha(src_img):
        reserved_key_rgb = (255, 0, 255)
        keyed = apply_transparent_key(src_img, reserved_key_rgb)
        # Keyed input must be quantized with reserved index handling below.
        img = ensure_indexed_image(keyed, quantize=True)
    else:
        img = ensure_indexed_png(args.input, quantize=args.quantize)
    anim_cfg = AnimConfig(
        enabled=args.anim,
        frames_x=args.frames_x,
        frames_y=args.frames_y,
        frame_w=args.frame_width,
        frame_h=args.frame_height,
        anim_name=args.anim_name,
    )
    preview_source_img = build_preview_source_image(args.input, args.bg_metatile_png)
    preview_source_frames = split_animation_frames(preview_source_img, anim_cfg)
    frames = split_animation_frames(img, anim_cfg)
    base_img = frames[0]
    remap_target = None
    remap_pixels = 0
    if args.lock_input_palette:
        rgb_frames = [fr.convert("RGB") for fr in frames]
        frames, palette_rgb = frames_rgb_to_exact_indexed(rgb_frames, reserve_index0=reserved_key_rgb)
        base_img = frames[0]
    elif reserved_key_rgb is not None:
        # Reserve palette index 0 for transparent key color deterministically.
        f0_15 = base_img.convert("RGB").quantize(colors=15, dither=Image.Dither.NONE)
        p15 = (f0_15.getpalette() or [])[:45]
        p15 += [0] * (45 - len(p15))
        pal_flat = [reserved_key_rgb[0], reserved_key_rgb[1], reserved_key_rgb[2]] + p15
        pal_flat += [0] * (768 - len(pal_flat))
        pal_img = Image.new("P", (16, 16))
        pal_img.putpalette(pal_flat)

        keyed_frames: List[Image.Image] = []
        for fr in frames:
            rgb = fr.convert("RGB")
            q = rgb.quantize(palette=pal_img, dither=Image.Dither.NONE)
            rgb_data = list(rgb.getdata())
            idx_data = list(q.getdata())
            changed = False
            for i, (c, idx) in enumerate(zip(rgb_data, idx_data)):
                if c == reserved_key_rgb:
                    if idx != 0:
                        idx_data[i] = 0
                        changed = True
                elif idx == 0:
                    idx_data[i] = 1
                    changed = True
            if changed:
                q.putdata(idx_data)
            q.putpalette(pal_flat)
            keyed_frames.append(q)
        frames = keyed_frames
        base_img = frames[0]
        palette_rgb = [(pal_flat[i * 3], pal_flat[i * 3 + 1], pal_flat[i * 3 + 2]) for i in range(16)]
    else:
        palette_rgb = get_palette_rgb(base_img)
    # If no transparent key is reserved, visible index 0 must be avoided
    # to prevent full-tile transparency in BG maps (e.g. grass background).
    if args.avoid_index_zero and reserved_key_rgb is None:
        frames, palette_rgb, remap_target, remap_pixels = remap_visible_index_zero(frames, palette_rgb)
        base_img = frames[0]
    cfg = BuildConfig(
        mode=args.mode,
        tile_base=args.tile_base,
        palette_slot=args.palette_slot,
        attr=args.metatile_attr,
        metatile_base=0,
    )

    tiles, metatiles, mt_w, mt_h, first_pos_by_unique, positions_by_unique = build_tiles_and_metatiles_with_layout(
        base_img,
        cfg,
        frames_for_dedup=frames if anim_cfg.enabled else None,
    )
    if cfg.tile_base + len(tiles) - 1 > MAX_TILE_ID:
        fail(
            "tile range exceeds 10-bit tile id limit in metatile words: "
            f"base={cfg.tile_base:#x}, count={len(tiles)}"
        )

    anim_frame_tiles = [frame_tiles_from_layout(fr, first_pos_by_unique) for fr in frames]
    validate_anim_layout(tiles, frames, first_pos_by_unique, anim_cfg.enabled)
    validate_dedup_anim_consistency(frames, positions_by_unique, anim_cfg.enabled, cfg.mode)

    tiles_blob = tiles_to_4bpp_blob(tiles)
    metatiles_blob = metatiles_bin(metatiles, cfg.palette_slot, cfg.tile_base)
    attrs_blob = metatile_attributes_bin(len(metatiles), cfg.attr)
    sheet = tilesheet_png(tiles, palette_rgb)

    report = {
        "input": str(args.input),
        "mode": cfg.mode,
        "image_size": [base_img.width, base_img.height],
        "metatile_grid": [mt_w, mt_h],
        "tile_count": len(tiles),
        "metatile_count": len(metatiles),
        "tile_base": cfg.tile_base,
        "tile_range": [cfg.tile_base, cfg.tile_base + len(tiles) - 1],
        "palette_slot": cfg.palette_slot,
        "metatile_attr": cfg.attr,
        "outputs": {
            "tiles_4bpp": str(args.out_dir / "tiles.4bpp"),
            "metatiles_bin": str(args.out_dir / "metatiles.bin"),
            "metatile_attributes_bin": str(args.out_dir / "metatile_attributes.bin"),
            "tiles_png": str(args.out_dir / "tiles.png"),
            "palette_gbapal": str(args.out_dir / "palette.gbapal"),
            "palette_pal": str(args.out_dir / "palette.pal"),
        },
    }
    if anim_cfg.enabled:
        report["animation"] = {
            "enabled": True,
            "frames_x": anim_cfg.frames_x,
            "frames_y": anim_cfg.frames_y,
            "frame_size": [anim_cfg.frame_w, anim_cfg.frame_h],
            "frame_count": len(frames),
            "anim_name": anim_cfg.anim_name,
            "anim_dir": str(args.out_dir / "anim" / anim_cfg.anim_name),
            "preview_dir": str(args.out_dir / "anim_preview" / anim_cfg.anim_name),
            "validated_layout": True,
        }
    if reserved_key_rgb is not None:
        report["transparent_key"] = {"mode": args.transparent_key, "rgb": list(reserved_key_rgb)}
    if args.avoid_index_zero:
        report["avoid_index_zero"] = {
            "enabled": True,
            "remap_target_index": remap_target,
            "remapped_pixels": remap_pixels,
        }
    if args.lock_input_palette:
        report["lock_input_palette"] = {"enabled": True}

    write_files(
        args.out_dir, tiles_blob, metatiles_blob, attrs_blob, sheet, palette_rgb, report
    )
    write_anim_frames(args.out_dir, anim_frame_tiles, preview_source_frames, anim_cfg)

    if args.apply_to is not None:
        apply_to_tileset(
            args.apply_to,
            tiles_blob,
            metatiles_blob,
            attrs_blob,
            sheet,
            palette_rgb,
            cfg.palette_slot,
            args.write_palette,
        )
        apply_anim_to_tileset(args.apply_to, anim_frame_tiles, anim_cfg)

    print(
        f"OK: tiles={len(tiles)} metatiles={len(metatiles)} "
        f"mode={cfg.mode} tile_base={cfg.tile_base:#x} palette_slot={cfg.palette_slot}"
    )
    print(f"Output: {args.out_dir}")
    if args.apply_to:
        print(f"Applied to: {args.apply_to}")


if __name__ == "__main__":
    main()
