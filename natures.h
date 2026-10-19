#pragma once
// Nature system (feature list section 30). Pure logic, host-testable.
#include <stdint.h>

enum NatureStat : uint8_t { NSTAT_NONE = 0, NSTAT_ATK, NSTAT_DEF, NSTAT_SPE };
enum NatureBerry : uint8_t { NBERRY_NONE = 0, NBERRY_RED, NBERRY_BLUE, NBERRY_GREEN };
constexpr uint8_t NATURE_COUNT = 25;

struct NatureInfo {
  const char *name;
  NatureStat plus, minus;
  int8_t foodDrainMod, joyDrainMod, poopChanceMod, sleepRecovMod, bondMod;
  NatureBerry berryPref;
  uint8_t animMs;
};

const NatureInfo &natureInfo(uint8_t nature);          // clamps to 0..24
uint8_t natureFromRoll(uint32_t rnd);                   // rnd % 25
uint8_t natureStatPct(uint8_t nature, NatureStat stat); // 110 / 90 / 100
uint16_t natureApplyStat(uint8_t nature, NatureStat stat, uint16_t base);
uint8_t natureFoodDrain(uint8_t nature, bool night);    // per minute, >= 0
uint8_t natureJoyDrain(uint8_t nature);
uint8_t naturePoopChancePct(uint8_t nature);            // min 1
uint8_t natureSleepRecov(uint8_t nature);               // ENE per minute
int8_t natureBondPerTick(uint8_t nature);
bool natureLikesBerry(uint8_t nature, uint8_t berryFlavor);  // 0 red,1 blue,2 green

// Localized names in repo Lang order: ES, EN, FR, DE, IT, PT (ASCII only; PT falls back to EN).
const char *natureLocalName(uint8_t nature, uint8_t lang);
const char *natureLabel(uint8_t lang);   // "Nature" heading
