#include "regional.h"
#include "dex.h"   // TYPE_*

static const RegionalDef TABLE[] = {
  {19, RF_ALOLAN, TYPE_DARK, TYPE_NORMAL, 0, false},
  {20, RF_ALOLAN, TYPE_DARK, TYPE_NORMAL, 0, false},
  {26, RF_ALOLAN, TYPE_ELECTRIC, TYPE_PSYCHIC, 0, false},
  {27, RF_ALOLAN, TYPE_ICE, TYPE_STEEL, 0, false},
  {28, RF_ALOLAN, TYPE_ICE, TYPE_STEEL, 0, false},
  {37, RF_ALOLAN, TYPE_ICE, TYPE_NONE, 0, false},
  {38, RF_ALOLAN, TYPE_ICE, TYPE_FAIRY, 0, false},
  {50, RF_ALOLAN, TYPE_GROUND, TYPE_STEEL, 0, false},
  {51, RF_ALOLAN, TYPE_GROUND, TYPE_STEEL, 0, false},
  {52, RF_ALOLAN, TYPE_DARK, TYPE_NONE, 0, false},
  {53, RF_ALOLAN, TYPE_DARK, TYPE_NONE, 0, false},
  {74, RF_ALOLAN, TYPE_ROCK, TYPE_ELECTRIC, 0, false},
  {75, RF_ALOLAN, TYPE_ROCK, TYPE_ELECTRIC, 0, false},
  {76, RF_ALOLAN, TYPE_ROCK, TYPE_ELECTRIC, 0, false},
  {88, RF_ALOLAN, TYPE_POISON, TYPE_DARK, 0, false},
  {89, RF_ALOLAN, TYPE_POISON, TYPE_DARK, 0, false},
  {103, RF_ALOLAN, TYPE_GRASS, TYPE_DRAGON, 0, false},
  {105, RF_ALOLAN, TYPE_FIRE, TYPE_GHOST, 0, false},
  {52, RF_GALARIAN, TYPE_STEEL, TYPE_NONE, 0, false},
  {77, RF_GALARIAN, TYPE_PSYCHIC, TYPE_NONE, 0, false},
  {78, RF_GALARIAN, TYPE_PSYCHIC, TYPE_FAIRY, 0, false},
  {79, RF_GALARIAN, TYPE_PSYCHIC, TYPE_NONE, 0, false},
  {80, RF_GALARIAN, TYPE_POISON, TYPE_PSYCHIC, 0, false},
  {199, RF_GALARIAN, TYPE_POISON, TYPE_PSYCHIC, 0, false},
  {83, RF_GALARIAN, TYPE_FIGHTING, TYPE_NONE, 0, false},
  {110, RF_GALARIAN, TYPE_POISON, TYPE_FAIRY, 0, false},
  {122, RF_GALARIAN, TYPE_ICE, TYPE_PSYCHIC, 0, false},
  {222, RF_GALARIAN, TYPE_GHOST, TYPE_NONE, 0, false},
  {263, RF_GALARIAN, TYPE_DARK, TYPE_NORMAL, 0, false},
  {264, RF_GALARIAN, TYPE_DARK, TYPE_NORMAL, 0, false},
  {554, RF_GALARIAN, TYPE_ICE, TYPE_NONE, 0, false},
  {555, RF_GALARIAN, TYPE_ICE, TYPE_NONE, 0, false},
  {562, RF_GALARIAN, TYPE_GROUND, TYPE_GHOST, 0, false},
  {618, RF_GALARIAN, TYPE_GROUND, TYPE_STEEL, 0, false},
  {58, RF_HISUIAN, TYPE_FIRE, TYPE_ROCK, 0, false},
  {59, RF_HISUIAN, TYPE_FIRE, TYPE_ROCK, 0, false},
  {100, RF_HISUIAN, TYPE_ELECTRIC, TYPE_GRASS, 0, false},
  {101, RF_HISUIAN, TYPE_ELECTRIC, TYPE_GRASS, 0, false},
  {157, RF_HISUIAN, TYPE_FIRE, TYPE_GHOST, 0, false},
  {211, RF_HISUIAN, TYPE_DARK, TYPE_POISON, 0, false},
  {215, RF_HISUIAN, TYPE_FIGHTING, TYPE_POISON, 0, false},
  {503, RF_HISUIAN, TYPE_WATER, TYPE_DARK, 0, false},
  {549, RF_HISUIAN, TYPE_GRASS, TYPE_FIGHTING, 0, false},
  {570, RF_HISUIAN, TYPE_NORMAL, TYPE_GHOST, 0, false},
  {571, RF_HISUIAN, TYPE_NORMAL, TYPE_GHOST, 0, false},
  {628, RF_HISUIAN, TYPE_PSYCHIC, TYPE_FLYING, 0, false},
  {705, RF_HISUIAN, TYPE_STEEL, TYPE_DRAGON, 0, false},
  {706, RF_HISUIAN, TYPE_STEEL, TYPE_DRAGON, 0, false},
  {713, RF_HISUIAN, TYPE_ICE, TYPE_ROCK, 0, false},
  {724, RF_HISUIAN, TYPE_GRASS, TYPE_FIGHTING, 0, false},
  {194, RF_PALDEAN, TYPE_POISON, TYPE_GROUND, 0, false},
  {128, RF_PALDEAN, TYPE_FIGHTING, TYPE_NONE, 0, false},
  {128, RF_PALDEAN, TYPE_FIGHTING, TYPE_FIRE, 1, false},
  {128, RF_PALDEAN, TYPE_FIGHTING, TYPE_WATER, 2, false},
  {866, RF_GALARIAN, TYPE_ICE, TYPE_PSYCHIC, 0, true},
  {864, RF_GALARIAN, TYPE_GHOST, TYPE_NONE, 0, true},
  {862, RF_GALARIAN, TYPE_DARK, TYPE_NORMAL, 0, true},
  {863, RF_GALARIAN, TYPE_STEEL, TYPE_NONE, 0, true},
  {865, RF_GALARIAN, TYPE_FIGHTING, TYPE_NONE, 0, true},
  {867, RF_GALARIAN, TYPE_GROUND, TYPE_GHOST, 0, true},
  {904, RF_HISUIAN, TYPE_DARK, TYPE_POISON, 0, true},
  {903, RF_HISUIAN, TYPE_FIGHTING, TYPE_POISON, 0, true},
  {980, RF_PALDEAN, TYPE_POISON, TYPE_GROUND, 0, true},
};
static const RegionalEvo EVOS[] = {
  {52, RF_GALARIAN, 863, RF_GALARIAN}, {83, RF_GALARIAN, 865, RF_GALARIAN}, {264, RF_GALARIAN, 862, RF_GALARIAN},
  {222, RF_GALARIAN, 864, RF_GALARIAN}, {562, RF_GALARIAN, 867, RF_GALARIAN}, {122, RF_GALARIAN, 866, RF_GALARIAN},
  {211, RF_HISUIAN, 904, RF_HISUIAN}, {215, RF_HISUIAN, 903, RF_HISUIAN}, {194, RF_PALDEAN, 980, RF_PALDEAN},
};
static const uint8_t N = (uint8_t)(sizeof(TABLE) / sizeof(TABLE[0]));

