#include "extras.h"
#include <string.h>

void extrasInit(ExtrasState &x) {
  memset(&x, 0, sizeof(x));
  x.version = EXTRAS_VERSION;
  gardenInit(x.garden);
  progressInit(x.gyms);
  achInit(x.ach);
  accInit(x.acc);
  // starter seeds so the garden is usable on day one
  gardenAddSeed(x.garden, BERRY_RED, 2);
  gardenAddSeed(x.garden, BERRY_BLUE, 2);
  gardenAddSeed(x.garden, BERRY_GREEN, 2);
}

bool extrasLoad(ExtrasState &x, const void *blob, uint32_t len) {
  if (!blob || len != sizeof(ExtrasState)) return false;
  ExtrasState tmp;
  memcpy(&tmp, blob, sizeof(tmp));
  if (tmp.version != EXTRAS_VERSION) return false;
  x = tmp;
  return true;
}

uint8_t extrasSetStat(ExtrasState &x, uint8_t stat, uint32_t value) {
  if (stat >= AS_COUNT) return 0;
  if (value > x.stat[stat]) x.stat[stat] = value;
  return achReport(x.ach, stat, x.stat[stat]);
}

uint8_t extrasBump(ExtrasState &x, uint8_t stat, uint32_t delta) {
  if (stat >= AS_COUNT) return 0;
  x.stat[stat] += delta;
  return achReport(x.ach, stat, x.stat[stat]);
}

uint8_t extrasSyncFacts(ExtrasState &x, const PetFacts &f) {
  uint8_t n = 0;
  n += extrasSetStat(x, AS_REG_KANTO, f.regKanto);
  n += extrasSetStat(x, AS_REG_JOHTO, f.regJohto);
  n += extrasSetStat(x, AS_REG_HOENN, f.regHoenn);
  n += extrasSetStat(x, AS_REGISTERED, f.registered);
  n += extrasSetStat(x, AS_SHINIES, f.shinies);
  n += extrasSetStat(x, AS_TYPES_CAUGHT, f.typesCaught);
  n += extrasSetStat(x, AS_MAX_LEVEL, f.maxLevel);
  n += extrasSetStat(x, AS_STREAK, f.streak);
  n += extrasSetStat(x, AS_BOND, f.bond);
  n += extrasSetStat(x, AS_FINAL_EVOS, f.finalEvos);
  n += extrasSetStat(x, AS_FAREWELLS, f.farewells);
  n += extrasSetStat(x, AS_MEDALS, f.medals);
  n += extrasSetStat(x, AS_WINS, f.wins);
  n += extrasSetStat(x, AS_WIN_STREAK, f.winStreak);
  n += extrasSetStat(x, AS_STEPS_K, f.stepsTotal / 1000u);
  n += extrasSetStat(x, AS_BADGES, totalBadges(x.gyms));
  n += extrasSetStat(x, AS_CHAMPIONS, championsBeaten(x.gyms));
  return n;
}

uint8_t extrasDailySeed(ExtrasState &x, uint32_t dayNumber, uint8_t roll) {
  uint8_t d = (uint8_t)(dayNumber & 0xFF);
  if (x.seedDay == d && x.stat[AS_DAILY_DAYS] != 0) return 255;
  x.seedDay = d;
  extrasBump(x, AS_DAILY_DAYS, 1);
  uint8_t type = (uint8_t)(roll % 3);   // red / blue / green
  gardenAddSeed(x.garden, type, 1);
  return type;
}

HarvestResult extrasHarvest(ExtrasState &x, uint8_t slot, uint32_t now, uint8_t yieldRoll) {
  HarvestResult r = gardenHarvest(x.garden, slot, now, yieldRoll);
  if (r.ok && !r.wilted && r.berries) extrasBump(x, AS_BERRIES, r.berries);
  return r;
}

ChallengeResult trainerBegin(ExtrasState &x, TrainerRun &run, TrainerKind kind, uint8_t region,
                             uint8_t index, uint8_t playerLevel, uint32_t seed,
                             const uint16_t *pool, uint16_t poolLen) {
  memset(&run, 0, sizeof(run));
  ChallengeResult c;
  if (kind == TK_GYM) c = canChallengeGym(x.gyms, region, index, playerLevel);
  else if (kind == TK_E4) {
    c = canChallengeE4(x.gyms, region);
    if (c == CH_OK) {
      int8_t n = nextE4(x.gyms, region);
      if (n < 0) return CH_ALREADY_BEATEN;
      index = (uint8_t)n;
    }
  } else c = canChallengeChampion(x.gyms, region);
  if (c != CH_OK) return c;
  RematchTier tier = rematchTier(x.gyms, region);
  uint8_t n = buildTeam(kind, region, index, playerLevel, tier, pool, poolLen, seed, run.team);
  if (n == 0) return CH_BAD_INDEX;
  run.active = 1; run.kind = kind; run.region = region; run.index = index;
  run.tier = tier; run.len = n; run.pos = 0;
  return CH_OK;
}

