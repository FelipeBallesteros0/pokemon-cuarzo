#!/usr/bin/env python3
import argparse
import json
import os
import re
import textwrap
import urllib.request
import time
import socket
from pathlib import Path

CACHE_DIR = Path(".cache/pokeapi")
BASE_URL = "https://pokeapi.co/api/v2"

PREFERRED_VERSION_GROUPS = [
    "scarlet-violet",
    "sword-shield",
    "ultra-sun-ultra-moon",
    "sun-moon",
    "omega-ruby-alpha-sapphire",
    "x-y",
    "black-2-white-2",
    "black-white",
    "heartgold-soulsilver",
    "platinum",
    "diamond-pearl",
    "emerald",
    "firered-leafgreen",
    "ruby-sapphire",
]

TARGET_FILES = {
    "moves": "src/data/moves_info.h",
    "abilities": "src/data/abilities.h",
    "items": "src/data/items.h",
    "types": "src/data/types_info.h",
}

CONST_FILES = {
    "moves": "include/constants/moves.h",
    "abilities": "include/constants/abilities.h",
    "items": "include/constants/items.h",
    "types": "include/constants/pokemon.h",
}


def fetch_json(url, retries=3, timeout=20, offline=False):
    CACHE_DIR.mkdir(parents=True, exist_ok=True)
    key = re.sub(r"[^a-zA-Z0-9._-]", "_", url)
    cache_path = CACHE_DIR / f"{key}.json"
    if cache_path.exists():
        with open(cache_path, "r", encoding="utf-8") as f:
            return json.load(f)
    if offline:
        return None
    headers = {
        "User-Agent": "pokeemerald-expansion/translate (contact: local)",
        "Accept": "application/json",
    }
    req = urllib.request.Request(url, headers=headers)
    last_err = None
    for attempt in range(retries):
        try:
            with urllib.request.urlopen(req, timeout=timeout) as r:
                data = r.read().decode("utf-8")
            break
        except Exception as e:
            last_err = e
            time.sleep(1 + attempt)
    else:
        raise last_err
    with open(cache_path, "w", encoding="utf-8") as f:
        f.write(data)
    return json.loads(data)


def normalize_name(s):
    s = s.lower()
    s = s.replace("’", "'")
    s = re.sub(r"[^a-z0-9]+", " ", s)
    s = re.sub(r"\s+", " ", s).strip()
    return s


def pick_spanish_name(names):
    for n in names:
        if n.get("language", {}).get("name") == "es":
            return n.get("name")
    return None


def pick_spanish_flavor_text(entries):
    # choose by preferred version group
    def vg_name(entry):
        vg = entry.get("version_group") or entry.get("version")
        if vg:
            return vg.get("name")
        return ""

    candidates = [e for e in entries if e.get("language", {}).get("name") == "es"]
    if not candidates:
        return None

    for vg in PREFERRED_VERSION_GROUPS:
        for e in candidates:
            if vg_name(e) == vg:
                return e.get("flavor_text")
    return candidates[0].get("flavor_text")


def wrap_description(text, width=18):
    if text is None:
        return ""
    text = text.replace("\f", " ").replace("\n", " ")
    text = re.sub(r"\s+", " ", text).strip()
    if not text:
        return ""
    lines = textwrap.wrap(text, width=width, break_long_words=False, break_on_hyphens=False)
    return lines


def build_lookup(resource, limit, offline=False):
    data = fetch_json(f"{BASE_URL}/{resource}?limit={limit}", offline=offline)
    return data.get("results", [])


