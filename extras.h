#pragma once
// Aggregated persistent state for the Gen 9 feature set (gyms, garden,
// achievements, accessories, Wonder Trade) and the glue logic between them.
// Pure logic, host-testable. The firmware stores ExtrasState as one NVS blob.
#include <stdint.h>
#include "garden.h"
#include "gyms.h"
#include "achievements.h"
#include "accessories.h"
#include "steps.h"
#include "pokeballs.h"

constexpr uint8_t EXTRAS_VERSION = 1;

struct ExtrasState {
  uint8_t version;
  uint8_t pendingEgg;          // 0 none, 1 legendary, 2 shiny legendary (applied by the next egg)
  uint8_t seedDay;             // low byte of the last day a daily seed was granted
  uint8_t reserved;
  uint32_t stat[AS_COUNT];     // lifetime counters feeding achievements
  Garden garden;
  GymProgress gyms;
  AchState ach;
  AccState acc;
  WonderState wonder;
};

void extrasInit(ExtrasState &x);
// Replace x with the stored blob if it is the right size and version.
bool extrasLoad(ExtrasState &x, const void *blob, uint32_t len);

// Achievements ---------------------------------------------------------
uint8_t extrasSetStat(ExtrasState &x, uint8_t stat, uint32_t value);   // max-style, returns newly unlocked
uint8_t extrasBump(ExtrasState &x, uint8_t stat, uint32_t delta);

struct PetFacts {
  uint16_t hatched;       // not tracked by Pet; bumped by events instead (ignored if 0)
  uint16_t regKanto, regJohto, regHoenn, registered, shinies;
  uint8_t typesCaught, maxLevel, finalEvos, farewells, medals;
  uint16_t streak, wins, winStreak;
  uint8_t bond;
  uint32_t stepsTotal;
};
uint8_t extrasSyncFacts(ExtrasState &x, const PetFacts &f);

// Daily seed: first call each day grants one basic berry seed. Returns type or 255.
uint8_t extrasDailySeed(ExtrasState &x, uint32_t dayNumber, uint8_t roll);

// Garden helpers -------------------------------------------------------
// Harvest and account for achievements/pending stats. Returns the result.
HarvestResult extrasHarvest(ExtrasState &x, uint8_t slot, uint32_t now, uint8_t yieldRoll);

// Trainer fights -------------------------------------------------------
struct TrainerRun {
  uint8_t active, kind, region, index, tier;
  uint8_t pos, len;
  TeamMon team[TEAM_MAX];
};

ChallengeResult trainerBegin(ExtrasState &x, TrainerRun &run, TrainerKind kind, uint8_t region,
                             uint8_t index, uint8_t playerLevel, uint32_t seed,
                             const uint16_t *pool, uint16_t poolLen);

struct TrainerOutcome {
  bool finished;        // trainer fight over (won all or lost)
  bool won;             // trainer defeated
  Rewards rewards;      // valid when finished && won
  uint8_t accessory;    // ACC_NONE or id dropped
  uint8_t newAchievements;
};
// Call after each member: win advances; loss ends the run with no progress.
TrainerOutcome trainerMemberResult(ExtrasState &x, TrainerRun &run, BallInventory &balls,
                                   bool playerWon, uint8_t accRoll, uint32_t accPickRoll);

// Texts used by the UI --------------------------------------------------
const char *challengeText(ChallengeResult r);
