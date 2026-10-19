#include "expedition.h"
#include "pokeballs.h"
#include "garden.h"

uint16_t expeditionMinutes(uint8_t l) { return l == EXP_30 ? 30 : l == EXP_60 ? 60 : 15; }
uint8_t expeditionEnergyCost(uint8_t l) { return l == EXP_30 ? 20 : l == EXP_60 ? 40 : 10; }

bool expeditionStart(Expedition &e, uint8_t len, uint8_t energy, uint8_t &energyOut, uint32_t now) {
  energyOut = energy;
  if (e.active || len > EXP_60) return false;
  uint8_t cost = expeditionEnergyCost(len);
  if (energy < cost) return false;                     // "cannot start if energy too low"
  energyOut = (uint8_t)(energy - cost);
  e.active = 1; e.len = len; e.startedAt = now;
  return true;
}
bool expeditionReady(const Expedition &e, uint32_t now) {
  return e.active && now - e.startedAt >= (uint32_t)expeditionMinutes(e.len) * 60u;
}
uint32_t expeditionSecondsLeft(const Expedition &e, uint32_t now) {
  if (!e.active) return 0;
  uint32_t total = (uint32_t)expeditionMinutes(e.len) * 60u, el = now - e.startedAt;
  return el >= total ? 0 : total - el;
}

Loot rollLoot(uint8_t cat, uint8_t sub, uint8_t acc) {
  Loot l = {LOOT_NONE, 0, acc < 15};
  if (cat < 20) {                                      // 20% Poke Balls
    static const uint8_t W[5] = {BALL_GREAT, BALL_NET, BALL_DUSK, BALL_TIMER, BALL_QUICK};
    static const uint8_t CUM[5] = {35, 55, 75, 90, 100};   // weighted: 35/20/20/15/10
    uint8_t i = 0; while (sub % 100 >= CUM[i]) i++;
    l.kind = LOOT_BALL; l.param = W[i];
  } else if (cat < 25) {                               // 5% rare trade item
    l.kind = LOOT_TRADE_ITEM; l.param = (uint8_t)(1 + sub % (TI_COUNT - 1));
  } else if (cat < 40) {                               // 15% uncommon seed
    l.kind = LOOT_BERRY_SEED; l.param = (uint8_t)(sub % BERRY_COUNT);
  } else {                                             // 60% consumables
    static const uint8_t K[4] = {LOOT_SNACK, LOOT_ENERGY_DRINK, LOOT_CARE_KIT, LOOT_TRAINING_WEIGHT};
    l.kind = K[sub % 4];
    if (l.kind == LOOT_TRAINING_WEIGHT) l.param = (uint8_t)((sub / 4) % 3);
  }
  return l;
}

bool expeditionCollect(Expedition &e, ExpBag &bag, uint32_t now, uint8_t c, uint8_t s, uint8_t a, Loot *out) {
  if (!expeditionReady(e, now) || bag.count >= EXP_BAG_MAX) return false;   // bag full: keep expedition
  Loot l = rollLoot(c, s, a);
  bag.item[bag.count++] = l;
  e.active = 0;
  if (out) *out = l;
  return true;
}
bool bagRemove(ExpBag &bag, uint8_t i) {
  if (i >= bag.count) return false;
  for (uint8_t k = i; k + 1 < bag.count; k++) bag.item[k] = bag.item[k + 1];
  bag.count--; return true;
}
