#pragma once
// Cosmetic accessories: 32 items across Head/Face/Neck/Back (feature list section 32).
#include <stdint.h>

enum AccSlot : uint8_t { SLOT_HEAD = 0, SLOT_FACE, SLOT_NECK, SLOT_BACK };
enum AccRarity : uint8_t { AR_COMMON = 0, AR_UNCOMMON, AR_RARE, AR_SPECIAL };
constexpr uint8_t ACC_COUNT = 32;
struct AccDef { const char *name; uint8_t rarity; };
struct AccState { uint32_t owned, isNew; uint8_t equipped[4]; uint16_t counter; };   // 14 bytes
constexpr uint8_t ACC_NONE = 255;

const AccDef &accDef(uint8_t id);                 // id = slot * 8 + index
uint8_t accSlotOf(uint8_t id);
uint8_t accCountByRarity(uint8_t rarity);
void accInit(AccState &s);
bool accGive(AccState &s, uint8_t id);            // false if already owned
bool accOwned(const AccState &s, uint8_t id);
bool accToggleEquip(AccState &s, uint8_t id);     // one per slot; equipping replaces
bool accEquipped(const AccState &s, uint8_t id);
void accClearNew(AccState &s);
// Pick an unowned drop with rarity <= maxRarity (never Special). roll: any value.
uint8_t accPickDrop(const AccState &s, uint8_t maxRarity, uint32_t roll);
// Drop sources
bool expeditionDropsAccessory(uint8_t roll);      // 15%
bool gymDropsAccessory(uint8_t roll);             // 25%
