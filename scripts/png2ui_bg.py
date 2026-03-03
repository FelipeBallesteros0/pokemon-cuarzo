#!/usr/bin/env python3
import argparse
import os
import sys
from pathlib import Path

try:
    from PIL import Image
except Exception as e:
    print("error: Pillow is required (pip install Pillow).", file=sys.stderr)
    sys.exit(1)


def die(msg):
    print(f"error: {msg}", file=sys.stderr)
    sys.exit(1)


def warn(msg):
    print(f"warning: {msg}", file=sys.stderr)


def parse_map(value):
    if value == "32x20":
        return (32, 20)
    if value == "32x32":
        return (32, 32)
    die("--map must be 32x20 or 32x32")


def count_colors_p(img):
    colors = img.getcolors(maxcolors=257)
    if colors is None:
        return 257
    return len(colors)


def image_to_indexed(img, fix):
    # If already paletted and no fix, preserve palette order
    if img.mode == "P" and not fix:
        colors = count_colors_p(img)
        if colors > 16:
            die("PNG uses more than 16 colors. Reduce colors or use --fix to quantize.")
        return img

    # Otherwise convert to RGBA and quantize
    rgba = img.convert("RGBA")
    # Count opaque colors
    opaque_colors = set()
    for r, g, b, a in rgba.getdata():
        if a != 0:
            opaque_colors.add((r, g, b))
            if len(opaque_colors) > 16 and not fix:
                die("PNG uses more than 16 colors. Reduce colors or use --fix to quantize.")
    if len(opaque_colors) > 16 and fix:
        warn("quantizing to 16 colors (--fix)")

    rgb = rgba.convert("RGB")
    # Quantize to 16 colors
    quant = rgb.quantize(colors=16, method=Image.MEDIANCUT, dither=Image.FLOYDSTEINBERG)
    return quant


def remap_transparency_to_index0(img_p, transparency_mask):
    if not transparency_mask:
        return img_p

    idx_data = list(img_p.getdata())
    palette = img_p.getpalette()[:48]
    pal_colors = [(palette[i], palette[i + 1], palette[i + 2]) for i in range(0, 48, 3)]

    # If any opaque pixels use index 0 and we need index 0 for transparency, try to move it.
    used = set(idx_data)
    opaque_used = set(i for i, is_trans in enumerate(transparency_mask) if not is_trans)
    opaque_indices = set(idx_data[i] for i in opaque_used) if opaque_used else set()

    if 0 in opaque_indices:
        # Find an unused index to move palette[0] to
        unused = [i for i in range(16) if i not in used]
        if not unused:
            # Try to find a duplicate color slot
            target = None
            for i in range(1, 16):
                if pal_colors[i] == pal_colors[0] and i not in opaque_indices:
                    target = i
                    break
            if target is None:
                die("cannot reserve palette index 0 for transparency without losing a color")
        else:
            target = unused[0]
            # Copy palette[0] to unused slot
            pal_colors[target] = pal_colors[0]
        # Remap pixels index 0 -> target
        idx_data = [target if v == 0 else v for v in idx_data]

    # Force transparent pixels to index 0
    for i, is_trans in enumerate(transparency_mask):
        if is_trans:
            idx_data[i] = 0

    # Write back
    out = Image.new("P", img_p.size)
    flat_pal = []
    for (r, g, b) in pal_colors:
        flat_pal.extend([r, g, b])
    out.putpalette(flat_pal + [0] * (768 - len(flat_pal)))
    out.putdata(idx_data)
    return out


def ensure_palette_16(img_p):
    palette = img_p.getpalette()[:48]
    pal_colors = [(palette[i], palette[i + 1], palette[i + 2]) for i in range(0, 48, 3)]
    if len(pal_colors) < 16:
        pal_colors += [(0, 0, 0)] * (16 - len(pal_colors))
    return pal_colors


def tile_bytes_from_indices(indices):
    return bytes(indices)


def flip_tile(indices, hflip, vflip):
    out = [0] * 64
    for y in range(8):
        for x in range(8):
            sx = 7 - x if hflip else x
            sy = 7 - y if vflip else y
            out[y * 8 + x] = indices[sy * 8 + sx]
    return out


def pack_tile_4bpp(indices):
    out = bytearray()
    for y in range(8):
        row = indices[y * 8:(y + 1) * 8]
        for x in range(0, 8, 2):
            out.append((row[x] & 0xF) | ((row[x + 1] & 0xF) << 4))
    return bytes(out)


def gba_color(r, g, b):
    r5 = (r * 31 + 127) // 255
    g5 = (g * 31 + 127) // 255
    b5 = (b * 31 + 127) // 255
    return r5 | (g5 << 5) | (b5 << 10)


