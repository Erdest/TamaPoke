#pragma once
// Pokedex tracking for 1,025 species (feature list sections 1 and 22).
#include <stdint.h>

constexpr uint16_t DEX_TOTAL = 1025;
constexpr uint16_t DEX_BYTES = (DEX_TOTAL + 7) / 8;   // 129

enum DexFilter : uint8_t { DF_ALL = 0, DF_RAISED, DF_CAUGHT };
struct DexBits { uint8_t raised[DEX_BYTES], caught[DEX_BYTES], shiny[DEX_BYTES]; };

void dexInit(DexBits &d);
bool dexMarkRaised(DexBits &d, uint16_t dex, bool shiny);
bool dexMarkCaught(DexBits &d, uint16_t dex, bool shiny);
bool dexIsRaised(const DexBits &d, uint16_t dex);
bool dexIsCaught(const DexBits &d, uint16_t dex);
bool dexIsShiny(const DexBits &d, uint16_t dex);
bool dexRegistered(const DexBits &d, uint16_t dex);        // raised or caught
uint16_t dexCountRegistered(const DexBits &d);
uint16_t dexCountRaised(const DexBits &d);
uint16_t dexCountCaught(const DexBits &d);
uint16_t dexCountShiny(const DexBits &d);
bool dexMatchesFilter(const DexBits &d, uint16_t dex, DexFilter f);
uint16_t dexFilteredCount(const DexBits &d, DexFilter f);
int16_t dexNextFiltered(const DexBits &d, int16_t from, int8_t dir, DexFilter f);   // gallery swipe; -1 if none
// Collection frames
constexpr uint8_t FRAME_COUNT = 8;
uint16_t frameThreshold(uint8_t frame);                     // 10,25,50,100,200,400,700,1025
uint8_t framesUnlocked(uint16_t registered);
bool canEquipFrame(uint16_t registered, uint8_t frame);
