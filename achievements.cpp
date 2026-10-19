#include "achievements.h"
#include <string.h>

static const AchDef DEFS[ACH_COUNT] = {
  {"Hatch 10", 0, 10, AR_NONE},
  {"Hatch 50", 0, 50, AR_NONE},
  {"Hatch 100", 0, 100, AR_EGG_RARITY_PLUS1},
  {"Hatch 200", 0, 200, AR_NONE},
  {"Complete Kanto Dex", 0, 151, AR_NONE},
  {"Complete Johto Dex", 0, 100, AR_NONE},
  {"Complete Hoenn Dex", 0, 135, AR_NONE},
  {"Catch every type", 0, 18, AR_NONE},
  {"Register 100", 0, 100, AR_NONE},
  {"Register 500", 0, 500, AR_NONE},
  {"Complete National Dex", 0, 1025, AR_CROWN_TITLE},
  {"Find a Shiny", 0, 1, AR_NONE},
  {"Raise to Lv.50", 1, 50, AR_NONE},
  {"Raise to Lv.100", 1, 100, AR_NONE},
  {"7-day care streak", 1, 7, AR_NONE},
  {"30-day care streak", 1, 30, AR_NONE},
  {"100-day care streak", 1, 100, AR_SHINY_MINUS4},
  {"365-day care streak", 1, 365, AR_NONE},
  {"Bond 50", 1, 50, AR_NONE},
  {"Max bond", 1, 100, AR_NONE},
  {"First final form", 1, 1, AR_NONE},
  {"10 final forms", 1, 10, AR_NONE},
  {"50 final forms", 1, 50, AR_NONE},
  {"First farewell", 1, 1, AR_NONE},
  {"10 farewells", 1, 10, AR_NONE},
  {"All 8 medals", 1, 8, AR_NONE},
  {"Win 10 battles", 2, 10, AR_NONE},
  {"Win 50 battles", 2, 50, AR_NONE},
  {"Win 100 battles", 2, 100, AR_NONE},
  {"5-win streak", 2, 5, AR_NONE},
  {"10-win streak", 2, 10, AR_NONE},
  {"Earn 8 badges", 2, 8, AR_NONE},
  {"Earn all 72 badges", 2, 72, AR_NONE},
  {"Beat a Champion", 2, 1, AR_NONE},
  {"Beat all Champions", 2, 9, AR_GRAND_CHAMPION},
  {"Win a PvP battle", 2, 1, AR_NONE},
  {"Win 10 PvP battles", 2, 10, AR_NONE},
  {"Walk 50K steps", 3, 50, AR_NONE},
  {"Walk 250K steps", 3, 250, AR_EXPLORER_BAG},
  {"Take 20 photos", 3, 20, AR_NONE},
  {"Quiz: 100 correct", 3, 100, AR_NONE},
  {"Quiz: 500 correct", 3, 500, AR_MONOCLE},
  {"Grow 10 berries", 3, 10, AR_NONE},
  {"Grow 50 berries", 3, 50, AR_NONE},
  {"10 expeditions", 3, 10, AR_NONE},
  {"50 expeditions", 3, 50, AR_NONE},
  {"Play 50 minigames", 3, 50, AR_NONE},
  {"10 Wonder Trades", 3, 10, AR_NONE},
  {"Catch 25 wild", 3, 25, AR_NONE},
  {"30 days of daily goals", 3, 30, AR_NONE},
  {"Mega Evolve 10 times", 3, 10, AR_NONE},
};
static const uint8_t STATS[ACH_COUNT] = {
  AS_HATCHED,
  AS_HATCHED,
  AS_HATCHED,
  AS_HATCHED,
  AS_REG_KANTO,
  AS_REG_JOHTO,
  AS_REG_HOENN,
  AS_TYPES_CAUGHT,
  AS_REGISTERED,
  AS_REGISTERED,
  AS_REGISTERED,
  AS_SHINIES,
  AS_MAX_LEVEL,
  AS_MAX_LEVEL,
  AS_STREAK,
  AS_STREAK,
  AS_STREAK,
  AS_STREAK,
  AS_BOND,
  AS_BOND,
  AS_FINAL_EVOS,
  AS_FINAL_EVOS,
  AS_FINAL_EVOS,
  AS_FAREWELLS,
  AS_FAREWELLS,
  AS_MEDALS,
  AS_WINS,
  AS_WINS,
  AS_WINS,
  AS_WIN_STREAK,
  AS_WIN_STREAK,
  AS_BADGES,
  AS_BADGES,
  AS_CHAMPIONS,
  AS_CHAMPIONS,
  AS_PVP_WINS,
  AS_PVP_WINS,
  AS_STEPS_K,
  AS_STEPS_K,
  AS_PHOTOS,
  AS_QUIZ_CORRECT,
  AS_QUIZ_CORRECT,
  AS_BERRIES,
  AS_BERRIES,
  AS_EXPEDITIONS,
  AS_EXPEDITIONS,
  AS_MINIGAMES,
  AS_WONDER,
  AS_CATCHES,
  AS_DAILY_DAYS,
  AS_MEGAS,
};

const AchDef &achDef(uint8_t id) { return DEFS[id < ACH_COUNT ? id : 0]; }
uint8_t achStat(uint8_t id) { return STATS[id < ACH_COUNT ? id : 0]; }
void achInit(AchState &s) { memset(&s, 0, sizeof(s)); }
bool achUnlocked(const AchState &s, uint8_t id) { return id < ACH_COUNT && ((s.unlocked[id >> 3] >> (id & 7)) & 1); }
uint8_t achUnlockedCount(const AchState &s) { uint8_t n = 0; for (uint8_t i = 0; i < ACH_COUNT; i++) n += achUnlocked(s, i); return n; }
uint8_t achCountInCategory(uint8_t c) { uint8_t n = 0; for (uint8_t i = 0; i < ACH_COUNT; i++) n += DEFS[i].category == c; return n; }

uint8_t achReport(AchState &s, uint8_t stat, uint32_t value) {
  uint8_t unlockedNow = 0;
  for (uint8_t i = 0; i < ACH_COUNT; i++) {
    if (STATS[i] != stat) continue;
    uint16_t v = value > 65535u ? 65535u : (uint16_t)value;
    if (v > s.progress[i]) s.progress[i] = v;
    if (!achUnlocked(s, i) && s.progress[i] >= DEFS[i].goal) {
      s.unlocked[i >> 3] |= (uint8_t)(1u << (i & 7));
      unlockedNow++;
      if (s.queueLen < ACH_BANNER_QUEUE) s.queue[s.queueLen++] = i;   // banner queue of 4; overflow still unlocks
    }
  }
  return unlockedNow;
}
uint8_t achAddTo(AchState &s, uint8_t stat, uint32_t delta, uint32_t current) { return achReport(s, stat, current + delta); }
bool achPopBanner(AchState &s, uint8_t *id) {
  if (!s.queueLen) return false;
  if (id) *id = s.queue[0];
  for (uint8_t i = 0; i + 1 < s.queueLen; i++) s.queue[i] = s.queue[i + 1];
  s.queueLen--; return true;
}
bool achHasReward(const AchState &s, uint8_t reward) {
  for (uint8_t i = 0; i < ACH_COUNT; i++) if (DEFS[i].reward == reward && achUnlocked(s, i)) return true;
  return false;
}
