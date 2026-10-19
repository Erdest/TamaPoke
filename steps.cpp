#include "steps.h"
uint32_t milestoneSteps(uint8_t m) { return m == SM_BRONZE ? 1000 : m == SM_SILVER ? 3000 : m == SM_GOLD ? 5000 : 0; }
uint8_t milestoneReached(uint32_t s) { return s >= 5000 ? SM_GOLD : s >= 3000 ? SM_SILVER : s >= 1000 ? SM_BRONZE : SM_NONE; }
StepReward milestoneReward(uint8_t m) {
  StepReward r = {};
  if (m == SM_BRONZE) r.snack = true;
  else if (m == SM_SILVER) r.energyDrink = true;
  else if (m == SM_GOLD) { r.trainingWeight = true; r.premierBalls = 1; }
  return r;
}
uint8_t trailRank(uint32_t s) {
  return s >= 250000 ? TR_EXPLORER : s >= 100000 ? TR_HIKER : s >= 25000 ? TR_JOGGER : s >= 5000 ? TR_WALKER : TR_HATCHLING;
}
uint8_t trailShinyReduction(uint8_t r) { return r >= TR_EXPLORER ? 8 : r >= TR_JOGGER ? 4 : 0; }
uint8_t trailCatchBonusPct(uint8_t r) { return r >= TR_HIKER ? 5 : r == TR_JOGGER ? 3 : r == TR_WALKER ? 2 : 0; }
bool trailGivesWalkJoy(uint8_t r) { return r >= TR_WALKER; }
bool trailExplorerBag(uint8_t r) { return r >= TR_EXPLORER; }

bool wonderTradeAllowed(WonderState &w, uint16_t today) {
  if (w.dayStamp != today) { w.dayStamp = today; w.used = 0; }
  return w.used < WONDER_TRADES_PER_DAY;
}
bool wonderTradeConsume(WonderState &w, uint16_t today) {
  if (!wonderTradeAllowed(w, today)) return false;
  w.used++; return true;
}
PvpReward pvpReward(bool won) { return won ? PvpReward{15, 10, true} : PvpReward{5, 0, false}; }