def lz10_compress(data):
    # Simple LZ77 type 0x10 compressor
    out = bytearray()
    size = len(data)
    out.append(0x10)
    out.extend((size & 0xFF, (size >> 8) & 0xFF, (size >> 16) & 0xFF))

    pos = 0
    while pos < size:
        flag_pos = len(out)
        out.append(0)
        flags = 0
        for i in range(8):
            if pos >= size:
                break
            best_len = 0
            best_dist = 0
            window_start = max(0, pos - 0x1000)
            max_len = min(18, size - pos)
            # naive search
            for s in range(pos - 1, window_start - 1, -1):
                match_len = 0
                while match_len < max_len and data[s + match_len] == data[pos + match_len]:
                    match_len += 1
                if match_len >= 3 and match_len > best_len:
                    best_len = match_len
                    best_dist = pos - s
                    if best_len == max_len:
                        break
            if best_len >= 3:
                flags |= (1 << (7 - i))
                dist = best_dist - 1
                length = best_len - 3
                out.append(((length & 0xF) << 4) | ((dist >> 8) & 0xF))
                out.append(dist & 0xFF)
                pos += best_len
            else:
                out.append(data[pos])
                pos += 1
        out[flag_pos] = flags
    return bytes(out)


def pad_to_multiple(data, align):
    if align <= 1:
        return data
    pad = (-len(data)) % align
    if pad:
        return data + (b"\x00" * pad)
    return data


def write_file(path, data):
    Path(path).parent.mkdir(parents=True, exist_ok=True)
    with open(path, "wb") as f:
        f.write(data)


def write_integration(out_dir, name, use_lz):
    rel_dir = os.path.normpath(out_dir).replace("\\", "/")
    tiles_ext = ".4bpp.lz" if use_lz else ".4bpp"
    pal_ext = ".gbapal.lz" if use_lz else ".gbapal"
    map_ext = ".bin.lz" if use_lz else ".bin"

    c_path = Path("src/data/interface/pokemon_select_bg.c")
    h_path = Path("include/data/interface/pokemon_select_bg.h")

    c_path.parent.mkdir(parents=True, exist_ok=True)

    c_text = (
        "#include \"global.h\"\n"
        "#include \"graphics.h\"\n\n"
        f"const u32 gPokemonSelectBgTiles[] = INCBIN_U32(\"{rel_dir}/{name}{tiles_ext}\");\n"
        f"const u32 gPokemonSelectBgTilemap[] = INCBIN_U32(\"{rel_dir}/{name}{map_ext}\");\n"
        f"const u16 gPokemonSelectBgPal[] = INCBIN_U16(\"{rel_dir}/{name}{pal_ext}\");\n"
    )
    h_text = (
        "#ifndef GUARD_POKEMON_SELECT_BG_H\n"
        "#define GUARD_POKEMON_SELECT_BG_H\n\n"
        "extern const u32 gPokemonSelectBgTiles[];\n"
        "extern const u32 gPokemonSelectBgTilemap[];\n"
        "extern const u16 gPokemonSelectBgPal[];\n\n"
        "#endif // GUARD_POKEMON_SELECT_BG_H\n"
    )

    c_path.write_text(c_text)
    h_path.write_text(h_text)

    # Ensure graphics.h includes the header
    gfx_h = Path("include/graphics.h")
    if gfx_h.exists():
        text = gfx_h.read_text()
        include_line = "#include \"data/interface/pokemon_select_bg.h\"\n"
        if include_line not in text:
            # Add before final #endif
            if text.rstrip().endswith("#endif // GUARD_GRAPHICS_H"):
                text = text.replace("#endif // GUARD_GRAPHICS_H", include_line + "\n#endif // GUARD_GRAPHICS_H")
            else:
                text += "\n" + include_line
            gfx_h.write_text(text)


