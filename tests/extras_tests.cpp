#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../extras.h"
#include "../dex.h"

int main() {
  ExtrasState x;
  extrasInit(x);
  assert(x.version == EXTRAS_VERSION && x.garden.seeds[BERRY_RED] == 2);
  assert(sizeof(ExtrasState) < 1000);

  // blob round trip and rejection of bad blobs
  ExtrasState y; memset(&y, 0, sizeof(y));
  assert(extrasLoad(y, &x, sizeof(x)) && y.garden.seeds[BERRY_BLUE] == 2);
  assert(!extrasLoad(y, &x, sizeof(x) - 1));
  ExtrasState bad = x; bad.version = 99;
  assert(!extrasLoad(y, &bad, sizeof(bad)));

  // achievements via counters
  for (int i = 0; i < 9; i++) assert(extrasBump(x, AS_WINS, 1) == 0);
  assert(extrasBump(x, AS_WINS, 1) == 1);          // Win 10 battles
  uint8_t id; assert(achPopBanner(x.ach, &id) && achUnlocked(x.ach, id));
  PetFacts f = {}; f.registered = 120; f.streak = 8;
  assert(extrasSyncFacts(x, f) >= 2);              // Register 100, 7-day streak

  // daily seed: once per day
  uint32_t seeds0 = x.garden.seeds[0] + x.garden.seeds[1] + x.garden.seeds[2];
  assert(extrasDailySeed(x, 20000, 1) != 255);
  assert(extrasDailySeed(x, 20000, 1) == 255);
  assert(extrasDailySeed(x, 20001, 2) != 255);
  assert(x.garden.seeds[0] + x.garden.seeds[1] + x.garden.seeds[2] == (int)seeds0 + 2);

  // garden harvest counts berries
  assert(gardenPlant(x.garden, 0, BERRY_RED, 1000));
  uint32_t ready = x.garden.slot[0].readyAt + 1;
  HarvestResult hr = extrasHarvest(x, 0, ready, 50);
  assert(hr.ok && hr.berries > 0 && x.stat[AS_BERRIES] == hr.berries);

  // trainer fight: first gym needs level 10, then wins advance
  uint16_t pool[128]; uint16_t n = 0;
  for (uint16_t d = 1; d <= 100; d++) pool[n++] = d;
  TrainerRun run; BallInventory balls; ballInventoryInit(balls);
  assert(trainerBegin(x, run, TK_GYM, 0, 0, 5, 1, pool, n) == CH_LEVEL_TOO_LOW);
  assert(trainerBegin(x, run, TK_GYM, 0, 1, 50, 1, pool, n) == CH_PREV_GYM_MISSING);
  assert(trainerBegin(x, run, TK_GYM, 0, 0, 20, 1, pool, n) == CH_OK && run.len == gymTeamSize(0));
  for (uint8_t i = 0; i + 1 < run.len; i++) {
    TrainerOutcome o = trainerMemberResult(x, run, balls, true, 0, 0);
    assert(!o.finished);
  }
  TrainerOutcome o = trainerMemberResult(x, run, balls, true, 0, 0);
  assert(o.finished && o.won && !run.active && badgesIn(x.gyms, 0) == 1);
  assert(trainerBegin(x, run, TK_GYM, 0, 0, 20, 1, pool, n) == CH_ALREADY_BEATEN);

  // a loss ends the run without progress
  assert(trainerBegin(x, run, TK_GYM, 0, 1, 25, 2, pool, n) == CH_OK);
  o = trainerMemberResult(x, run, balls, false, 0, 0);
  assert(o.finished && !o.won && badgesIn(x.gyms, 0) == 1);

  // full region: 8 gyms, E4 x4, champion -> legendary egg + master ball
  for (uint8_t g = 1; g < 8; g++) {
    assert(trainerBegin(x, run, TK_GYM, 0, g, 60, 3, pool, n) == CH_OK);
    while (run.active) o = trainerMemberResult(x, run, balls, true, 99, 0);
  }
  assert(badgesIn(x.gyms, 0) == 8);
  for (uint8_t e = 0; e < 4; e++) {
    assert(trainerBegin(x, run, TK_E4, 0, 0, 80, 4, pool, n) == CH_OK && run.index == e);
    while (run.active) o = trainerMemberResult(x, run, balls, true, 99, 0);
  }
  assert(trainerBegin(x, run, TK_CHAMPION, 0, 0, 90, 5, pool, n) == CH_OK);
  while (run.active) o = trainerMemberResult(x, run, balls, true, 99, 7);
  assert(o.won && x.pendingEgg == 1 && balls.count[BALL_MASTER] == 1);
  assert(o.accessory != ACC_NONE);   // champion: guaranteed rare drop
  assert(championsBeaten(x.gyms) == 1);
  assert(x.stat[AS_BADGES] == 8 && achUnlocked(x.ach, 31));

  puts("extras tests passed");
  return 0;
}
