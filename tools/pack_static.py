#!/usr/bin/env python3
"""Fallback sprite packer: builds a one-frame Idle TPK2 file from the static
PokeAPI/sprites PNGs for species that SpriteCollab (pack_pmd.py) cannot supply.

    python3 tools/pack_static.py            # only species with no p<NNN>.bin yet
    python3 tools/pack_static.py 514 520    # explicit list (overwrites)

NOTE: PokeAPI/sprites ships Nintendo / Game Freak artwork with no open licence.
Keep the output out of the repository; it is for local SD-card use only.
"""
import io
import os
import struct
import sys
import urllib.request
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, 'sdcard', 'mons')
BASE = 'https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon'
MAX_N = 1025
ALPHA_T = 128


def rgb565(r, g, b):
    return (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3)


def get(url):
    with urllib.request.urlopen(url, timeout=30) as r:
        return r.read()


def pack(n, shiny):
    im = Image.open(io.BytesIO(get(f'{BASE}/{"shiny/" if shiny else ""}{n}.png'))).convert('RGBA')
    box = im.getchannel('A').point(lambda a: 255 if a >= ALPHA_T else 0).getbbox()
    if not box:
        raise ValueError('empty sprite')
    im = im.crop(box)
    w, h = im.size
    colmap, pal, data = {}, [], bytearray()
    for px in im.getdata():
        if px[3] < ALPHA_T:
            data.append(0xFF)
            continue
        k = px[:3]
        if k not in colmap:
            if len(pal) >= 255:
                k2 = min(colmap, key=lambda c: sum((a - b) ** 2 for a, b in zip(c, k)))
                colmap[k] = colmap[k2]
            else:
                colmap[k] = len(pal)
                pal.append(k)
        data.append(colmap[k])
    path = os.path.join(OUT, f'p{"s" if shiny else ""}{n:03d}.bin')
    with open(path, 'wb') as f:
        f.write(b'TPK2')
        f.write(struct.pack('<BH', 1, len(pal)))
        for r, g, b in pal:
            f.write(struct.pack('<H', rgb565(r, g, b)))
        f.write(struct.pack('<4B', 0, w, h, 1))   # action 0 = Idle, 1 frame
        f.write(struct.pack('<H', 1000))
        f.write(data)
    return w, h, len(pal)


if __name__ == '__main__':
    os.makedirs(OUT, exist_ok=True)
    args = [int(a) for a in sys.argv[1:] if a.isdigit()]
    nums = args or [n for n in range(1, MAX_N + 1)
                    if not os.path.exists(os.path.join(OUT, f'p{n:03d}.bin'))]
    bad = []
    for n in nums:
        for sh in (False, True):
            try:
                print(f'#{n:03d}{" shiny" if sh else ""}: {pack(n, sh)}')
            except Exception as e:
                print(f'#{n:03d}{" shiny" if sh else ""}: FAIL {e}')
                bad.append((n, sh))
    print('FAILED:', bad if bad else 'none')
