#pragma once
// Achievement system, 51 achievements (feature list section 28).
#include <stdint.h>

enum AchCategory : uint8_t { AC_COLLECTION = 0, AC_GROWTH, AC_COMBAT, AC_ACTIVITY };
constexpr uint8_t ACH_COUNT = 51;
constexpr uint8_t ACH_BANNER_QUEUE = 4;

enum AchReward : uint8_t {
  AR_NONE = 0, AR_EGG_RARITY_PLUS1,   // Hatch 100: +1 egg rarity tier permanently
  AR_SHINY_MINUS4,                    // 100-day streak
  AR_CROWN_TITLE,                     // Complete National Dex
  AR_GRAND_CHAMPION,                  // Beat all 9 Champions: title + shiny legendary egg
  AR_EXPLORER_BAG,                    // Walk 250K steps
  AR_MONOCLE,                         // Quiz: 500 correct
};
struct AchDef { const char *name; uint8_t category; uint16_t goal; uint8_t reward; };

// Counter slots the game feeds (progress is "max seen" or running total per slot)
enum AchStat : uint8_t {
  AS_HATCHED = 0, AS_REG_KANTO, AS_REG_JOHTO, AS_REG_HOENN, AS_TYPES_CAUGHT, AS_REGISTERED, AS_SHINIES,
  AS_MAX_LEVEL, AS_STREAK, AS_BOND, AS_FINAL_EVOS, AS_FAREWELLS, AS_MEDALS,
  AS_WINS, AS_WIN_STREAK, AS_BADGES, AS_CHAMPIONS, AS_PVP_WINS,
  AS_STEPS_K, AS_PHOTOS, AS_QUIZ_CORRECT, AS_BERRIES, AS_EXPEDITIONS, AS_MINIGAMES, AS_WONDER, AS_CATCHES,
  AS_DAILY_DAYS, AS_MEGAS, AS_QUIZ_PERFECT, AS_COUNT
};

struct AchState {
  uint16_t progress[ACH_COUNT];   // 102 bytes
  uint8_t unlocked[7];            // 51 bits
  uint8_t queue[ACH_BANNER_QUEUE];
  uint8_t queueLen;
};
const AchDef &achDef(uint8_t id);
uint8_t achStat(uint8_t id);                     // which counter feeds this achievement
void achInit(AchState &s);
bool achUnlocked(const AchState &s, uint8_t id);
uint8_t achUnlockedCount(const AchState &s);
uint8_t achCountInCategory(uint8_t category);
// Update one counter: `value` is the new absolute value (progress keeps the max).
// Returns how many achievements unlocked by this update (also queued for banners).
uint8_t achReport(AchState &s, uint8_t stat, uint32_t value);
uint8_t achAddTo(AchState &s, uint8_t stat, uint32_t delta, uint32_t current);   // convenience: report(current+delta)
bool achPopBanner(AchState &s, uint8_t *id);     // oldest first
bool achHasReward(const AchState &s, uint8_t reward);
