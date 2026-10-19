#pragma once
// Berry Garden (feature list section 31). RTC-timestamp based, host-testable.
// NOTE: the spec says "12 berry types" but its table lists 13 (3 basic, 3 stat,
// 2 battle, 3 rare, 2 special); all 13 are implemented here.
#include <stdint.h>

enum BerryType : uint8_t {
  BERRY_RED = 0, BERRY_BLUE, BERRY_GREEN,
  BERRY_PROTEIN, BERRY_IRON, BERRY_CARBOS,
  BERRY_SITRUS, BERRY_LUM,
  BERRY_LIECHI, BERRY_PETAYA, BERRY_SALAC,
  BERRY_ENIGMA, BERRY_STARF,
  BERRY_COUNT
};

constexpr uint8_t GARDEN_SLOTS = 6;
constexpr uint8_t BERRY_BAG_MAX = 99;
constexpr uint8_t MAX_WATERINGS = 4;
constexpr uint32_t WATER_COOLDOWN_S = 3600;
constexpr uint32_t WILT_AFTER_READY_S = 24u * 3600u;

struct BerryDef { const char *name; uint8_t growHours; uint8_t yieldMin, yieldMax; };
const BerryDef &berryDef(uint8_t type);

enum SlotState : uint8_t { SLOT_EMPTY = 0, SLOT_GROWING, SLOT_READY, SLOT_WILTED };

struct GardenSlot {          // 16 bytes in NVS
  uint8_t type;
  uint8_t waterCount;
  uint8_t mulched;
  uint8_t used;              // 0 empty, 1 planted
  uint32_t plantedAt;        // RTC seconds
  uint32_t readyAt;
  uint32_t lastWatered;
};

struct Garden {
  GardenSlot slot[GARDEN_SLOTS];
  uint8_t seeds[BERRY_COUNT];
  uint8_t berries[BERRY_COUNT];
};

struct HarvestResult { bool ok; uint8_t type; uint8_t berries; uint8_t seedsBack; bool wilted; bool perfect; };

void gardenInit(Garden &g);                    // empty garden, no seeds
bool gardenAddSeed(Garden &g, uint8_t type, uint8_t n);
bool gardenPlant(Garden &g, uint8_t slot, uint8_t type, uint32_t now);
SlotState gardenState(const Garden &g, uint8_t slot, uint32_t now);
uint32_t gardenSecondsLeft(const Garden &g, uint8_t slot, uint32_t now);
bool gardenWater(Garden &g, uint8_t slot, uint32_t now);  // -10% of total time each, max 4, 1 h cooldown
bool gardenMulch(Garden &g, uint8_t slot, uint32_t now);  // once per cycle, halves remaining
// yieldRoll: any value, mapped into [yieldMin,yieldMax]
HarvestResult gardenHarvest(Garden &g, uint8_t slot, uint32_t now, uint8_t yieldRoll);
bool gardenTakeBerry(Garden &g, uint8_t type);            // consume one from the bag