def load_constants(path, prefix):
    text = Path(path).read_text(encoding="utf-8")
    consts = {}
    # First, capture any explicit #defines.
    for line in text.splitlines():
        m = re.match(rf"#define\s+({prefix}_[A-Z0-9_]+)\s+(\d+)", line)
        if m:
            consts[m.group(1)] = int(m.group(2))

    # Then, parse enums with implicit incrementing values.
    in_enum = False
    pending_enum = False
    current = None
    for line in text.splitlines():
        if not in_enum and "enum" in line:
            pending_enum = True
        if pending_enum and "{" in line:
            in_enum = True
            pending_enum = False
            current = None
            continue
        if in_enum and "}" in line:
            in_enum = False
            current = None
            continue
        if not in_enum:
            continue

        m = re.match(rf"\s*({prefix}_[A-Z0-9_]+)\s*(=\s*(\d+))?\s*,?", line)
        if not m:
            continue
        name = m.group(1)
        if m.group(3) is not None:
            current = int(m.group(3))
        else:
            current = 0 if current is None else current + 1
        if name not in consts:
            consts[name] = current
    return consts


def get_move_translation_by_id(move_id, offline=False):
    detail = fetch_json(f"{BASE_URL}/move/{move_id}/", offline=offline)
    if not detail:
        return None, None
    es_name = pick_spanish_name(detail.get("names", []))
    es_desc = pick_spanish_flavor_text(detail.get("flavor_text_entries", []))
    return es_name, es_desc


def get_ability_translation_by_id(ability_id, offline=False):
    detail = fetch_json(f"{BASE_URL}/ability/{ability_id}/", offline=offline)
    if not detail:
        return None, None
    es_name = pick_spanish_name(detail.get("names", []))
    es_desc = pick_spanish_flavor_text(detail.get("flavor_text_entries", []))
    return es_name, es_desc


def get_item_translation_by_id(item_id, offline=False):
    detail = fetch_json(f"{BASE_URL}/item/{item_id}/", offline=offline)
    if not detail:
        return None, None
    es_name = pick_spanish_name(detail.get("names", []))
    es_desc = pick_spanish_flavor_text(detail.get("flavor_text_entries", []))
    return es_name, es_desc


def get_type_translation_by_id(type_id, offline=False):
    detail = fetch_json(f"{BASE_URL}/type/{type_id}/", offline=offline)
    if not detail:
        return None, None
    es_name = pick_spanish_name(detail.get("names", []))
    return es_name, None


def escape_c_string(s):
    s = s.replace("\\", "\\\\")
    s = s.replace("\"", "\\\"")
    return s


def replace_compound_string(lines, indent="        "):
    if len(lines) == 1:
        return f"COMPOUND_STRING(\"{escape_c_string(lines[0])}\")"
    out = "COMPOUND_STRING(\n"
    for line in lines:
        out += f"{indent}\"{escape_c_string(line)}\\n\"\n"
    out = out.rstrip("\n") + ")"
    return out


def replace_description_section(block, text):
    lines = block.splitlines()
    desc_idx = None
    effect_idx = None
    for i, line in enumerate(lines):
        if ".description" in line:
            desc_idx = i
            break
    if desc_idx is None:
        return block
    for i in range(desc_idx + 1, len(lines)):
        if ".effect" in lines[i]:
            effect_idx = i
            break
    if effect_idx is None:
        return block

    power_idx = None
    for i in range(effect_idx + 1, len(lines)):
        if ".power" in lines[i]:
            power_idx = i
            break
    if power_idx is None:
        power_idx = len(lines)

    i = effect_idx
    while i < power_idx:
        stripped = lines[i].lstrip()
        if stripped.startswith(("#else", "#elif")):
            j = i + 1
            while j < power_idx and not lines[j].lstrip().startswith("#endif"):
                j += 1
            if j < power_idx and lines[j].lstrip().startswith("#endif"):
                del lines[i:j + 1]
                power_idx -= (j + 1 - i)
                continue
        i += 1

    indent = re.match(r"\s*", lines[desc_idx]).group(0)
    desc_lines = wrap_description(text, width=18)
    if not desc_lines:
        return block
    compound = replace_compound_string(desc_lines, indent=indent + "    ")
    desc_block = f"{indent}.description = {compound},"

    start_idx = desc_idx
    while start_idx > 0:
        prev = lines[start_idx - 1].lstrip()
        if prev.startswith(("#if", "#elif", "#else", "#endif")):
            start_idx -= 1
            continue
        break

    new_lines = lines[:start_idx] + desc_block.splitlines() + lines[effect_idx:]
    trailing_newline = "\n" if block.endswith("\n") else ""
    return "\n".join(new_lines) + trailing_newline


