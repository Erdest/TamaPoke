#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../expedition.h"
#include "../tradeevo.h"
#include "../regional.h"
#include "../gimmicks.h"
#include "../seasonal.h"
#include "../pokeballs.h"
#include "../garden.h"
#include "../dex.h"

int main() {
  // ---- expeditions ----
  Expedition e = {}; uint8_t en;
  assert(expeditionMinutes(EXP_15) == 15 && expeditionMinutes(EXP_60) == 60 && expeditionEnergyCost(EXP_30) == 20);
  assert(!expeditionStart(e, EXP_60, 39, en, 1000) && en == 39 && !e.active);        // energy too low
  assert(expeditionStart(e, EXP_60, 40, en, 1000) && en == 0 && e.active);
  assert(!expeditionStart(e, EXP_15, 100, en, 1000));                                 // already running
  assert(!expeditionReady(e, 1000 + 3599) && expeditionReady(e, 1000 + 3600));
  assert(expeditionSecondsLeft(e, 1000 + 60) == 3540 && expeditionSecondsLeft(e, 99999) == 0);
  ExpBag bag = {}; Loot l;
  assert(!expeditionCollect(e, bag, 1000 + 10, 50, 0, 99, &l));                       // not ready
  assert(expeditionCollect(e, bag, 1000 + 3600, 50, 0, 99, &l) && !e.active && bag.count == 1 && l.kind == LOOT_SNACK && !l.accessory);
  for (int i = 0; i < 2; i++) { expeditionStart(e, EXP_15, 50, en, 0); assert(expeditionCollect(e, bag, 900, 50, 1, 0, &l) && l.accessory); }
  assert(bag.count == 3);
  expeditionStart(e, EXP_15, 50, en, 0);
  assert(!expeditionCollect(e, bag, 900, 50, 0, 99, &l) && e.active);                // bag full keeps expedition
  assert(bagRemove(bag, 0) && bag.count == 2 && expeditionCollect(e, bag, 900, 50, 0, 99, &l));
  assert(rollLoot(0, 0, 99).kind == LOOT_BALL && rollLoot(0, 0, 99).param == BALL_GREAT);
  assert(rollLoot(19, 99, 99).param == BALL_QUICK && rollLoot(19, 40, 99).param == BALL_NET);
  assert(rollLoot(20, 0, 99).kind == LOOT_TRADE_ITEM && rollLoot(20, 0, 99).param == TI_METAL_COAT);
  assert(rollLoot(25, 14, 99).kind == LOOT_BERRY_SEED && rollLoot(25, 14, 99).param == 1);
  assert(rollLoot(60, 3, 99).kind == LOOT_TRAINING_WEIGHT && rollLoot(60, 7, 99).param == 1);
  for (int c = 0; c < 100; c++) for (int s = 0; s < 100; s++) {
    Loot x = rollLoot((uint8_t)c, (uint8_t)s, 50);
    assert(x.kind != LOOT_NONE);
    if (x.kind == LOOT_TRADE_ITEM) assert(x.param >= 1 && x.param < TI_COUNT);
    if (x.kind == LOOT_BERRY_SEED) assert(x.param < BERRY_COUNT);
  }

  // ---- trade evolutions ----
  assert(tradeEvolution(64, TI_NONE, 0) == 65 && tradeEvolution(93, TI_KINGS_ROCK, 0) == 94);
  assert(tradeEvolution(95, TI_NONE, 0) == 0 && tradeEvolution(95, TI_METAL_COAT, 0) == 208);
  assert(tradeEvolution(95, TI_DRAGON_SCALE, 0) == 0);
  assert(tradeEvolution(233, TI_DUBIOUS_DISC, 0) == 474 && tradeEvolution(137, TI_UP_GRADE, 0) == 233);
  assert(tradeEvolution(366, TI_DEEP_SEA_TOOTH, 0) == 367 && tradeEvolution(366, TI_DEEP_SEA_SCALE, 0) == 368);
  assert(tradeEvolution(616, TI_NONE, 588) == 617 && tradeEvolution(588, TI_NONE, 616) == 589);
  assert(tradeEvolution(616, TI_NONE, 0) == 0 && tradeEvolution(616, TI_NONE, 1) == 0);
  assert(isTradeEvolver(710) && !isTradeEvolver(25));
  for (uint8_t i = 0; i < tradeEvoCount(); i++) assert(tradeEvoAt(i).from != tradeEvoAt(i).to);

  // ---- regional forms ----
  assert(regionalCount() > 50);
  int16_t i = regionalIndex(37, RF_ALOLAN, 0);
  assert(i >= 0 && regionalAt((uint8_t)i).type1 == TYPE_ICE);                         // Alolan Vulpix: Ice
  assert(regionalAt((uint8_t)regionalIndex(105, RF_ALOLAN, 0)).type1 == TYPE_FIRE &&
         regionalAt((uint8_t)regionalIndex(105, RF_ALOLAN, 0)).type2 == TYPE_GHOST);  // Marowak: Fire/Ghost
  uint8_t idx[4];
  assert(regionalFormsFor(52, idx) == 2);                                              // Alolan + Galarian Meowth
  assert(regionalFormsFor(128, idx) == 3);                                             // 3 Tauros breeds
  assert(regionalFormsFor(863, idx) == 0);                                             // Perrserker: evolution only
  assert(regionalHatchChancePct(0) == 20 && regionalHatchChancePct(30) == 50 && regionalHatchChancePct(100) == 50);
  assert(rollRegionalHatch(37, 0, 19, 0) >= 0 && rollRegionalHatch(37, 0, 20, 0) == -1);
  assert(rollRegionalHatch(25, 30, 0, 0) == -1);                                       // Pikachu has none
  assert(rollRegionalHatch(52, 0, 0, 0) != rollRegionalHatch(52, 0, 0, 1));
  RegionalEvo re;
  assert(regionalEvolution(52, RF_GALARIAN, &re) && re.toDex == 863);
  assert(!regionalEvolution(52, RF_ALOLAN, &re));
  assert(regionalEvolution(194, RF_PALDEAN, &re) && re.toDex == 980);
  assert(regionalEvolution(211, RF_HISUIAN, &re) && re.toDex == 904);
  uint8_t bm[REGIONAL_BITMAP_BYTES] = {0};
  assert(!regionalSeen(bm, 5)); regionalMarkSeen(bm, 5); regionalMarkSeen(bm, 5);
  assert(regionalSeen(bm, 5) && regionalSeenCount(bm) == 1);

  // ---- gimmicks ----
  assert(megaFormCount() == 48 && gmaxSpeciesCount() == 32);
  assert(hasMega(6) && hasMegaXY(6) && hasMegaXY(150) && !hasMegaXY(9) && !hasMega(25) && hasGigantamax(25));
  GimmickState g = {}; g.teraType = TYPE_FIRE;
  assert(availableGimmicks(6, 19, g) == 0);                                            // Charizard bond 19
  assert(availableGimmicks(6, 20, g) == GMF_MEGA);
  assert(availableGimmicks(6, 30, g) == (GMF_MEGA | GMF_TERA));                        // player chooses
  assert(availableGimmicks(25, 25, g) == GMF_DYNAMAX && availableGimmicks(25, 30, g) == (GMF_DYNAMAX | GMF_TERA));
  assert(!activateGimmick(g, 6, 20, GM_TERA) && activateGimmick(g, 6, 30, GM_TERA));
  assert(availableGimmicks(6, 100, g) == 0 && !activateGimmick(g, 6, 100, GM_MEGA));   // one per battle
  gimmickBattleEnd(g); assert(g.teraType == TYPE_FIRE && availableGimmicks(6, 20, g) == GMF_MEGA);
  assert(activateGimmick(g, 25, 25, GM_DYNAMAX) && g.turnsLeft == 3);
  assert(!gimmickEndTurn(g) && !gimmickEndTurn(g) && gimmickEndTurn(g) && g.active == GM_NONE);   // 3 turns
  assert(megaStatPct(GM_MEGA, false, true) == 130 && megaStatPct(GM_MEGA, true, false) == 120 && megaStatPct(GM_DYNAMAX, true, true) == 100);
  assert(dynamaxMaxHp(80) == 160 && dynamaxEndHp(160, 160, 80) == 80 && dynamaxEndHp(40, 160, 80) == 20 && dynamaxEndHp(1, 160, 80) == 1);
  for (int r = 0; r < 18; r++) assert(rollTeraType((uint8_t)r) == r + 1);

  // ---- seasonal ----
  EventEffects ev = eventsFor(10, 31, 0, 0);
  assert((ev.flags & EV_HALLOWEEN) && ev.ghostBoost && featuredSpecies(710, ev) && !featuredSpecies(225, ev));
  ev = eventsFor(12, 5, 0, 0); assert(ev.iceBoost && ev.delibirdGift && featuredSpecies(225, ev));
  ev = eventsFor(2, 27, 0, 0); assert(ev.shinyDoubled && ev.guaranteedRareEncounter && shinyDenominatorWithEvents(48, ev) == 24);
  assert(!eventsFor(2, 26, 0, 0).shinyDoubled && shinyDenominatorWithEvents(48, eventsFor(5, 5, 0, 0)) == 48);
  ev = eventsFor(7, 4, 7, 4); assert((ev.flags & EV_BIRTHDAY) && ev.birthdayEgg && ev.birthdayJoy == 25);
  assert(!eventsFor(7, 4, 0, 0).birthdayEgg && shinyDenominatorWithEvents(1, eventsFor(2, 27, 0, 0)) == 1);
  puts("misc tests passed"); return 0;
}