uint8_t regionalCount() { return N; }
const RegionalDef &regionalAt(uint8_t i) { return TABLE[i < N ? i : 0]; }
int16_t regionalIndex(uint16_t dex, uint8_t form, uint8_t variant) {
  for (uint8_t i = 0; i < N; i++)
    if (TABLE[i].dex == dex && TABLE[i].form == form && TABLE[i].variant == variant) return i;
  return -1;
}
uint8_t regionalFormsFor(uint16_t dex, uint8_t out[4]) {
  uint8_t n = 0;
  for (uint8_t i = 0; i < N && n < 4; i++)
    if (TABLE[i].dex == dex && !TABLE[i].fromEvolution) out[n++] = i;
  return n;
}
uint8_t regionalHatchChancePct(uint8_t bond) { int v = 20 + bond; return (uint8_t)(v > 50 ? 50 : v); }
int16_t rollRegionalHatch(uint16_t dex, uint8_t bond, uint8_t chanceRoll, uint8_t pickRoll) {
  uint8_t idx[4]; uint8_t n = regionalFormsFor(dex, idx);
  if (!n || chanceRoll >= regionalHatchChancePct(bond)) return -1;
  return idx[pickRoll % n];
}
bool regionalEvolution(uint16_t dex, uint8_t form, RegionalEvo *out) {
  for (const RegionalEvo &e : EVOS)
    if (e.fromDex == dex && e.fromForm == form) { if (out) *out = e; return true; }
  return false;
}
bool regionalSeen(const uint8_t *b, uint8_t i) { return i < REGIONAL_BITMAP_BYTES * 8 && (b[i >> 3] >> (i & 7)) & 1; }
void regionalMarkSeen(uint8_t *b, uint8_t i) { if (i < REGIONAL_BITMAP_BYTES * 8) b[i >> 3] |= (uint8_t)(1u << (i & 7)); }
uint8_t regionalSeenCount(const uint8_t *b) {
  uint8_t n = 0; for (uint8_t i = 0; i < N; i++) n += regionalSeen(b, i); return n;
}
