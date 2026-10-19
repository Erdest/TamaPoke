#!/usr/bin/env python3
"""Generates dex_gen9.h: Pokedex data for Gen 4-9 (#387-#1025) from the public
PokeAPI CSV dump (https://github.com/PokeAPI/pokeapi, data/v2/csv).

The existing 386-species dex.h is NOT touched; this adds an extension block that
the firmware can adopt when Gen 4-9 support is wired in:

  python3 tools/gen_dex_gen9.py            # downloads the CSVs (cached in tools/pokeapi_cache)

Notes
- Names are ASCII upper-case (the GFX font has no accents). PokeAPI has no
  Portuguese species names, so PT falls back to EN.
- Base stats are the current-generation values (the 1-386 table uses Gen 1 stats).
- Trade/item/location evolutions use the level-based fallbacks from the feature list
  (trade ~40, item ~30, location ~35, happiness -> EVO_BOND*), see map_evolution().
"""
import csv
import os
import sys
import unicodedata
import urllib.request

BASE = 'https://raw.githubusercontent.com/PokeAPI/pokeapi/master/data/v2/csv'
HERE = os.path.dirname(os.path.abspath(__file__))
CACHE = os.path.join(HERE, 'pokeapi_cache')
OUT = os.path.join(HERE, '..', 'dex_gen9.h')
FIRST, LAST = 387, 1025
LANGS = {'ES': 7, 'EN': 9, 'FR': 5, 'DE': 6, 'IT': 8}   # PT -> EN fallback

TYPE_IDS = {1: 'NORMAL', 2: 'FIGHTING', 3: 'FLYING', 4: 'POISON', 5: 'GROUND', 6: 'ROCK', 7: 'BUG',
            8: 'GHOST', 9: 'STEEL', 10: 'FIRE', 11: 'WATER', 12: 'GRASS', 13: 'ELECTRIC',
            14: 'PSYCHIC', 15: 'ICE', 16: 'DRAGON', 17: 'DARK', 18: 'FAIRY'}
ACCENT = {  # same palette as tools/dex_data.py TYPE_ACCENTS, English keys
    'NORMAL': '#8a8a6a', 'FIRE': '#e8503a', 'WATER': '#4f93c4', 'GRASS': '#3c8a4c', 'ELECTRIC': '#b8960b',
    'ICE': '#4fb4c4', 'FIGHTING': '#a5552d', 'POISON': '#8a4f9e', 'GROUND': '#b08a3d', 'FLYING': '#4f93c4',
    'PSYCHIC': '#d4527e', 'BUG': '#7a9a24', 'ROCK': '#93803d', 'GHOST': '#6a5a9e', 'DRAGON': '#5a52c4',
    'DARK': '#5a4f73', 'STEEL': '#7f8c9a', 'FAIRY': '#d4527e'}
BIOME = {'WATER': 1, 'GRASS': 2, 'BUG': 2, 'FIRE': 3, 'ROCK': 4, 'GROUND': 4, 'DRAGON': 1, 'ICE': 5, 'STEEL': 4}


def fetch(name):
    os.makedirs(CACHE, exist_ok=True)
    path = os.path.join(CACHE, name + '.csv')
    if not os.path.exists(path):
        req = urllib.request.Request(f'{BASE}/{name}.csv', headers={'User-Agent': 'tamapoke-gen'})
        open(path, 'wb').write(urllib.request.urlopen(req, timeout=60).read())
    with open(path, newline='', encoding='utf-8') as f:
        return list(csv.DictReader(f))


def ascii_upper(s):
    s = s.replace('♀', 'F').replace('♂', 'M').replace('’', "'")
    s = unicodedata.normalize('NFKD', s)
    s = ''.join(c for c in s if not unicodedata.combining(c))
    s = ''.join(c if ord(c) < 128 else '' for c in s).upper().replace('"', '')
    return s or '?'


def rgb565(hexcol):
    r, g, b = int(hexcol[1:3], 16), int(hexcol[3:5], 16), int(hexcol[5:7], 16)
    return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3)


def map_evolution(row):
    """Returns (min_level, condition) following the feature-list fallbacks."""
    trig = int(row['evolution_trigger_id'] or 0)
    lvl = int(row['minimum_level'] or 0)
    happy = int(row['minimum_happiness'] or 0) or int(row['minimum_affection'] or 0) or int(row['minimum_beauty'] or 0)
    tod = row['time_of_day']
    if row['relative_physical_stats']:
        return max(lvl, 20), {'1': 'EVO_ATK_GT_DEF', '-1': 'EVO_DEF_GT_ATK', '0': 'EVO_ATK_EQ_DEF'}[row['relative_physical_stats']]
    if happy:
        return max(lvl, 16), {'day': 'EVO_DAY_BOND', 'night': 'EVO_NIGHT_BOND'}.get(tod, 'EVO_BOND')
    if trig == 1 and lvl:
        return lvl, 'EVO_LEVEL'
    if trig == 2:                       # trade
        return 38 if row['held_item_id'] else 40, 'EVO_LEVEL'
    if trig == 3 or row['trigger_item_id']:   # stone / item
        return 30, 'EVO_LEVEL'
    if row['location_id']:
        return 35, 'EVO_LEVEL'
    return max(lvl, 32), 'EVO_LEVEL'


