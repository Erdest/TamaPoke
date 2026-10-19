#include <assert.h>
#include <stdio.h>
#include "../pokeballs.h"
#include "../dex.h"

static CatchContext ctx(uint8_t pl, uint8_t wl, uint8_t hp) {
  CatchContext c = {}; c.petLevel = pl; c.wildLevel = wl; c.wildHpPct = hp; c.round = 1; c.hour = 12;
  c.wildType1 = TYPE_FIRE; return c;
}

int main() {
  BallInventory inv; ballInventoryInit(inv);
  assert(inv.count[BALL_GREAT] == 5 && ballAvailable(inv, BALL_POKE) && !ballAvailable(inv, BALL_ULTRA));
  assert(ballConsume(inv, BALL_POKE) && ballConsume(inv, BALL_GREAT) && inv.count[BALL_GREAT] == 4);
  assert(ballAdd(inv, BALL_ULTRA, 200) && inv.count[BALL_ULTRA] == 99 && !ballAdd(inv, BALL_POKE, 1));

  CatchContext c = ctx(20, 20, 100);           // 50 base
  assert(catchRatePct(BALL_POKE, c) == 50);
  assert(catchRatePct(BALL_GREAT, c) == 75 && catchRatePct(BALL_ULTRA, c) == 99);
  c = ctx(10, 30, 100); assert(catchRatePct(BALL_POKE, c) == 10);   // 50-40
  c = ctx(1, 90, 100);  assert(catchRatePct(BALL_POKE, c) == 5);    // clamp low
  c = ctx(20, 20, 0);   assert(catchRatePct(BALL_POKE, c) == 83);   // 50+33
  assert(catchRatePct(BALL_MASTER, c) == 100);

  c = ctx(20, 20, 100); c.wildType2 = TYPE_BUG;
  assert(ballModifierPct(BALL_NET, c) == 300);
  c.wildType2 = TYPE_NONE; assert(ballModifierPct(BALL_NET, c) == 100);
  c.hour = 17; assert(ballModifierPct(BALL_DUSK, c) == 100);
  c.hour = 18; assert(ballModifierPct(BALL_DUSK, c) == 300);
  c.hour = 5;  assert(ballModifierPct(BALL_DUSK, c) == 300);
  c.hour = 6;  assert(ballModifierPct(BALL_DUSK, c) == 100);
  c.round = 1; assert(ballModifierPct(BALL_TIMER, c) == 100 && ballModifierPct(BALL_QUICK, c) == 400);
  c.round = 2; assert(ballModifierPct(BALL_TIMER, c) == 130 && ballModifierPct(BALL_QUICK, c) == 100);
  c.round = 30; assert(ballModifierPct(BALL_TIMER, c) == 400);

  const uint8_t low[3] = {0, 0, 0}, fail2[3] = {0, 0, 99};
  c = ctx(20, 20, 100);
  CatchOutcome o = resolveCatch(BALL_MASTER, c, 99, fail2, 99, 99);
  assert(o.caught && o.shakes == 3 && !o.critical);
  o = resolveCatch(BALL_POKE, c, 3, fail2, 99, 99);   // 4% crit
  assert(o.caught && o.critical && o.shakes == 1);
  o = resolveCatch(BALL_POKE, c, 4, fail2, 0, 99);    // no crit, 3rd shake fails
  assert(!o.caught && o.shakes == 2);
  o = resolveCatch(BALL_POKE, c, 4, low, 49, 99);     // rate 50, final 49 -> caught
  assert(o.caught && o.shakes == 3);
  o = resolveCatch(BALL_POKE, c, 4, low, 50, 99);
  assert(!o.caught && o.shakes == 3);
  o = resolveCatch(BALL_LUXURY, c, 4, low, 0, 99); assert(o.bondBonus == 10);
  o = resolveCatch(BALL_PREMIER, c, 4, low, 0, 4);  assert(o.shinyRoll);
  o = resolveCatch(BALL_PREMIER, c, 4, low, 0, 5);  assert(!o.shinyRoll);
  c.trailBonusPct = 5; assert(catchRatePct(BALL_POKE, c) == 55);
  puts("pokeball tests passed");
  return 0;
}
