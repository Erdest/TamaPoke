#pragma once
// Egg, hatching, rarity and shiny odds (feature list section 5).
#include <stdint.h>

enum EggRarity : uint8_t { ER_COMMON = 0, ER_RARE, ER_LEGENDARY };
constexpr uint8_t HATCH_TAPS = 3;
constexpr uint16_t HATCH_AUTO_SECONDS = 180;
constexpr uint8_t LEGENDARY_MIN_REGISTERED = 25;

struct EggBlessings {            // flags that affect the NEXT egg
  bool farewell;                 // clean farewell: better rarity, 1/24 shiny
  bool runaway;                  // curse: forces Common
  bool enigma;                   // +1 rarity tier
  bool starf;                    // shiny odds 1/8
  bool hatch100;                 // achievement: +1 rarity tier permanently
};
uint8_t eggLegendaryPct(const EggBlessings &b);   // 3 / 10
uint8_t eggRarePct(const EggBlessings &b);        // 27 / 45
// roll 0..99. Returns tier after blessings/curses.
uint8_t rollEggRarity(uint8_t roll, const EggBlessings &b, uint16_t registeredCount);
uint8_t eggCrackStage(uint8_t taps);              // 0..3
bool eggHatches(uint8_t taps, uint16_t secondsSinceStart);

struct ShinyInputs {
  bool farewell, starf;
  uint8_t careStreakDays, bond;     // care bonus
  uint8_t trailPerk;                // denominator reduction from Trail Rank (0, 4, 8)
  uint8_t achievementPerk;          // 4 with the 100-day streak achievement
  bool pokemonDay;                  // odds doubled
};
uint8_t careShinyReduction(uint8_t streakDays, uint8_t bond);   // 0..16
uint16_t shinyDenominator(const ShinyInputs &in);               // floor 8 (Starf = exactly 8)
bool rollShiny(uint16_t denom, uint32_t roll);                  // roll % denom == 0

uint8_t geneFromRoll(uint8_t roll);                             // 90..110 (%)
uint8_t personalityOf(uint16_t species, bool shiny, uint32_t ageAtHatch);   // 0..4