def main():
    species = {int(r['id']): r for r in fetch('pokemon_species')}
    pokemon = {int(r['species_id']): int(r['id']) for r in fetch('pokemon') if r['is_default'] == '1'}
    stats, types = {}, {}
    for r in fetch('pokemon_stats'):
        stats.setdefault(int(r['pokemon_id']), {})[int(r['stat_id'])] = int(r['base_stat'])
    for r in fetch('pokemon_types'):
        types.setdefault(int(r['pokemon_id']), {})[int(r['slot'])] = TYPE_IDS[int(r['type_id'])]
    names = {}
    for r in fetch('pokemon_species_names'):
        names[(int(r['pokemon_species_id']), int(r['local_language_id']))] = r['name']
    evo_rows = [r for r in fetch('pokemon_evolution') if r['is_default'] != '0' or True]

    # evolution rules (first rule per from->to pair; keep branches)
    rules, seen = [], set()
    by_to = {}
    for r in evo_rows:
        to = int(r['evolved_species_id'])
        by_to.setdefault(to, []).append(r)
    for to in sorted(by_to):
        frm = species[to]['evolves_from_species_id']
        if not frm:
            continue
        frm = int(frm)
        if (frm, to) in seen:
            continue
        seen.add((frm, to))
        level, cond = map_evolution(by_to[to][0])
        rules.append((frm, to, level, cond))
    rules = [x for x in rules if x[1] >= FIRST or x[0] >= FIRST]   # rules touching the new range

    evolves_to = {}
    for frm, to, lvl, cond in rules:
        evolves_to.setdefault(frm, (to, lvl))

    # chain BST for rarity
    def bst(num):
        s = stats.get(pokemon.get(num, num), {})
        return sum(s.get(i, 0) for i in (1, 2, 3, 4, 5, 6))
    chain_max = {}
    for num in range(1, LAST + 1):
        ch = species[num]['evolution_chain_id']
        chain_max[ch] = max(chain_max.get(ch, 0), bst(num))
    evolved_targets = {t for _, t, _, _ in rules}

    rows, evo_lines = [], []
    for num in range(FIRST, LAST + 1):
        sp = species[num]
        pid = pokemon[num]
        t = types[pid]
        t1, t2 = t[1], t.get(2)
        st = stats[pid]
        evo, lvl = evolves_to.get(num, (0, 0))
        if num in evolved_targets or sp['evolves_from_species_id']:
            rar = 'R_EVO'
        elif sp['is_legendary'] == '1' or sp['is_mythical'] == '1':
            rar = 'R_LEGENDARIO'
        elif chain_max[sp['evolution_chain_id']] >= 500:
            rar = 'R_RARO'
        else:
            rar = 'R_COMUN'
        biome = BIOME.get(t1, 0)
        name = ascii_upper(names[(num, 9)])
        rows.append(f'  {{ "{name}", {evo}, {lvl}, {rar}, 0x{rgb565(ACCENT[t1]):04X}, {st[1]}, {st[2]}, {st[3]}, {st[6]}, '
                    f'TYPE_{t1}, {"TYPE_" + t2 if t2 else "TYPE_NONE"}, {biome} }},  // {num} {t1.lower()}' + (f'/{t2.lower()}' if t2 else '') + '\n')
    for frm, to, lvl, cond in rules:
        evo_lines.append(f'  {{ {frm}, {to}, {lvl}, {cond} }},\n')
    root = os.path.dirname(OUT)
    def write_inc(name, text):
        with open(os.path.join(root, name), 'w', encoding='ascii') as f:
            f.write('// GENERATED by tools/gen_dex_gen9.py - do not edit.\n' + text)
    write_inc('dex_gen9_rows.def.h', ''.join(rows))
    write_inc('dex_gen9_evo.def.h', ''.join(evo_lines))
    for label, lid in (('ES', 7), ('EN', 9), ('FR', 5), ('DE', 6), ('IT', 8), ('PT', 9)):
        row = ', '.join('"%s"' % ascii_upper(names.get((n, lid)) or names[(n, 9)]) for n in range(FIRST, LAST + 1))
        write_inc(f'dex_gen9_names_{label}.def.h', row + '\n')
    out = ['#pragma once\n#include <stdint.h>\n#include "dex.h"\n\n',
           '// GENERATED by tools/gen_dex_gen9.py from the PokeAPI CSV dump - do not edit.\n',
           '// Extension block for Gen 4-9 (#387-#1025). Index = dex - DEX9_FIRST.\n',
           '// The rows live in dex_gen9_*.def.h so dex.h can splice them into the main\n',
           '// tables when built with -DTAMAPOKE_GEN9_DEX.\n\n',
           f'#define DEX9_FIRST {FIRST}\n#define DEX9_LAST {LAST}\n#define DEX9_COUNT {LAST - FIRST + 1}\n\n',
           'static const DexEntry DEX9_TBL[DEX9_COUNT] = {\n#include "dex_gen9_rows.def.h"\n};\n\n',
           '// Evolution rules touching Gen 4-9 species (cross-generation links included).\n',
           'static const EvolutionRule EVOLUTION_RULES_GEN9[] = {\n#include "dex_gen9_evo.def.h"\n};\n',
           f'#define EVOLUTION_RULE_GEN9_COUNT {len(rules)}\n\n',
           '// Localized names in Lang order: ES, EN, FR, DE, IT, PT (PT falls back to EN)\n',
           'static const char *const DEX9_NAMES[DEX_LANG_COUNT][DEX9_COUNT] = {\n']
    for label in ('ES', 'EN', 'FR', 'DE', 'IT', 'PT'):
        out.append(f'  // {label}\n  {{\n#include "dex_gen9_names_{label}.def.h"\n  }},\n')
    out.append('};\n')
    with open(OUT, 'w', encoding='ascii') as f:
        f.write(''.join(out))
    print(f'wrote {os.path.relpath(OUT)}: {LAST - FIRST + 1} species, {len(rules)} evolution rules')


if __name__ == '__main__':
    sys.exit(main())
