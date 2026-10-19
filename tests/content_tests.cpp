#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../achievements.h"
#include "../accessories.h"
#include "../quiz.h"
#include "../dex.h"
#include "../battle.h"

int main() {
  // ---- achievements ----
  assert(ACH_COUNT == 51 && achCountInCategory(AC_COLLECTION) == 12 && achCountInCategory(AC_GROWTH) == 14 &&
         achCountInCategory(AC_COMBAT) == 11 && achCountInCategory(AC_ACTIVITY) == 14);
  assert(sizeof(((AchState *)0)->progress) == 102 && sizeof(((AchState *)0)->unlocked) == 7);   // 109 NVS bytes
  for (uint8_t i = 0; i < ACH_COUNT; i++) assert(achDef(i).name && achDef(i).goal > 0 && achStat(i) < AS_COUNT);
  AchState a; achInit(a);
  assert(achReport(a, AS_HATCHED, 9) == 0 && achReport(a, AS_HATCHED, 10) == 1 && achUnlockedCount(a) == 1);
  assert(achReport(a, AS_HATCHED, 10) == 0);                          // no duplicate unlock
  assert(achReport(a, AS_HATCHED, 100) == 2 && achHasReward(a, AR_EGG_RARITY_PLUS1));   // Hatch 50 + Hatch 100
  uint8_t id; assert(achPopBanner(a, &id) && !strcmp(achDef(id).name, "Hatch 10"));
  assert(achPopBanner(a, &id) && achPopBanner(a, &id) && !achPopBanner(a, &id));
  assert(achReport(a, AS_STREAK, 100) == 3 && achHasReward(a, AR_SHINY_MINUS4));          // 7, 30, 100-day
  assert(achReport(a, AS_STREAK, 5) == 0);                                                // progress keeps max
  // banner queue holds 4; extras still unlock
  AchState q; achInit(q); achReport(q, AS_HATCHED, 200); achReport(q, AS_WINS, 100); achReport(q, AS_QUIZ_CORRECT, 500);
  assert(q.queueLen == ACH_BANNER_QUEUE && achUnlockedCount(q) > ACH_BANNER_QUEUE && achHasReward(q, AR_MONOCLE));
  achReport(q, AS_CHAMPIONS, 9); assert(achHasReward(q, AR_GRAND_CHAMPION) && !achHasReward(q, AR_CROWN_TITLE));
  achReport(q, AS_REGISTERED, 1025); assert(achHasReward(q, AR_CROWN_TITLE));
  achReport(q, AS_STEPS_K, 250); assert(achHasReward(q, AR_EXPLORER_BAG));
  assert(achAddTo(q, AS_PHOTOS, 20, 0) == 1);

  // ---- accessories ----
  assert(ACC_COUNT == 32 && accCountByRarity(AR_COMMON) == 14 && accCountByRarity(AR_UNCOMMON) == 10 &&
         accCountByRarity(AR_RARE) == 6 && accCountByRarity(AR_SPECIAL) == 2);
  assert(sizeof(AccState) <= 16);
  AccState s; accInit(s);
  assert(accGive(s, 0) && !accGive(s, 0) && accOwned(s, 0) && s.counter == 1 && (s.isNew & 1));
  assert(!accToggleEquip(s, 1));                                    // not owned
  assert(accToggleEquip(s, 0) && accEquipped(s, 0) && !(s.isNew & 1));
  accGive(s, 1); assert(accToggleEquip(s, 1) && accEquipped(s, 1) && !accEquipped(s, 0));   // same slot replaces
  assert(accToggleEquip(s, 1) && !accEquipped(s, 1));
  accGive(s, 8); accToggleEquip(s, 8); accToggleEquip(s, 0);
  assert(accEquipped(s, 8) && accEquipped(s, 0) && accSlotOf(8) == SLOT_FACE);              // one per slot, 4 max
  AccState d; accInit(d);
  for (uint32_t r = 0; r < 200; r++) { uint8_t p = accPickDrop(d, AR_COMMON, r); assert(p != ACC_NONE && accDef(p).rarity == AR_COMMON); }
  for (uint32_t r = 0; r < 200; r++) { uint8_t p = accPickDrop(d, AR_RARE, r); assert(accDef(p).rarity <= AR_RARE); }
  for (int i = 0; i < 32; i++) { uint8_t p = accPickDrop(d, AR_RARE, (uint32_t)i); if (p == ACC_NONE) break; accGive(d, p); }
  assert(accPickDrop(d, AR_RARE, 0) == ACC_NONE && !accOwned(d, 7) && !accOwned(d, 31));    // specials are achievement-only
  assert(expeditionDropsAccessory(14) && !expeditionDropsAccessory(15) && gymDropsAccessory(24) && !gymDropsAccessory(25));

  // ---- quiz ----
  assert(quizChoices(QD_EASY) == 4 && quizChoices(QD_EXPERT) == 6 && quizTimerMs(QD_MEDIUM) == 6000 && quizTimerMs(QD_EXPERT) == 3000);
  assert(quizSeenOnly(QD_EASY) && !quizSeenOnly(QD_MEDIUM) && quizShinySilhouettes(QD_EXPERT));
  uint16_t pool[20]; for (int i = 0; i < 20; i++) pool[i] = 1 + i;
  QuizQuestion qq;
  for (uint32_t seed = 0; seed < 300; seed++) {
    assert(quizBuildSpecies(7, 6, pool, 20, seed, qq) && qq.n == 6 && qq.choices[qq.correctIdx] == 7);
    for (int x = 0; x < 6; x++) for (int y = x + 1; y < 6; y++) assert(qq.choices[x] != qq.choices[y]);
  }
  assert(!quizBuildSpecies(7, 6, pool, 3, 1, qq) && !quizBuildSpecies(7, 4, nullptr, 0, 1, qq));
  for (uint32_t seed = 0; seed < 300; seed++) {
    assert(quizBuildType(TYPE_GRASS, TYPE_FLYING, 4, seed, qq) && qq.n == 4);
    assert(battleTypeEffectPct(qq.choices[qq.correctIdx], TYPE_GRASS, TYPE_FLYING) > 100);
    for (int x = 0; x < 4; x++) if (x != qq.correctIdx) assert(battleTypeEffectPct(qq.choices[x], TYPE_GRASS, TYPE_FLYING) <= 100);
  }
  assert(quizPoints(8000, 8000, 0) == 20 && quizPoints(0, 8000, 0) == 10 && quizPoints(4000, 8000, 5) == 20 && quizPoints(8000, 8000, 5) == 25);
  assert(quizStars(10) == 3 && quizStars(7) == 2 && quizStars(4) == 1 && quizStars(3) == 0);
  assert(quizSessionReward(10, 49).seedDrop && !quizSessionReward(10, 50).seedDrop && !quizSessionReward(9, 0).seedDrop);
  assert(quizSessionReward(10, 0).joy == 15 && quizSessionReward(7, 0).bond == 2);
  QuizStats qs = {};
  assert(quizRecordSession(qs, QM_CRY, 120, 10) && !quizRecordSession(qs, QM_CRY, 120, 5) && quizRecordSession(qs, QM_CRY, 121, 5));
  assert(qs.lifetimeCorrect == 20 && qs.perfectRounds == 1 && qs.high[QM_SILHOUETTE] == 0 && !quizRecordSession(qs, 9, 1, 1));
  puts("content tests passed"); return 0;
}
