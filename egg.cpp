#include "egg.h"

uint8_t eggLegendaryPct(const EggBlessings &b) { return b.farewell ? 10 : 3; }
uint8_t eggRarePct(const EggBlessings &b) { return b.farewell ? 45 : 27; }

uint8_t rollEggRarity(uint8_t roll, const EggBlessings &b, uint16_t registered) {
  if (b.runaway) return ER_COMMON;                          // curse forces Common
  bool legendOk = registered >= LEGENDARY_MIN_REGISTERED;
  uint8_t tier;
  if (roll < eggLegendaryPct(b) && legendOk) tier = ER_LEGENDARY;
  else if (roll < eggLegendaryPct(b) + eggRarePct(b)) tier = ER_RARE;
  else tier = ER_COMMON;
  if (b.enigma && tier < ER_LEGENDARY) tier++;
  if (b.hatch100 && tier < ER_LEGENDARY) tier++;
  if (tier == ER_LEGENDARY && !legendOk) tier = ER_RARE;    // legendaries stay locked below 25 registered
  return tier;
}
uint8_t eggCrackStage(uint8_t taps) { return taps > HATCH_TAPS ? HATCH_TAPS : taps; }
bool eggHatches(uint8_t taps, uint16_t secs) { return taps >= HATCH_TAPS || secs >= HATCH_AUTO_SECONDS; }

uint8_t careShinyReduction(uint8_t streak, uint8_t bond) {
  int r = streak / 7 + bond / 10;     // 1 per week of streak + 1 per 10 bond
  return (uint8_t)(r > 16 ? 16 : r);
}
uint16_t shinyDenominator(const ShinyInputs &in) {
  if (in.starf) return 8;
  int d = in.farewell ? 24 : 48;
  d -= careShinyReduction(in.careStreakDays, in.bond);
  d -= in.trailPerk;
  d -= in.achievementPerk;
  if (in.pokemonDay) d /= 2;
  return (uint16_t)(d < 8 ? 8 : d);
}
bool rollShiny(uint16_t denom, uint32_t roll) { return denom && roll % denom == 0; }
uint8_t geneFromRoll(uint8_t r) { return (uint8_t)(90 + r % 21); }
uint8_t personalityOf(uint16_t sp, bool sh, uint32_t age) { return (uint8_t)((sp + (sh ? 1u : 0u) + age) % 5); }