def main():
    parser = argparse.ArgumentParser(description="Convert PNG UI background to GBA assets.")
    parser.add_argument("input_png")
    parser.add_argument("out_dir")
    parser.add_argument("name")
    parser.add_argument("--map", default="32x20")
    parser.add_argument("--pad-to-32x20", action="store_true")
    parser.add_argument("--lz", action="store_true")
    parser.add_argument("--fix", action="store_true")
    parser.add_argument("--no-flip-dedupe", action="store_true")
    args = parser.parse_args()

    map_w, map_h = parse_map(args.map)

    if not os.path.isfile(args.input_png):
        die(f"input not found: {args.input_png}")

    img = Image.open(args.input_png)
    img.load()
    w, h = img.size

    if w % 8 != 0 or h % 8 != 0:
        die("PNG dimensions must be multiples of 8")

    target_w = map_w * 8
    target_h = map_h * 8

    if map_w == 32 and map_h == 20:
        if (w, h) == (240, 160):
            if not args.pad_to_32x20:
                die("PNG is 240x160; use --pad-to-32x20 or provide 256x160")
            # Pad to 256x160
            if img.mode == "P" and not args.fix:
                pal_img = img
                new_img = Image.new("P", (256, 160))
                new_img.putpalette(pal_img.getpalette())
                new_img.paste(pal_img, (0, 0))
                img = new_img
            else:
                # Pad with solid color (index 0 equivalent) to avoid introducing transparency
                new_img = Image.new("RGB", (256, 160), (0, 0, 0))
                new_img.paste(img.convert("RGB"), (0, 0))
                img = new_img
            w, h = img.size
        if (w, h) != (256, 160):
            die("PNG must be 256x160 for --map 32x20 (or 240x160 with --pad-to-32x20)")
    elif map_w == 32 and map_h == 32:
        if (w, h) != (256, 256):
            die("PNG must be 256x256 for --map 32x32")

    # Build transparency mask from alpha if available
    transparency_mask = []
    if img.mode in ("RGBA", "LA"):
        rgba = img.convert("RGBA")
        for r, g, b, a in rgba.getdata():
            transparency_mask.append(a == 0)
        img = rgba
    elif img.mode == "P":
        # Use transparency index if present
        trans_idx = img.info.get("transparency")
        if trans_idx is not None:
            transparency_mask = [px == trans_idx for px in img.getdata()]

    img_p = image_to_indexed(img, args.fix)
    # Ensure transparency mapped to index 0
    img_p = remap_transparency_to_index0(img_p, transparency_mask)

    colors = count_colors_p(img_p)
    if colors > 16:
        die("PNG uses more than 16 colors after processing. Reduce colors or use --fix.")

    # Validate indices are within 0..15
    idx_data = list(img_p.getdata())
    if any(v > 15 for v in idx_data):
        die("PNG uses palette indices > 15; must be 16 colors or fewer")

    palette = ensure_palette_16(img_p)

    # Build tiles and tilemap
    unique_tiles = []
    tile_lookup = {}  # bytes -> index
    tilemap_entries = []

    width, height = img_p.size
    for ty in range(map_h):
        for tx in range(map_w):
            indices = []
            for y in range(8):
                row_start = (ty * 8 + y) * width + (tx * 8)
                indices.extend(idx_data[row_start:row_start + 8])
            tile = tile_bytes_from_indices(indices)

            if args.no_flip_dedupe:
                key = tile
                if key in tile_lookup:
                    tile_index = tile_lookup[key]
                    hflip = 0
                    vflip = 0
                else:
                    tile_index = len(unique_tiles)
                    unique_tiles.append(indices)
                    tile_lookup[key] = tile_index
                    hflip = 0
                    vflip = 0
            else:
                # Try match with flips
                variants = [
                    (tile, 0, 0),
                    (tile_bytes_from_indices(flip_tile(indices, True, False)), 1, 0),
                    (tile_bytes_from_indices(flip_tile(indices, False, True)), 0, 1),
                    (tile_bytes_from_indices(flip_tile(indices, True, True)), 1, 1),
                ]
                found = False
                for key, hflip, vflip in variants:
                    if key in tile_lookup:
                        tile_index = tile_lookup[key]
                        found = True
                        break
                if not found:
                    tile_index = len(unique_tiles)
                    unique_tiles.append(indices)
                    tile_lookup[tile] = tile_index
                    hflip = 0
                    vflip = 0

            if tile_index > 1023:
                die("tile count exceeds 1024; reduce unique tiles")

            entry = tile_index | (hflip << 10) | (vflip << 11)
            tilemap_entries.append(entry)

    # Pack tiles
    tile_bytes = bytearray()
    for indices in unique_tiles:
        tile_bytes.extend(pack_tile_4bpp(indices))

    # Pack palette
    pal_bytes = bytearray()
    for r, g, b in palette[:16]:
        val = gba_color(r, g, b)
        pal_bytes.append(val & 0xFF)
        pal_bytes.append((val >> 8) & 0xFF)

    # Pack tilemap
    tm_bytes = bytearray()
    for entry in tilemap_entries:
        tm_bytes.append(entry & 0xFF)
        tm_bytes.append((entry >> 8) & 0xFF)

    out_dir = args.out_dir
    name = args.name
    Path(out_dir).mkdir(parents=True, exist_ok=True)

    tiles_path = os.path.join(out_dir, f"{name}.4bpp")
    pal_path = os.path.join(out_dir, f"{name}.gbapal")
    map_path = os.path.join(out_dir, f"{name}.bin")

    write_file(tiles_path, tile_bytes)
    write_file(pal_path, pal_bytes)
    write_file(map_path, tm_bytes)

    lz_sizes = {}
    if args.lz:
        tiles_lz = pad_to_multiple(lz10_compress(tile_bytes), 4)
        pal_lz = pad_to_multiple(lz10_compress(pal_bytes), 4)
        map_lz = pad_to_multiple(lz10_compress(tm_bytes), 4)
        write_file(tiles_path + ".lz", tiles_lz)
        write_file(pal_path + ".lz", pal_lz)
        write_file(map_path + ".lz", map_lz)
        lz_sizes = {
            "tiles": len(tiles_lz),
            "pal": len(pal_lz),
            "map": len(map_lz),
        }

    # Integration files
    write_integration(out_dir, name, args.lz)

    # Summary
    print("done")
    print(f"tiles unique: {len(unique_tiles)}")
    print(f"tiles size: {len(tile_bytes)} bytes")
    print(f"pal size: {len(pal_bytes)} bytes")
    print(f"tilemap size: {len(tm_bytes)} bytes")
    if lz_sizes:
        print(f"tiles lz size: {lz_sizes['tiles']} bytes")
        print(f"pal lz size: {lz_sizes['pal']} bytes")
        print(f"tilemap lz size: {lz_sizes['map']} bytes")


if __name__ == "__main__":
    main()
