#include "dexbits.h"
#include <string.h>

static bool getb(const uint8_t *b, uint16_t d) { return d >= 1 && d <= DEX_TOTAL && ((b[(d - 1) >> 3] >> ((d - 1) & 7)) & 1); }
static bool setb(uint8_t *b, uint16_t d) {
  if (d < 1 || d > DEX_TOTAL) return false;
  b[(d - 1) >> 3] |= (uint8_t)(1u << ((d - 1) & 7)); return true;
}
static uint16_t count(const uint8_t *b) { uint16_t n = 0; for (uint16_t d = 1; d <= DEX_TOTAL; d++) n += getb(b, d); return n; }

void dexInit(DexBits &d) { memset(&d, 0, sizeof(d)); }
bool dexMarkRaised(DexBits &d, uint16_t dex, bool sh) { if (!setb(d.raised, dex)) return false; if (sh) setb(d.shiny, dex); return true; }
bool dexMarkCaught(DexBits &d, uint16_t dex, bool sh) { if (!setb(d.caught, dex)) return false; if (sh) setb(d.shiny, dex); return true; }
bool dexIsRaised(const DexBits &d, uint16_t x) { return getb(d.raised, x); }
bool dexIsCaught(const DexBits &d, uint16_t x) { return getb(d.caught, x); }
bool dexIsShiny(const DexBits &d, uint16_t x) { return getb(d.shiny, x); }
bool dexRegistered(const DexBits &d, uint16_t x) { return getb(d.raised, x) || getb(d.caught, x); }
uint16_t dexCountRaised(const DexBits &d) { return count(d.raised); }
uint16_t dexCountCaught(const DexBits &d) { return count(d.caught); }
uint16_t dexCountShiny(const DexBits &d) { return count(d.shiny); }
uint16_t dexCountRegistered(const DexBits &d) { uint16_t n = 0; for (uint16_t x = 1; x <= DEX_TOTAL; x++) n += dexRegistered(d, x); return n; }
bool dexMatchesFilter(const DexBits &d, uint16_t x, DexFilter f) {
  if (x < 1 || x > DEX_TOTAL) return false;
  return f == DF_RAISED ? dexIsRaised(d, x) : f == DF_CAUGHT ? dexIsCaught(d, x) : true;
}
uint16_t dexFilteredCount(const DexBits &d, DexFilter f) {
  uint16_t n = 0; for (uint16_t x = 1; x <= DEX_TOTAL; x++) n += dexMatchesFilter(d, x, f); return n;
}
int16_t dexNextFiltered(const DexBits &d, int16_t from, int8_t dir, DexFilter f) {
  for (int16_t x = from + dir; x >= 1 && x <= (int16_t)DEX_TOTAL; x += dir) if (dexMatchesFilter(d, (uint16_t)x, f)) return x;
  return -1;
}
static const uint16_t FR[FRAME_COUNT] = {10, 25, 50, 100, 200, 400, 700, 1025};
uint16_t frameThreshold(uint8_t f) { return FR[f < FRAME_COUNT ? f : FRAME_COUNT - 1]; }
uint8_t framesUnlocked(uint16_t r) { uint8_t n = 0; for (uint8_t i = 0; i < FRAME_COUNT; i++) n += r >= FR[i]; return n; }
bool canEquipFrame(uint16_t r, uint8_t f) { return f < FRAME_COUNT && r >= FR[f]; }
