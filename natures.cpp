#include "natures.h"

#define N NSTAT_NONE
#define A NSTAT_ATK
#define D NSTAT_DEF
#define S NSTAT_SPE
#define BN NBERRY_NONE
#define BR NBERRY_RED
#define BB NBERRY_BLUE
#define BG NBERRY_GREEN

static const NatureInfo TABLE[NATURE_COUNT] = {
  {"Hardy",   N,N, 0, 0, 0, 0, 0, BN, 200}, {"Lonely",  A,D, 0, 1, 0, 0, 0, BR, 200},
  {"Brave",   A,S, 1, 0, 2, 1,-1, BR, 180}, {"Adamant", A,N, 1, 0, 1, 0,-1, BR, 170},
  {"Naughty", A,D, 0,-1, 4,-1, 0, BR, 150}, {"Bold",    D,A, 0, 0,-1, 0, 1, BB, 210},
  {"Docile",  N,N, 0, 0, 0, 0, 0, BN, 200}, {"Relaxed", D,S,-1,-1, 0, 2, 0, BB, 230},
  {"Impish",  D,N, 0,-1, 1, 0, 0, BB, 180}, {"Lax",     D,N, 0,-1, 2, 1, 0, BB, 220},
  {"Timid",   S,A,-1, 1,-2,-1, 0, BG, 160}, {"Hasty",   S,D, 0, 0, 0,-1, 0, BG, 150},
  {"Serious", N,N, 0, 0, 0, 0, 0, BN, 200}, {"Jolly",   S,N, 0,-2, 0, 0, 1, BG, 150},
  {"Naive",   S,D, 0,-2, 1, 0, 0, BG, 160}, {"Modest",  N,A,-1, 0,-1, 1, 0, BB, 250},
  {"Mild",    N,D, 0, 0, 0, 0, 0, BB, 200}, {"Quiet",   N,S, 0, 0,-2, 3, 0, BB, 250},
  {"Bashful", N,N, 0, 1, 0, 0, 0, BN, 200}, {"Rash",    N,D, 0, 0, 2,-1,-1, BR, 170},
  {"Calm",    N,A,-1,-1,-3, 2, 1, BG, 230}, {"Gentle",  N,D,-1, 0,-4, 1, 2, BG, 220},
  {"Sassy",   N,S, 0, 0, 1, 1, 0, BR, 200}, {"Careful", N,N, 0, 0,-2, 1, 1, BG, 210},
  {"Quirky",  N,N, 0, 0, 0, 0, 0, BN, 200},
};

const NatureInfo &natureInfo(uint8_t n) { return TABLE[n < NATURE_COUNT ? n : 0]; }
uint8_t natureFromRoll(uint32_t rnd) { return (uint8_t)(rnd % NATURE_COUNT); }

uint8_t natureStatPct(uint8_t n, NatureStat s) {
  if (s == NSTAT_NONE) return 100;
  const NatureInfo &i = natureInfo(n);
  if (i.plus == s && i.minus != s) return 110;
  if (i.minus == s && i.plus != s) return 90;
  return 100;  // +/- same stat cancels
}
uint16_t natureApplyStat(uint8_t n, NatureStat s, uint16_t base) {
  return (uint16_t)((uint32_t)base * natureStatPct(n, s) / 100);
}
static uint8_t clampNonNeg(int v) { return v < 0 ? 0 : (uint8_t)v; }
uint8_t natureFoodDrain(uint8_t n, bool night) { return clampNonNeg((night ? 1 : 2) + natureInfo(n).foodDrainMod); }
uint8_t natureJoyDrain(uint8_t n) { return clampNonNeg(1 + natureInfo(n).joyDrainMod); }
uint8_t naturePoopChancePct(uint8_t n) {
  int v = 15 + natureInfo(n).poopChanceMod;
  return v < 1 ? 1 : (uint8_t)v;
}
uint8_t natureSleepRecov(uint8_t n) { return clampNonNeg(6 + natureInfo(n).sleepRecovMod); }
int8_t natureBondPerTick(uint8_t n) { return natureInfo(n).bondMod; }
bool natureLikesBerry(uint8_t n, uint8_t flavor) {
  NatureBerry p = natureInfo(n).berryPref;
  return p != NBERRY_NONE && (uint8_t)p == flavor + 1;
}
