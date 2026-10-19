#pragma once
// Unified view over the original table (#1-386, dex.h) and the Gen 4-9
// extension (#387-1025, dex_gen9.h). Lets new systems address all 1,025 species
// without changing the shipping firmware's DEX_COUNT or save layout.
#include <stdint.h>
#include "dex.h"
#include "dex_gen9.h"

constexpr uint16_t DEXALL_COUNT = DEX9_LAST;     // 1025

inline const DexEntry *dexAllEntry(uint16_t dex) {
  if (dex >= 1 && dex <= DEX_COUNT) return &DEX_TBL[dex];
  if (dex >= DEX9_FIRST && dex <= DEX9_LAST) return &DEX9_TBL[dex - DEX9_FIRST];
  return nullptr;
}
inline const char *dexAllName(uint16_t dex, uint8_t lang) {
  if (dex >= 1 && dex <= DEX_COUNT) return DEX_NAMES[lang < DEX_LANG_COUNT ? lang : 1][dex];
  if (dex >= DEX9_FIRST && dex <= DEX9_LAST) return DEX9_NAMES[lang < DEX_LANG_COUNT ? lang : 1][dex - DEX9_FIRST];
  return "?";
}
// Fills `out` with species of `type` (primary or secondary); skips legendaries unless allowed.
inline uint16_t dexAllPoolForType(uint8_t type, bool allowLegendary, uint16_t maxDex, uint16_t *out, uint16_t cap) {
  uint16_t n = 0;
  for (uint16_t d = 1; d <= DEXALL_COUNT && d <= maxDex && n < cap; d++) {
    const DexEntry *e = dexAllEntry(d);
    if (!e) continue;
    if ((e->type1 == type || e->type2 == type) && (allowLegendary || e->rarity != R_LEGENDARIO)) out[n++] = d;
  }
  return n;
}
