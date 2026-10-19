#pragma once
// Regional forms (feature list section 9). National Dex numbers.
#include <stdint.h>

enum RegionalForm : uint8_t { RF_ALOLAN = 0, RF_GALARIAN, RF_HISUIAN, RF_PALDEAN };
constexpr uint8_t REGIONAL_BITMAP_BYTES = 128;

struct RegionalDef {
  uint16_t dex; uint8_t form;
  uint8_t type1, type2;
  uint8_t variant;        // breed index (Paldean Tauros 0..2), else 0
  bool fromEvolution;     // only exists as an evolution of a regional form
};
struct RegionalEvo { uint16_t fromDex; uint8_t fromForm; uint16_t toDex; uint8_t toForm; };

uint8_t regionalCount();
const RegionalDef &regionalAt(uint8_t idx);
int16_t regionalIndex(uint16_t dex, uint8_t form, uint8_t variant);   // -1 if none
uint8_t regionalFormsFor(uint16_t dex, uint8_t outIdx[4]);            // hatchable variants only
uint8_t regionalHatchChancePct(uint8_t bond);                         // 20% + bond, max 50
// Decide at hatch: returns table index of the regional form or -1 for the normal form.
int16_t rollRegionalHatch(uint16_t dex, uint8_t bond, uint8_t chanceRoll, uint8_t pickRoll);
bool regionalEvolution(uint16_t dex, uint8_t form, RegionalEvo *out);
// 128-byte seen bitmap indexed by table index
bool regionalSeen(const uint8_t *bitmap, uint8_t idx);
void regionalMarkSeen(uint8_t *bitmap, uint8_t idx);
uint8_t regionalSeenCount(const uint8_t *bitmap);
