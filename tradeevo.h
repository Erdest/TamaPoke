#pragma once
// Trade evolutions (feature list sections 4 and 8). National Dex numbers.
#include <stdint.h>
#include "expedition.h"   // TradeItem

struct TradeEvo { uint16_t from, to; uint8_t item; uint16_t partner; };  // partner: required swap partner (0 = none)
uint8_t tradeEvoCount();
const TradeEvo &tradeEvoAt(uint8_t i);
// Returns the evolved dex number, or 0 if no trade evolution applies.
// partnerDex is the species received in a swap (for Shelmet <-> Karrablast).
uint16_t tradeEvolution(uint16_t dex, uint8_t heldItem, uint16_t partnerDex);
bool isTradeEvolver(uint16_t dex);
