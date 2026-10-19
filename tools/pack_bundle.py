#!/usr/bin/env python3
"""Empaqueta el conjunto estable #1–251 en web/sprites.pak para que el
instalador público lo suba de un clic. Los conjuntos locales Gen-3 se generan
con --gen3 o --gen3-full.

Formato TPAK (little-endian):
  char[4]  "TPAK"
  uint16   count
  count x { uint8 nameLen; char name[nameLen]; uint32 size }   (indice)
  ...datos de cada fichero, en el mismo orden...

El instalador (web/index.html) lo descarga, lo parte por el indice y manda cada
fichero a la placa con el protocolo PUT (igual que tools/send_sd.py).

Flags locales: --gen2 (161–251), --gen3 (252–386) y --gen3-full (1–386).
--gen9 [--thumbs PATH]: un pak por generacion 4-9 (#387-1025), web/sprites-genN-update.pak.
"""
import os
import struct
import sys

HERE = os.path.dirname(__file__)
MONS = os.path.join(HERE, 'sdcard', 'mons')
OUT = os.path.join(HERE, '..', 'web', 'sprites.pak')
GEN2_OUT = os.path.join(HERE, '..', 'web', 'sprites-gen2-update.pak')
GEN3_OUT = os.path.join(HERE, '..', 'web', 'sprites-gen3-update.pak')
GEN3_FULL_OUT = os.path.join(HERE, '..', 'web', 'sprites-gen3-full.pak')
PUBLIC_THUMBS = os.path.join(MONS, 'thumbs-public.bin')


GEN_RANGES = {4: (387, 493), 5: (494, 649), 6: (650, 721),
              7: (722, 809), 8: (810, 905), 9: (906, 1025)}


def is_static_fallback(path):
    """pack_static.py output (one idle frame, PokeAPI/Nintendo art): never shipped."""
    with open(path, 'rb') as f:
        head = f.read(5)
    return len(head) == 5 and head[4] == 1


def pack_gen9(args):
    """One update pak per generation (Gen 4..9, #387-1025), like the Gen 2/3 updates.
    Every pak also carries the complete 1025-entry thumbs.bin (pass --thumbs PATH,
    default mons/thumbs.bin) so the gallery can address all entries after any subset.
    Static fallback sprites from pack_static.py are skipped (no open licence)."""
    thumbs = os.path.join(MONS, 'thumbs.bin')
    if '--thumbs' in args:
        thumbs = args[args.index('--thumbs') + 1]
    if not os.path.exists(thumbs):
        raise SystemExit('missing thumbs index: ' + thumbs)
    total = 0
    for gen, (lo, hi) in GEN_RANGES.items():
        files = []
        for n in range(lo, hi + 1):
            for pre in ('p', 'ps'):
                f = os.path.join(MONS, f'{pre}{n:03d}.bin')
                if os.path.exists(f) and not is_static_fallback(f):
                    files.append(f)
        if not files:
            print(f'gen{gen}: no sprites, skipped')
            continue
        entries = [(os.path.basename(f), f) for f in files]
        entries = [('mons/' + n, f) for n, f in entries] + [('mons/thumbs.bin', thumbs)]
        out_path = os.path.join(HERE, '..', 'web', f'sprites-gen{gen}-update.pak')
        blobs = [open(f, 'rb').read() for _, f in entries]
        with open(out_path, 'wb') as o:
            o.write(b'TPAK')
            o.write(struct.pack('<H', len(entries)))
            for (name, _), b in zip(entries, blobs):
                nb = name.encode()
                o.write(struct.pack('<B', len(nb)) + nb + struct.pack('<I', len(b)))
            for b in blobs:
                o.write(b)
        size = os.path.getsize(out_path)
        total += size
        print(f'{os.path.normpath(out_path)}: {len(entries)} files, {size / 1048576:.1f} MB')
        if size > 95 * 1048576:
            raise SystemExit('pak exceeds the 100 MB GitHub limit')
    print(f'total {total / 1048576:.1f} MB')


def main():
    args = sys.argv[1:]
    gen2_only = '--gen2' in args
    gen3_only = '--gen3' in args
    gen3_full = '--gen3-full' in args
    gen9 = '--gen9' in args
    output = None
    if '--output' in args:
        output_index = args.index('--output')
        if output_index + 1 >= len(args):
            raise SystemExit('--output requires a path')
        output = args[output_index + 1]
    if gen9:
        return pack_gen9(args)
    if sum((gen2_only, gen3_only, gen3_full)) > 1:
        raise SystemExit('choose one of --gen2, --gen3 or --gen3-full')
    if gen2_only:
        files = [os.path.join(MONS, f'p{n:03d}.bin') for n in range(161, 252)]
        files += [os.path.join(MONS, f'ps{n:03d}.bin') for n in range(161, 252)]
        # thumbs.bin is a complete index and must accompany a partial sprite
        # update so the gallery can address all 251 entries.
        files.append(os.path.join(MONS, 'thumbs.bin'))
        out_path = output or GEN2_OUT
    elif gen3_only:
        files = [os.path.join(MONS, f'p{n:03d}.bin') for n in range(252, 387)]
        files += [os.path.join(MONS, f'ps{n:03d}.bin') for n in range(252, 387)]
        # thumbs.bin is a complete index and must accompany a partial sprite
        # update so the gallery can address all 386 entries.
        files.append(os.path.join(MONS, 'thumbs.bin'))
        out_path = output or GEN3_OUT
    elif gen3_full:
        files = [os.path.join(MONS, f'p{n:03d}.bin') for n in range(1, 387)]
        files += [os.path.join(MONS, f'ps{n:03d}.bin') for n in range(1, 387)]
        files.append(os.path.join(MONS, 'thumbs.bin'))
        out_path = output or GEN3_FULL_OUT
    else:
        # The public installer is intentionally pinned to the stable #1–251
        # set. A local Gen-3 run may have extra files in this directory, so do
        # not use a broad glob here. The public thumbnail index is renamed in
        # the TPAK below to the firmware's expected mons/thumbs.bin path.
        files = [os.path.join(MONS, f'p{n:03d}.bin') for n in range(1, 252)]
        files += [os.path.join(MONS, f'ps{n:03d}.bin') for n in range(1, 252)]
        files.append(PUBLIC_THUMBS)
        out_path = output or OUT
    if not files:
        raise SystemExit('no hay sprites en ' + MONS)
    names = ['mons/thumbs.bin' if f == PUBLIC_THUMBS else 'mons/' + os.path.basename(f)
             for f in files]
    blobs = [open(f, 'rb').read() for f in files]

    with open(out_path, 'wb') as o:
        o.write(b'TPAK')
        o.write(struct.pack('<H', len(files)))
        for name, blob in zip(names, blobs):
            nb = name.encode()
            o.write(struct.pack('<B', len(nb)))
            o.write(nb)
            o.write(struct.pack('<I', len(blob)))
        for blob in blobs:
            o.write(blob)

    total = sum(len(b) for b in blobs)
    print(f'{os.path.normpath(out_path)}: {len(files)} sprites, {total / 1048576:.1f} MB datos '
          f'({os.path.getsize(out_path) / 1048576:.1f} MB total)')


if __name__ == '__main__':
    main()
