#!/usr/bin/env python3
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
META = ROOT / 'data/tilesets/secondary/pueblo_ciendra_pool/metatiles.bin'

# fountain block in secondary metatile space (local ids 0..63 == map ids 0x200..0x23F)
FOUNTAIN_RANGE = range(0, 64)


def to_editor(v: int) -> int:
    tile = v & 0x03FF
    attrs = v & ~0x03FF
    if 0x200 <= tile <= 0x2FF:
        tile -= 0x200
    return attrs | tile


def to_runtime(v: int) -> int:
    tile = v & 0x03FF
    attrs = v & ~0x03FF
    if 0x000 <= tile <= 0x0FF:
        tile += 0x200
    return attrs | tile


def convert(mode: str) -> None:
    b = bytearray(META.read_bytes())
    count = len(b) // 16
    if count < 64:
        raise SystemExit('metatiles.bin too small')

    fn = to_editor if mode == 'editor' else to_runtime

    for m in FOUNTAIN_RANGE:
        off = m * 16
        vals = list(struct.unpack_from('<8H', b, off))
        vals = [fn(v) for v in vals]
        struct.pack_into('<8H', b, off, *vals)

    META.write_bytes(b)


def main() -> None:
    if len(sys.argv) != 2 or sys.argv[1] not in {'editor', 'runtime'}:
        print('Usage: pool_metatile_mode.py [editor|runtime]')
        raise SystemExit(2)

    mode = sys.argv[1]
    convert(mode)
    print(f'OK: {mode} mode written to {META}')


if __name__ == '__main__':
    main()
