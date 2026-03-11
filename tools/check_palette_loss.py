#!/usr/bin/env python3
from __future__ import annotations

import argparse
import math
from pathlib import Path
from typing import List, Sequence, Tuple

from PIL import Image


RGB = Tuple[int, int, int]


def parse_jasc_pal(path: Path) -> List[RGB]:
    lines = [ln.strip() for ln in path.read_text(encoding="utf-8", errors="replace").splitlines() if ln.strip()]
    if len(lines) < 3 or lines[0] != "JASC-PAL":
        raise SystemExit(f"error: invalid JASC-PAL file: {path}")
    count = int(lines[2])
    colors: List[RGB] = []
    for ln in lines[3 : 3 + count]:
        parts = ln.split()
        if len(parts) < 3:
            continue
        colors.append((int(parts[0]), int(parts[1]), int(parts[2])))
    if len(colors) < 16:
        colors += [(0, 0, 0)] * (16 - len(colors))
    return colors[:16]


def input_used_colors(png: Path) -> List[RGB]:
    img = Image.open(png)
    if img.mode != "P":
        # For non-indexed images, evaluate visible RGB values.
        rgb = img.convert("RGB")
        return sorted(set(rgb.getdata()))
    pal = img.getpalette() or []
    pal = pal[:48] + [0] * max(0, 48 - len(pal))
    palette = [(pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]) for i in range(16)]
    used_idx = sorted(set(img.tobytes()))
    return [palette[i] for i in used_idx if 0 <= i < 16]


def dist(a: RGB, b: RGB) -> float:
    return math.sqrt((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2 + (a[2] - b[2]) ** 2)


def nearest(color: RGB, palette: Sequence[RGB]) -> Tuple[RGB, float]:
    best = None
    best_d = 1e9
    for p in palette:
        d = dist(color, p)
        if d < best_d:
            best_d = d
            best = p
    return best, best_d


def fmt(c: RGB) -> str:
    return f"#{c[0]:02X}{c[1]:02X}{c[2]:02X}"


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Diagnose if colors from input PNG were lost/replaced in output palette."
    )
    ap.add_argument("--input-png", type=Path, required=True, help="Original PNG used for conversion")
    ap.add_argument("--output-pal", type=Path, required=True, help="Output palette.pal (JASC-PAL)")
    ap.add_argument(
        "--warn-distance",
        type=float,
        default=8.0,
        help="Warn when nearest output color differs by this distance or more (default: 8.0)",
    )
    args = ap.parse_args()

    in_colors = input_used_colors(args.input_png)
    out_pal = parse_jasc_pal(args.output_pal)

    print(f"Input used colors: {len(in_colors)}")
    print(f"Output palette colors: {len(out_pal)}")
    print("")

    exact = 0
    changed = 0
    for c in in_colors:
        if c in out_pal:
            exact += 1
            continue
        n, d = nearest(c, out_pal)
        changed += 1
        mark = " !!" if d >= args.warn_distance else ""
        print(f"{fmt(c)} -> {fmt(n)}  dist={d:.2f}{mark}")

    print("")
    print(f"Exact preserved: {exact}/{len(in_colors)}")
    print(f"Changed/replaced: {changed}/{len(in_colors)}")


if __name__ == "__main__":
    main()