def update_moves(file_path, move_ids, valid_ids, offline=False):
    text = Path(file_path).read_text(encoding="utf-8")
    out = []
    i = 0
    while i < len(text):
        m = re.search(r"\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text[i:])
        if not m:
            out.append(text[i:])
            break
        const_name = m.group(1)
        start = i + m.start()
        out.append(text[i:start])
        # find block end
        brace = 0
        j = start
        while j < len(text):
            if text[j] == '{':
                brace += 1
            elif text[j] == '}':
                brace -= 1
                if brace == 0:
                    j += 1
                    break
            j += 1
        block = text[start:j]

        # extract current english name
        move_id = move_ids.get(const_name)
        if move_id is not None and (valid_ids is None or move_id in valid_ids):
            es_name, es_desc = get_move_translation_by_id(move_id, offline=offline)
            if es_name:
                es_name = escape_c_string(es_name)
                block = re.sub(r"\.name\s*=\s*(?:COMPOUND_STRING|_)\(\"[^\"]*\"\)",
                               f".name = _(\"{es_name}\")", block)
            if es_desc is not None:
                block = replace_description_section(block, es_desc)
        out.append(block)
        i = j
    Path(file_path).write_text("".join(out), encoding="utf-8")


def update_abilities(file_path, ability_ids, valid_ids, offline=False):
    text = Path(file_path).read_text(encoding="utf-8")
    out = []
    i = 0
    while i < len(text):
        m = re.search(r"\[(ABILITY_[A-Z0-9_]+)\]\s*=\s*\{", text[i:])
        if not m:
            out.append(text[i:])
            break
        const_name = m.group(1)
        start = i + m.start()
        out.append(text[i:start])
        brace = 0
        j = start
        while j < len(text):
            if text[j] == '{':
                brace += 1
            elif text[j] == '}':
                brace -= 1
                if brace == 0:
                    j += 1
                    break
            j += 1
        block = text[start:j]
        ability_id = ability_ids.get(const_name)
        if ability_id is not None and (valid_ids is None or ability_id in valid_ids):
            es_name, es_desc = get_ability_translation_by_id(ability_id, offline=offline)
            if es_name:
                es_name = escape_c_string(es_name)
                block = re.sub(r"\.name\s*=\s*(?:COMPOUND_STRING|_)\(\"[^\"]*\"\)",
                               f".name = _(\"{es_name}\")", block)
            if es_desc is not None:
                desc_lines = wrap_description(es_desc, width=18)
                if desc_lines:
                    compound = replace_compound_string(desc_lines)
                    block = re.sub(r"\.description\s*=\s*COMPOUND_STRING\((?:.|\n)*?\),",
                                   lambda m: f".description = {compound},", block)
        out.append(block)
        i = j
    Path(file_path).write_text("".join(out), encoding="utf-8")


def update_items(file_path, item_ids, valid_ids, offline=False):
    text = Path(file_path).read_text(encoding="utf-8")
    out = []
    i = 0
    while i < len(text):
        m = re.search(r"\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\{", text[i:])
        if not m:
            out.append(text[i:])
            break
        const_name = m.group(1)
        start = i + m.start()
        out.append(text[i:start])
        brace = 0
        j = start
        while j < len(text):
            if text[j] == '{':
                brace += 1
            elif text[j] == '}':
                brace -= 1
                if brace == 0:
                    j += 1
                    break
            j += 1
        block = text[start:j]
        item_id = item_ids.get(const_name)
        if item_id is not None and (valid_ids is None or item_id in valid_ids):
            es_name, es_desc = get_item_translation_by_id(item_id, offline=offline)
            if es_name:
                es_name = escape_c_string(es_name)
                block = re.sub(r"\.name\s*=\s*COMPOUND_STRING\(\"[^\"]*\"\)",
                               f".name = COMPOUND_STRING(\"{es_name}\")", block)
            if es_desc is not None:
                desc_lines = wrap_description(es_desc, width=18)
                if desc_lines:
                    compound = replace_compound_string(desc_lines)
                    block = re.sub(r"\.description\s*=\s*COMPOUND_STRING\((?:.|\n)*?\),",
                                   lambda m: f".description = {compound},", block)
        out.append(block)
        i = j
    Path(file_path).write_text("".join(out), encoding="utf-8")


def update_types(file_path, type_ids, valid_ids, offline=False):
    text = Path(file_path).read_text(encoding="utf-8")
    out = []
    i = 0
    while i < len(text):
        m = re.search(r"\[(TYPE_[A-Z0-9_]+)\]\s*=\s*\{", text[i:])
        if not m:
            out.append(text[i:])
            break
        const_name = m.group(1)
        start = i + m.start()
        out.append(text[i:start])
        brace = 0
        j = start
        while j < len(text):
            if text[j] == '{':
                brace += 1
            elif text[j] == '}':
                brace -= 1
                if brace == 0:
                    j += 1
                    break
            j += 1
        block = text[start:j]
        type_id = type_ids.get(const_name)
        if type_id is not None and (valid_ids is None or type_id in valid_ids):
            es_name, _ = get_type_translation_by_id(type_id, offline=offline)
            if es_name:
                es_name = escape_c_string(es_name)
                block = re.sub(r"\.name\s*=\s*_\(\"[^\"]*\"\)",
                               f".name = _(\"{es_name}\")", block)
        out.append(block)
        i = j
    Path(file_path).write_text("".join(out), encoding="utf-8")


def extract_ids_from_list(results):
    ids = set()
    for r in results:
        url = r.get("url", "")
        m = re.search(r"/(\\d+)/?$", url)
        if m:
            ids.add(int(m.group(1)))
    return ids


def main():
    parser = argparse.ArgumentParser(description="Translate moves, abilities, items, types to Spanish (official) using PokeAPI.")
    parser.add_argument("--apply", action="store_true", help="Apply translations to repo files")
    parser.add_argument("--offline", action="store_true", help="Use cache only, do not access network")
    args = parser.parse_args()

    try:
        move_ids = load_constants(CONST_FILES["moves"], "MOVE")
        ability_ids = load_constants(CONST_FILES["abilities"], "ABILITY")
        item_ids = load_constants(CONST_FILES["items"], "ITEM")
        type_ids = load_constants(CONST_FILES["types"], "TYPE")
        if args.offline:
            valid_move_ids = None
            valid_ability_ids = None
            valid_item_ids = None
            valid_type_ids = None
        else:
            move_list = build_lookup("move", 2000, offline=args.offline)
            ability_list = build_lookup("ability", 2000, offline=args.offline)
            item_list = build_lookup("item", 3000, offline=args.offline)
            type_list = build_lookup("type", 100, offline=args.offline)
            valid_move_ids = extract_ids_from_list(move_list)
            valid_ability_ids = extract_ids_from_list(ability_list)
            valid_item_ids = extract_ids_from_list(item_list)
            valid_type_ids = extract_ids_from_list(type_list)
    except (urllib.error.URLError, socket.timeout) as e:
        print(f"Network error: {e}", flush=True)
        print("PokeAPI not reachable or cache missing. Ensure you have Internet access or run once online to fill cache.", flush=True)
        return

    if not args.apply:
        print("Dry run complete. Use --apply to write changes.")
        return

    update_moves(TARGET_FILES["moves"], move_ids, valid_move_ids, offline=args.offline)
    update_abilities(TARGET_FILES["abilities"], ability_ids, valid_ability_ids, offline=args.offline)
    update_items(TARGET_FILES["items"], item_ids, valid_item_ids, offline=args.offline)
    update_types(TARGET_FILES["types"], type_ids, valid_type_ids, offline=args.offline)

    print("Translation applied.")


if __name__ == "__main__":
    main()
