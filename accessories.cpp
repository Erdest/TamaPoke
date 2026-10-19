#include "accessories.h"
#include <string.h>

static const AccDef DEFS[ACC_COUNT] = {
  {"Red Cap", AR_COMMON},
  {"Trainer Hat", AR_COMMON},
  {"Sun Hat", AR_COMMON},
  {"Bandana", AR_COMMON},
  {"Flower Wreath", AR_UNCOMMON},
  {"Wizard Hat", AR_UNCOMMON},
  {"Crown", AR_RARE},
  {"Angel Halo", AR_SPECIAL},
  {"Round Glasses", AR_COMMON},
  {"Cool Shades", AR_COMMON},
  {"Eye Patch", AR_COMMON},
  {"Mask", AR_COMMON},
  {"Fake Mustache", AR_COMMON},
  {"Star Glasses", AR_UNCOMMON},
  {"Heart Glasses", AR_UNCOMMON},
  {"Monocle", AR_RARE},
  {"Red Scarf", AR_COMMON},
  {"Blue Scarf", AR_COMMON},
  {"Bow Tie", AR_COMMON},
  {"Bell Collar", AR_COMMON},
  {"Gold Chain", AR_UNCOMMON},
  {"Pearl Necklace", AR_UNCOMMON},
  {"Medal Ribbon", AR_UNCOMMON},
  {"Rainbow Lei", AR_RARE},
  {"Backpack", AR_COMMON},
  {"Parachute", AR_UNCOMMON},
  {"Hero Cape", AR_UNCOMMON},
  {"Leaf Cloak", AR_UNCOMMON},
  {"Fairy Wings", AR_RARE},
  {"Rocket Pack", AR_RARE},
  {"Devil Wings", AR_RARE},
  {"Explorer Bag", AR_SPECIAL},
};
const AccDef &accDef(uint8_t id) { return DEFS[id < ACC_COUNT ? id : 0]; }
uint8_t accSlotOf(uint8_t id) { return (uint8_t)(id / 8); }
uint8_t accCountByRarity(uint8_t r) { uint8_t n = 0; for (auto &d : DEFS) n += d.rarity == r; return n; }
void accInit(AccState &s) { memset(&s, 0, sizeof(s)); for (auto &e : s.equipped) e = ACC_NONE; }
bool accOwned(const AccState &s, uint8_t id) { return id < ACC_COUNT && ((s.owned >> id) & 1); }
bool accGive(AccState &s, uint8_t id) {
  if (id >= ACC_COUNT || accOwned(s, id)) return false;
  s.owned |= 1u << id; s.isNew |= 1u << id; s.counter++; return true;
}
bool accEquipped(const AccState &s, uint8_t id) { return id < ACC_COUNT && s.equipped[id / 8] == id; }
bool accToggleEquip(AccState &s, uint8_t id) {
  if (!accOwned(s, id)) return false;
  uint8_t &slot = s.equipped[id / 8];
  slot = (slot == id) ? ACC_NONE : id;
  s.isNew &= ~(1u << id);
  return true;
}
void accClearNew(AccState &s) { s.isNew = 0; }
uint8_t accPickDrop(const AccState &s, uint8_t maxR, uint32_t roll) {
  uint8_t cand[ACC_COUNT], n = 0;
  for (uint8_t i = 0; i < ACC_COUNT; i++)
    if (!accOwned(s, i) && DEFS[i].rarity <= maxR && DEFS[i].rarity != AR_SPECIAL) cand[n++] = i;
  return n ? cand[roll % n] : ACC_NONE;
}
bool expeditionDropsAccessory(uint8_t roll) { return roll < 15; }
bool gymDropsAccessory(uint8_t roll) { return roll < 25; }
