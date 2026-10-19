#include "garden.h"
#include <string.h>

static const BerryDef DEFS[BERRY_COUNT] = {
  {"Red", 2, 3, 5}, {"Blue", 2, 3, 5}, {"Green", 2, 3, 5},
  {"Protein", 4, 2, 3}, {"Iron", 4, 2, 3}, {"Carbos", 4, 2, 3},
  {"Sitrus", 3, 2, 4}, {"Lum", 3, 2, 4},
  {"Liechi", 6, 1, 2}, {"Petaya", 6, 1, 2}, {"Salac", 6, 1, 2},
  {"Enigma", 8, 1, 1}, {"Starf", 8, 1, 1},
};
const BerryDef &berryDef(uint8_t t) { return DEFS[t < BERRY_COUNT ? t : 0]; }

void gardenInit(Garden &g) { memset(&g, 0, sizeof(g)); }

static void addCapped(uint8_t &v, uint16_t n) {
  uint16_t s = (uint16_t)v + n;
  v = s > BERRY_BAG_MAX ? BERRY_BAG_MAX : (uint8_t)s;
}
bool gardenAddSeed(Garden &g, uint8_t t, uint8_t n) {
  if (t >= BERRY_COUNT) return false;
  addCapped(g.seeds[t], n);
  return true;
}

bool gardenPlant(Garden &g, uint8_t s, uint8_t t, uint32_t now) {
  if (s >= GARDEN_SLOTS || t >= BERRY_COUNT || g.slot[s].used || g.seeds[t] == 0) return false;
  g.seeds[t]--;
  GardenSlot &sl = g.slot[s];
  sl = GardenSlot();
  sl.type = t; sl.used = 1; sl.plantedAt = now;
  sl.readyAt = now + (uint32_t)berryDef(t).growHours * 3600u;
  return true;
}

SlotState gardenState(const Garden &g, uint8_t s, uint32_t now) {
  if (s >= GARDEN_SLOTS || !g.slot[s].used) return SLOT_EMPTY;
  const GardenSlot &sl = g.slot[s];
  if (now < sl.readyAt) return SLOT_GROWING;
  return (now - sl.readyAt > WILT_AFTER_READY_S) ? SLOT_WILTED : SLOT_READY;
}

uint32_t gardenSecondsLeft(const Garden &g, uint8_t s, uint32_t now) {
  if (gardenState(g, s, now) != SLOT_GROWING) return 0;
  return g.slot[s].readyAt - now;
}

bool gardenWater(Garden &g, uint8_t s, uint32_t now) {
  if (gardenState(g, s, now) != SLOT_GROWING) return false;
  GardenSlot &sl = g.slot[s];
  if (sl.waterCount >= MAX_WATERINGS) return false;
  if (sl.waterCount > 0 && now - sl.lastWatered < WATER_COOLDOWN_S) return false;
  uint32_t cut = (uint32_t)berryDef(sl.type).growHours * 3600u / 10u;  // 10% of total
  uint32_t left = sl.readyAt - now;
  sl.readyAt = now + (cut >= left ? 0 : left - cut);
  sl.waterCount++;
  sl.lastWatered = now;
  return true;
}

bool gardenMulch(Garden &g, uint8_t s, uint32_t now) {
  if (gardenState(g, s, now) != SLOT_GROWING || g.slot[s].mulched) return false;
  GardenSlot &sl = g.slot[s];
  sl.readyAt = now + (sl.readyAt - now) / 2;
  sl.mulched = 1;
  return true;
}

HarvestResult gardenHarvest(Garden &g, uint8_t s, uint32_t now, uint8_t roll) {
  HarvestResult r = {};
  SlotState st = gardenState(g, s, now);
  if (st != SLOT_READY && st != SLOT_WILTED) return r;
  GardenSlot &sl = g.slot[s];
  const BerryDef &d = berryDef(sl.type);
  uint8_t span = d.yieldMax - d.yieldMin + 1;
  uint8_t n = d.yieldMin + roll % span;
  r.wilted = (st == SLOT_WILTED);
  if (r.wilted) { n = n / 2; if (n == 0) n = 1; }   // half yield, never below 1
  r.ok = true; r.type = sl.type; r.berries = n; r.seedsBack = 1;
  r.perfect = !r.wilted && sl.waterCount >= MAX_WATERINGS;
  if (r.perfect) r.seedsBack++;                      // bonus seed
  addCapped(g.berries[sl.type], n);
  addCapped(g.seeds[sl.type], r.seedsBack);
  sl = GardenSlot();
  return r;
}

bool gardenTakeBerry(Garden &g, uint8_t t) {
  if (t >= BERRY_COUNT || g.berries[t] == 0) return false;
  g.berries[t]--;
  return true;
}
