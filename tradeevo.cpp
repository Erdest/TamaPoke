#include "tradeevo.h"
static const TradeEvo T[] = {
  // no item
  {64, 65, TI_NONE, 0}, {67, 68, TI_NONE, 0}, {75, 76, TI_NONE, 0}, {93, 94, TI_NONE, 0},
  {525, 526, TI_NONE, 0}, {533, 534, TI_NONE, 0}, {708, 709, TI_NONE, 0}, {710, 711, TI_NONE, 0},
  // with item
  {95, 208, TI_METAL_COAT, 0}, {123, 212, TI_METAL_COAT, 0}, {117, 230, TI_DRAGON_SCALE, 0},
  {137, 233, TI_UP_GRADE, 0}, {233, 474, TI_DUBIOUS_DISC, 0}, {112, 464, TI_PROTECTOR, 0},
  {125, 466, TI_ELECTIRIZER, 0}, {126, 467, TI_MAGMARIZER, 0}, {356, 477, TI_REAPER_CLOTH, 0},
  {682, 683, TI_SACHET, 0}, {684, 685, TI_WHIPPED_DREAM, 0}, {349, 350, TI_PRISM_SCALE, 0},
  {366, 367, TI_DEEP_SEA_TOOTH, 0}, {366, 368, TI_DEEP_SEA_SCALE, 0},
  {61, 186, TI_KINGS_ROCK, 0}, {79, 199, TI_KINGS_ROCK, 0},
  // simultaneous swap pair
  {616, 617, TI_NONE, 588}, {588, 589, TI_NONE, 616},
};
uint8_t tradeEvoCount() { return (uint8_t)(sizeof(T) / sizeof(T[0])); }
const TradeEvo &tradeEvoAt(uint8_t i) { return T[i < tradeEvoCount() ? i : 0]; }
uint16_t tradeEvolution(uint16_t dex, uint8_t item, uint16_t partner) {
  for (const TradeEvo &e : T) {
    if (e.from != dex) continue;
    if (e.partner) { if (partner == e.partner) return e.to; continue; }
    if (e.item == TI_NONE ? true : e.item == item) return e.to;
  }
  return 0;
}
bool isTradeEvolver(uint16_t dex) { for (const TradeEvo &e : T) if (e.from == dex) return true; return false; }
