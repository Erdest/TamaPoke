#include "pokeballs.h"
#include <string.h>
#include "dex.h"  // TYPE_* ids

void ballInventoryInit(BallInventory &inv) {
  memset(&inv, 0, sizeof(inv));
  inv.count[BALL_GREAT] = BALL_STARTER_GREAT;
}

bool ballAvailable(const BallInventory &inv, BallType b) {
  if (b >= BALL_COUNT) return false;
  return b == BALL_POKE || inv.count[b] > 0;
}

bool ballConsume(BallInventory &inv, BallType b) {
  if (!ballAvailable(inv, b)) return false;
  if (b != BALL_POKE) inv.count[b]--;
  return true;
}

bool ballAdd(BallInventory &inv, BallType b, uint8_t n) {
  if (b >= BALL_COUNT || b == BALL_POKE) return false;
  uint16_t v = (uint16_t)inv.count[b] + n;
  inv.count[b] = v > BALL_MAX_STACK ? BALL_MAX_STACK : (uint8_t)v;
  return true;
}

bool isNightHour(uint8_t hour) { return hour >= 18 || hour <= 5; }

uint16_t ballModifierPct(BallType b, const CatchContext &c) {
  switch (b) {
    case BALL_GREAT: return 150;
    case BALL_ULTRA: return 200;
    case BALL_NET:
      return (c.wildType1 == TYPE_WATER || c.wildType1 == TYPE_BUG ||
              c.wildType2 == TYPE_WATER || c.wildType2 == TYPE_BUG) ? 300 : 100;
    case BALL_DUSK: return isNightHour(c.hour) ? 300 : 100;
    case BALL_TIMER: {
      uint16_t m = 100 + 30 * (c.round > 0 ? c.round - 1 : 0);
      return m > 400 ? 400 : m;
    }
    case BALL_QUICK: return c.round <= 1 ? 400 : 100;
    default: return 100;  // Poke, Luxury, Premier (Master bypasses)
  }
}

uint8_t catchRatePct(BallType b, const CatchContext &c) {
  if (b == BALL_MASTER) return 100;
  int rate = 50 + ((int)c.petLevel - (int)c.wildLevel) * 2 +
             (100 - (int)(c.wildHpPct > 100 ? 100 : c.wildHpPct)) / 3;
  rate += c.trailBonusPct;
  rate = rate * (int)ballModifierPct(b, c) / 100;
  if (rate < 5) rate = 5;
  if (rate > 99) rate = 99;
  return (uint8_t)rate;
}

CatchOutcome resolveCatch(BallType b, const CatchContext &c, uint8_t critRoll,
                          const uint8_t shakeRolls[3], uint8_t finalRoll,
                          uint8_t shinyRoll) {
  CatchOutcome o = {};
  o.ratePct = catchRatePct(b, c);
  o.bondBonus = (b == BALL_LUXURY) ? 10 : 0;
  o.shinyRoll = (b == BALL_PREMIER) && shinyRoll < 5;
  if (b == BALL_MASTER) { o.caught = true; o.shakes = 3; return o; }
  if (critRoll < CRIT_CATCH_PCT) {
    o.caught = true; o.critical = true; o.shakes = 1; return o;
  }
  for (int i = 0; i < 3; i++) {
    if (shakeRolls[i] >= o.ratePct) { o.shakes = (uint8_t)i; return o; }  // broke free
  }
  o.shakes = 3;
  o.caught = finalRoll < o.ratePct;
  return o;
}
