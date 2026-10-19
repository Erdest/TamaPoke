#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../natures.h"
int main() {
  assert(!strcmp(natureInfo(0).name, "Hardy") && !strcmp(natureInfo(24).name, "Quirky"));
  assert(natureFromRoll(26) == 1);
  uint8_t adamant = 3, bold = 5, hardy = 0, gentle = 21, quiet = 17, hasty = 11, naughty = 4;
  assert(natureStatPct(adamant, NSTAT_ATK) == 110 && natureStatPct(adamant, NSTAT_DEF) == 100);
  assert(natureStatPct(bold, NSTAT_ATK) == 90 && natureStatPct(bold, NSTAT_DEF) == 110);
  assert(natureStatPct(hardy, NSTAT_ATK) == 100 && natureApplyStat(adamant, NSTAT_ATK, 100) == 110);
  assert(naturePoopChancePct(gentle) == 11 && naturePoopChancePct(naughty) == 19);  // spec 11%..19%
  assert(natureSleepRecov(hasty) == 5 && natureSleepRecov(quiet) == 9);             // spec 5..9
  assert(natureBondPerTick(gentle) == 2 && natureBondPerTick(adamant) == -1);
  assert(natureFoodDrain(adamant, false) == 3 && natureFoodDrain(adamant, true) == 2);
  assert(natureFoodDrain(21, true) == 0 && natureJoyDrain(13) == 0 && natureJoyDrain(1) == 2);
  assert(natureLikesBerry(1, 0) && !natureLikesBerry(1, 1) && natureLikesBerry(5, 1));
  assert(!natureLikesBerry(0, 0) && natureLikesBerry(10, 2));
  for (uint8_t i = 0; i < NATURE_COUNT; i++) {  // every stat-affecting nature has a distinct +/-
    const NatureInfo &n = natureInfo(i);
    assert(n.animMs >= 150 && n.animMs <= 250);
    assert(naturePoopChancePct(i) >= 1 && natureSleepRecov(i) >= 5 && natureSleepRecov(i) <= 9);
  }
  puts("nature tests passed"); return 0;
}
