#include <assert.h>
#include <stdio.h>
#include "../garden.h"
int main() {
  Garden g; gardenInit(g);
  const uint32_t T0 = 1000000, H = 3600;
  assert(!gardenPlant(g, 0, BERRY_RED, T0));               // no seed
  gardenAddSeed(g, BERRY_RED, 2); gardenAddSeed(g, BERRY_ENIGMA, 120);
  assert(g.seeds[BERRY_ENIGMA] == 99);
  assert(gardenPlant(g, 0, BERRY_RED, T0) && g.seeds[BERRY_RED] == 1);
  assert(!gardenPlant(g, 0, BERRY_RED, T0));               // occupied
  assert(gardenState(g, 0, T0 + 1) == SLOT_GROWING && gardenSecondsLeft(g, 0, T0) == 2 * H);
  assert(!gardenHarvest(g, 0, T0 + H, 0).ok);              // not ready
  // water: -10% of 2h = 720 s; cooldown 1 h; max 4
  assert(gardenWater(g, 0, T0));
  assert(gardenSecondsLeft(g, 0, T0) == 2 * H - 720);
  assert(!gardenWater(g, 0, T0 + 100));                    // cooldown
  assert(gardenWater(g, 0, T0 + H) && gardenSecondsLeft(g, 0, T0 + H) == H - 720 - 720);
  assert(gardenMulch(g, 0, T0 + H) && !gardenMulch(g, 0, T0 + H));
  assert(gardenSecondsLeft(g, 0, T0 + H) == (H - 1440) / 2);
  // perfect care on a 8h Starf berry: 4 waterings at 1h spacing
  assert(gardenAddSeed(g, BERRY_STARF, 1) && gardenPlant(g, 1, BERRY_STARF, T0));
  for (int i = 0; i < 4; i++) assert(gardenWater(g, 1, T0 + i * H));
  assert(!gardenWater(g, 1, T0 + 4 * H));                  // 5th denied
  uint32_t ready = T0 + 8 * H - 4 * (8 * H / 10);
  assert(gardenState(g, 1, ready - 1) == SLOT_GROWING && gardenState(g, 1, ready) == SLOT_READY);
  HarvestResult r = gardenHarvest(g, 1, ready + 10, 7);
  assert(r.ok && r.berries == 1 && r.seedsBack == 2 && r.perfect && !r.wilted);
  assert(g.berries[BERRY_STARF] == 1 && gardenState(g, 1, ready) == SLOT_EMPTY);
  // wilting: ready + >24h => half yield, 1 seed, no perfect bonus
  gardenAddSeed(g, BERRY_BLUE, 1); assert(gardenPlant(g, 2, BERRY_BLUE, T0));
  uint32_t rd = T0 + 2 * H;
  assert(gardenState(g, 2, rd + 24 * H) == SLOT_READY && gardenState(g, 2, rd + 24 * H + 1) == SLOT_WILTED);
  r = gardenHarvest(g, 2, rd + 30 * H, 2);                 // yield 3 + 2%3=5 -> half = 2
  assert(r.ok && r.wilted && r.berries == 2 && r.seedsBack == 1 && !r.perfect);
  // yield ranges and bag cap / consumption
  for (uint8_t t = 0; t < BERRY_COUNT; t++) {
    gardenAddSeed(g, t, 1); assert(gardenPlant(g, 3, t, T0));
    r = gardenHarvest(g, 3, T0 + 9 * H, 255);
    assert(r.ok && r.berries >= berryDef(t).yieldMin && r.berries <= berryDef(t).yieldMax);
  }
  assert(gardenTakeBerry(g, BERRY_RED) && !gardenTakeBerry(g, 200));
  assert(sizeof(GardenSlot) == 12 || sizeof(GardenSlot) == 16);
  puts("garden tests passed"); return 0;
}
