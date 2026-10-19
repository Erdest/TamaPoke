#include <assert.h>
#include <stdio.h>
#include "../dexbits.h"
#include "../egg.h"
#include "../steps.h"

int main() {
  // ---- dex bitmaps ----
  DexBits d; dexInit(d);
  assert(DEX_BYTES == 129 && dexCountRegistered(d) == 0);
  assert(dexMarkRaised(d, 1, false) && dexMarkRaised(d, 1025, true) && !dexMarkRaised(d, 0, false) && !dexMarkRaised(d, 1026, false));
  assert(dexMarkCaught(d, 25, false) && dexMarkCaught(d, 1, false));
  assert(dexIsRaised(d, 1025) && dexIsShiny(d, 1025) && !dexIsShiny(d, 1) && dexIsCaught(d, 25) && !dexIsRaised(d, 25));
  assert(dexCountRaised(d) == 2 && dexCountCaught(d) == 2 && dexCountRegistered(d) == 3 && dexCountShiny(d) == 1);
  assert(dexFilteredCount(d, DF_ALL) == 1025 && dexFilteredCount(d, DF_RAISED) == 2 && dexFilteredCount(d, DF_CAUGHT) == 2);
  assert(dexNextFiltered(d, 0, 1, DF_CAUGHT) == 1 && dexNextFiltered(d, 1, 1, DF_CAUGHT) == 25 && dexNextFiltered(d, 25, 1, DF_CAUGHT) == -1);
  assert(dexNextFiltered(d, 1025, -1, DF_RAISED) == 1 && dexNextFiltered(d, 1, -1, DF_RAISED) == -1);
  assert(framesUnlocked(9) == 0 && framesUnlocked(10) == 1 && framesUnlocked(200) == 5 && framesUnlocked(1025) == 8);
  assert(canEquipFrame(25, 1) && !canEquipFrame(24, 1) && !canEquipFrame(2000, 8));

  // ---- egg rarity ----
  EggBlessings b = {};
  assert(eggLegendaryPct(b) == 3 && eggRarePct(b) == 27);
  assert(rollEggRarity(2, b, 30) == ER_LEGENDARY && rollEggRarity(3, b, 30) == ER_RARE && rollEggRarity(29, b, 30) == ER_RARE && rollEggRarity(30, b, 30) == ER_COMMON);
  assert(rollEggRarity(0, b, 24) == ER_RARE);                    // legendaries locked under 25 registered
  b.farewell = true; assert(eggLegendaryPct(b) == 10 && eggRarePct(b) == 45);
  assert(rollEggRarity(9, b, 25) == ER_LEGENDARY && rollEggRarity(54, b, 25) == ER_RARE && rollEggRarity(55, b, 25) == ER_COMMON);
  b.runaway = true; assert(rollEggRarity(0, b, 500) == ER_COMMON);   // curse
  b = EggBlessings(); b.enigma = true;
  assert(rollEggRarity(90, b, 100) == ER_RARE && rollEggRarity(10, b, 100) == ER_LEGENDARY && rollEggRarity(90, b, 5) == ER_RARE);
  b.hatch100 = true; assert(rollEggRarity(90, b, 100) == ER_LEGENDARY);
  assert(eggCrackStage(1) == 1 && eggCrackStage(9) == 3 && !eggHatches(2, 179) && eggHatches(3, 0) && eggHatches(0, 180));

  // ---- shiny odds ----
  ShinyInputs s = {};
  assert(shinyDenominator(s) == 48);
  s.farewell = true; assert(shinyDenominator(s) == 24);
  s.farewell = false; s.careStreakDays = 255; s.bond = 100; assert(careShinyReduction(255, 100) == 16 && shinyDenominator(s) == 32);
  s.trailPerk = 8; s.achievementPerk = 4; assert(shinyDenominator(s) == 20);
  s.farewell = true; assert(shinyDenominator(s) == 8);            // 24-16-8-4 floors at 8
  s.starf = true; s.farewell = false; assert(shinyDenominator(s) == 8);
  s = ShinyInputs(); s.pokemonDay = true; assert(shinyDenominator(s) == 24);
  assert(rollShiny(48, 96) && !rollShiny(48, 97) && !rollShiny(0, 0));
  for (int r = 0; r < 255; r++) assert(geneFromRoll((uint8_t)r) >= 90 && geneFromRoll((uint8_t)r) <= 110);
  assert(personalityOf(25, false, 0) == 0 && personalityOf(25, true, 3) == 4 && personalityOf(1, false, 4) == 0);

  // ---- steps and trail ----
  assert(milestoneReached(999) == SM_NONE && milestoneReached(1000) == SM_BRONZE && milestoneReached(4999) == SM_SILVER && milestoneReached(5000) == SM_GOLD);
  assert(milestoneReward(SM_BRONZE).snack && milestoneReward(SM_SILVER).energyDrink);
  assert(milestoneReward(SM_GOLD).trainingWeight && milestoneReward(SM_GOLD).premierBalls == 1 && !milestoneReward(SM_NONE).snack);
  assert(trailRank(4999) == TR_HATCHLING && trailRank(5000) == TR_WALKER && trailRank(25000) == TR_JOGGER && trailRank(100000) == TR_HIKER && trailRank(250000) == TR_EXPLORER);
  assert(trailShinyReduction(TR_WALKER) == 0 && trailShinyReduction(TR_JOGGER) == 4 && trailShinyReduction(TR_EXPLORER) == 8);
  assert(trailCatchBonusPct(TR_HATCHLING) == 0 && trailCatchBonusPct(TR_HIKER) == 5 && trailGivesWalkJoy(TR_WALKER) && !trailGivesWalkJoy(TR_HATCHLING) && trailExplorerBag(TR_EXPLORER));

  // ---- wonder trade + PvP ----
  WonderState w = {};
  for (int i = 0; i < 3; i++) assert(wonderTradeConsume(w, 100));
  assert(!wonderTradeConsume(w, 100) && !wonderTradeAllowed(w, 100));
  assert(wonderTradeAllowed(w, 101) && w.used == 0 && wonderTradeConsume(w, 101));
  assert(pvpReward(true).joy == 15 && pvpReward(true).bond == 10 && pvpReward(false).joy == 5 && pvpReward(false).bond == 0);
  puts("progress tests passed"); return 0;
}