ChallengeResult trainerRematchBegin(ExtrasState &x, TrainerRun &run, uint8_t region,
                                    uint8_t playerLevel, uint32_t seed,
                                    const uint16_t *pool, uint16_t poolLen) {
  memset(&run, 0, sizeof(run));
  if (region >= REGION_COUNT) return CH_BAD_INDEX;
  if (!regionComplete(x.gyms, region)) return CH_NEED_E4;
  RematchTier tier = rematchTier(x.gyms, region);
  uint8_t n = buildTeam(TK_CHAMPION, region, 0, playerLevel, tier, pool, poolLen, seed, run.team);
  if (n == 0) return CH_BAD_INDEX;
  run.active = 1; run.kind = TK_CHAMPION; run.region = region; run.index = 0;
  run.tier = tier; run.len = n; run.pos = 0;
  return CH_OK;
}

RematchBonus extrasRematchBonus(ExtrasState &x, BallInventory &balls, uint32_t dayNumber) {
  RematchBonus b = {};
  uint8_t d = (uint8_t)(dayNumber & 0xFF);
  if (d == 0) d = 1;            // 0 means "never paid"
  if (x.rematchDay == d) return b;
  x.rematchDay = d;
  ballAdd(balls, BALL_ULTRA, 2);
  b.paid = true; b.joy = 10; b.bond = 3;
  return b;
}

TrainerOutcome trainerMemberResult(ExtrasState &x, TrainerRun &run, BallInventory &balls,
                                   bool playerWon, uint8_t accRoll, uint32_t accPickRoll) {
  TrainerOutcome out = {};
  out.accessory = ACC_NONE;
  if (!run.active) return out;
  if (!playerWon) { run.active = 0; out.finished = true; return out; }
  run.pos++;
  if (run.pos < run.len) return out;
  run.active = 0;
  out.finished = out.won = true;
  bool firstChampion = championsBeaten(x.gyms) == 0;
  if (run.kind == TK_GYM) out.rewards = awardGymWin(x.gyms, run.region, run.index);
  else if (run.kind == TK_E4) out.rewards = awardE4Win(x.gyms, run.region, run.index);
  else out.rewards = awardChampionWin(x.gyms, run.region, firstChampion);
  ballAdd(balls, BALL_ULTRA, out.rewards.ultraBalls);
  ballAdd(balls, BALL_LUXURY, out.rewards.luxuryBalls);
  ballAdd(balls, BALL_MASTER, out.rewards.masterBalls);
  if (out.rewards.legendaryEgg && x.pendingEgg < 1) x.pendingEgg = 1;
  if (out.rewards.shinyLegendaryEgg) x.pendingEgg = 2;
  bool drop = out.rewards.accessoryGuaranteedRare || (out.rewards.accessoryChancePct && gymDropsAccessory(accRoll));
  if (drop) {
    uint8_t maxRarity = out.rewards.accessoryGuaranteedRare ? AR_RARE : AR_UNCOMMON;
    uint8_t id = accPickDrop(x.acc, maxRarity, accPickRoll);
    if (id != ACC_NONE && accGive(x.acc, id)) out.accessory = id;
  }
  out.newAchievements += extrasSetStat(x, AS_BADGES, totalBadges(x.gyms));
  out.newAchievements += extrasSetStat(x, AS_CHAMPIONS, championsBeaten(x.gyms));
  return out;
}

const char *challengeText(ChallengeResult r) {
  switch (r) {
    case CH_OK: return "Ready";
    case CH_BAD_INDEX: return "Unavailable";
    case CH_ALREADY_BEATEN: return "Beaten";
    case CH_PREV_GYM_MISSING: return "Beat previous gym";
    case CH_LEVEL_TOO_LOW: return "Level too low";
    case CH_NEED_ALL_BADGES: return "Need 8 badges";
    case CH_NEED_E4: return "Beat Elite Four";
    default: return "?";
  }
}
