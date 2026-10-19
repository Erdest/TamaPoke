#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../gyms.h"
#include "../dex.h"

int main() {
  // Table integrity
  int leaders = 0, e4 = 0, champs = 0;
  for (uint8_t r = 0; r < REGION_COUNT; r++) {
    assert(regionName(r)[0]);
    for (uint8_t g = 0; g < 8; g++) { assert(gymLeader(r, g).name && gymLeader(r, g).type >= 1 && gymLeader(r, g).type <= 18); leaders++; }
    for (uint8_t i = 0; i < 4; i++) { assert(eliteFour(r, i).name); e4++; }
    assert(champion(r).name); champs++;
  }
  assert(leaders == 72 && e4 == 36 && champs == 9);
  assert(!strcmp(gymLeader(0, 0).name, "Brock") && gymLeader(0, 0).type == TYPE_ROCK);
  assert(!strcmp(gymLeader(8, 7).name, "Grusha") && gymLeader(8, 7).type == TYPE_ICE);
  assert(!strcmp(gymLeader(6, 6).name, "Nanu") && gymLeader(6, 6).type == TYPE_DARK);
  assert(!strcmp(champion(0).name, "Lance") && !strcmp(champion(8).name, "Geeta"));

  // Levels and sizes
  assert(gymMinPlayerLevel(0) == 10 && gymMinPlayerLevel(7) == 45);
  assert(gymTeamSize(0) == 3 && gymTeamSize(7) == 6);
  assert(scaledLevel(20, 10, RM_NORMAL) == 20);            // under-leveled player: base level
  assert(scaledLevel(20, 40, RM_NORMAL) == 30);            // (20+40)/2
  assert(scaledLevel(20, 40, RM_HARD) == 40 && scaledLevel(20, 40, RM_ELITE) == 50);
  assert(scaledLevel(90, 100, RM_ELITE) == 100);
  assert(e4HealAmount(100) == 20);

  // Progression
  GymProgress p; progressInit(p);
  assert(canChallengeGym(p, 0, 0, 9) == CH_LEVEL_TOO_LOW);
  assert(canChallengeGym(p, 0, 1, 50) == CH_PREV_GYM_MISSING);
  assert(canChallengeGym(p, 0, 0, 10) == CH_OK && canChallengeGym(p, 9, 0, 10) == CH_BAD_INDEX);
  Rewards w = awardGymWin(p, 0, 0);
  assert(w.joy == 15 && w.bond == 10 && w.ultraBalls == 2 && w.accessoryChancePct == 25 && !w.luxuryBalls);
  assert(canChallengeGym(p, 0, 0, 99) == CH_ALREADY_BEATEN && awardGymWin(p, 0, 0).joy == 0);   // no double reward
  assert(canChallengeGym(p, 0, 1, 15) == CH_OK);
  assert(canChallengeE4(p, 0) == CH_NEED_ALL_BADGES && canChallengeChampion(p, 0) == CH_NEED_ALL_BADGES);
  for (uint8_t g = 1; g < 8; g++) {
    w = awardGymWin(p, 0, g);
    assert((g == 3 || g == 7) == (w.luxuryBalls == 1));      // every 4th badge
    assert((g == 7) == w.legendaryEgg);                      // full 8 badges
  }
  assert(badgesIn(p, 0) == 8 && totalBadges(p) == 8 && badgesIn(p, 1) == 0);
  assert(canChallengeE4(p, 0) == CH_OK && nextE4(p, 0) == 0 && canChallengeChampion(p, 0) == CH_NEED_E4);
  w = awardE4Win(p, 0, 0); assert(w.joy == 10 && w.bond == 5 && nextE4(p, 0) == 1);
  assert(awardE4Win(p, 0, 0).joy == 0);                       // beaten E4 stays beaten on re-entry
  for (uint8_t i = 1; i < 4; i++) awardE4Win(p, 0, i);
  assert(nextE4(p, 0) == -1 && canChallengeE4(p, 0) == CH_ALREADY_BEATEN && canChallengeChampion(p, 0) == CH_OK);
  assert(!setRematchTier(p, 0, RM_HARD));                     // champion not beaten -> no rematch
  w = awardChampionWin(p, 0, true);
  assert(w.joy == 25 && w.bond == 15 && w.accessoryGuaranteedRare && w.legendaryEgg && w.masterBalls == 1);
  assert(!w.grandChampionAchievement && canChallengeChampion(p, 0) == CH_ALREADY_BEATEN);
  assert(awardChampionWin(p, 0, true).joy == 0);
  assert(regionComplete(p, 0) && setRematchTier(p, 0, RM_ELITE) && rematchTier(p, 0) == RM_ELITE);
  assert(!setRematchTier(p, 1, RM_HARD));
  // Grand champion after all 9
  for (uint8_t r = 1; r < 9; r++) {
    for (uint8_t g = 0; g < 8; g++) awardGymWin(p, r, g);
    for (uint8_t i = 0; i < 4; i++) awardE4Win(p, r, i);
    w = awardChampionWin(p, r, false);
    assert(w.masterBalls == 0 && (r == 8) == w.grandChampionAchievement && (r == 8) == w.shinyLegendaryEgg);
  }
  assert(totalBadges(p) == 72 && championsBeaten(p) == 9 && grandChampion(p));
  assert(sizeof(GymProgress) <= 28);   // 27 data bytes (+1 alignment padding)

  // Team building
  uint16_t pool[40]; for (int i = 0; i < 40; i++) pool[i] = 100 + i;
  TeamMon t[TEAM_MAX];
  assert(buildTeam(TK_GYM, 0, 0, 10, RM_NORMAL, pool, 40, 123, t) == 3);
  assert(buildTeam(TK_GYM, 0, 7, 40, RM_NORMAL, pool, 40, 123, t) == 6);
  for (int a = 0; a < 6; a++) for (int b = a + 1; b < 6; b++) assert(t[a].dex != t[b].dex);
  assert(t[5].level >= t[0].level && t[5].level == scaledLevel(gymBaseLevel(7), 40, RM_NORMAL) + 2);
  TeamMon u[TEAM_MAX]; buildTeam(TK_GYM, 0, 7, 40, RM_NORMAL, pool, 40, 123, u);
  for (int i = 0; i < 6; i++) assert(t[i].dex == u[i].dex && t[i].level == u[i].level);   // deterministic
  assert(buildTeam(TK_E4, 2, 1, 30, RM_HARD, pool, 40, 5, t) == 5 && t[0].level >= 60);
  assert(buildTeam(TK_CHAMPION, 3, 0, 70, RM_ELITE, pool, 40, 5, t) == 6 && t[5].level == 87);   // (60+70)/2 + 20 + 2 ace
  uint16_t tiny[2] = {7, 9};
  assert(buildTeam(TK_CHAMPION, 0, 0, 50, RM_NORMAL, tiny, 2, 1, t) == 6);   // small pool repeats
  assert(buildTeam(TK_GYM, 0, 0, 10, RM_NORMAL, nullptr, 0, 1, t) == 0);
  puts("gym tests passed"); return 0;
}
