#pragma once
// Expeditions and loot (feature list section 11). RTC-timestamp based.
#include <stdint.h>

enum ExpeditionLen : uint8_t { EXP_15 = 0, EXP_30, EXP_60 };
enum LootKind : uint8_t {
  LOOT_NONE = 0, LOOT_SNACK, LOOT_ENERGY_DRINK, LOOT_CARE_KIT, LOOT_TRAINING_WEIGHT,
  LOOT_BALL, LOOT_TRADE_ITEM, LOOT_BERRY_SEED,
};
enum TradeItem : uint8_t {   // spec lists "13 items" but names 14 (Deep Sea Tooth and Scale)
  TI_NONE = 0, TI_METAL_COAT, TI_KINGS_ROCK, TI_DRAGON_SCALE, TI_UP_GRADE, TI_DUBIOUS_DISC,
  TI_PROTECTOR, TI_ELECTIRIZER, TI_MAGMARIZER, TI_REAPER_CLOTH, TI_SACHET, TI_WHIPPED_DREAM,
  TI_PRISM_SCALE, TI_DEEP_SEA_TOOTH, TI_DEEP_SEA_SCALE, TI_COUNT
};
constexpr uint8_t EXP_BAG_MAX = 3;

struct Loot {
  uint8_t kind;      // LootKind
  uint8_t param;     // ball type / trade item / berry type / stat (0 ATK,1 DEF,2 SPE)
  bool accessory;    // separate 15% accessory drop
};
struct Expedition { uint8_t active; uint8_t len; uint32_t startedAt; };  // active: 0 idle,1 running
struct ExpBag { Loot item[EXP_BAG_MAX]; uint8_t count; };

uint16_t expeditionMinutes(uint8_t len);       // 15 / 30 / 60
uint8_t expeditionEnergyCost(uint8_t len);     // 10 / 20 / 40
bool expeditionStart(Expedition &e, uint8_t len, uint8_t energy, uint8_t &energyOut, uint32_t now);
bool expeditionReady(const Expedition &e, uint32_t now);
uint32_t expeditionSecondsLeft(const Expedition &e, uint32_t now);
// Roll loot. rolls are 0..99 (deterministic for tests).
Loot rollLoot(uint8_t catRoll, uint8_t subRoll, uint8_t accRoll);
bool expeditionCollect(Expedition &e, ExpBag &bag, uint32_t now, uint8_t catRoll, uint8_t subRoll, uint8_t accRoll, Loot *out);
bool bagRemove(ExpBag &bag, uint8_t idx);
