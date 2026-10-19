#include "gimmicks.h"

static const uint16_t MEGA[] = {
  3, 6, 9, 15, 18, 65, 80, 94, 115, 127, 130, 142, 150, 181, 208, 212, 214, 229, 248, 254, 257, 260,
  282, 302, 303, 306, 308, 310, 319, 323, 334, 354, 359, 362, 373, 376, 380, 381, 384, 428, 445, 448,
  460, 475, 531, 719,
};  // 46 species; Charizard and Mewtwo have two forms -> 48 Mega forms.
// (The spec's list has 44 names / 46 forms; Sceptile and Swampert added to reach its stated 48.)
static const uint16_t GMAX[] = {
  3, 6, 9, 12, 25, 52, 68, 94, 99, 131, 133, 143, 569, 809, 812, 815, 818, 823, 826, 834, 839, 841,
  842, 844, 849, 851, 858, 861, 869, 879, 884, 892,
};  // 32 species

static bool inList(const uint16_t *l, unsigned n, uint16_t d) { for (unsigned i = 0; i < n; i++) if (l[i] == d) return true; return false; }
bool hasMega(uint16_t d) { return inList(MEGA, sizeof(MEGA) / sizeof(MEGA[0]), d); }
bool hasMegaXY(uint16_t d) { return d == 6 || d == 150; }
bool hasGigantamax(uint16_t d) { return inList(GMAX, sizeof(GMAX) / sizeof(GMAX[0]), d); }
uint8_t megaFormCount() { return (uint8_t)(sizeof(MEGA) / sizeof(MEGA[0]) + 2); }
uint8_t gmaxSpeciesCount() { return (uint8_t)(sizeof(GMAX) / sizeof(GMAX[0])); }

uint8_t availableGimmicks(uint16_t dex, uint8_t bond, const GimmickState &s) {
  if (s.used) return 0;                                  // one gimmick per battle
  uint8_t m = 0;
  bool mega = hasMega(dex);
  if (mega && bond >= MEGA_BOND) m |= GMF_MEGA;
  if (!mega && bond >= DYNAMAX_BOND) m |= GMF_DYNAMAX;   // "any non-Mega species"
  if (bond >= TERA_BOND && s.teraType >= 1 && s.teraType <= 18) m |= GMF_TERA;
  return m;
}
bool activateGimmick(GimmickState &s, uint16_t dex, uint8_t bond, Gimmick g) {
  if (g == GM_NONE || !(availableGimmicks(dex, bond, s) & (1u << g))) return false;
  s.used = 1; s.active = g;
  s.turnsLeft = (g == GM_DYNAMAX) ? DYNAMAX_TURNS : 0;
  return true;
}
bool gimmickEndTurn(GimmickState &s) {
  if (s.active != GM_DYNAMAX) return false;
  if (s.turnsLeft > 0) s.turnsLeft--;
  if (s.turnsLeft == 0) { s.active = GM_NONE; return true; }
  return false;
}
void gimmickBattleEnd(GimmickState &s) { s.used = 0; s.active = 0; s.turnsLeft = 0; }
uint16_t megaStatPct(Gimmick a, bool isSpeed, bool isAtkDef) {
  if (a != GM_MEGA) return 100;
  return isAtkDef ? 130 : isSpeed ? 120 : 100;
}
uint16_t dynamaxMaxHp(uint16_t b) { return (uint16_t)(b * 2u > 65535u ? 65535u : b * 2u); }
uint16_t dynamaxEndHp(uint16_t cur, uint16_t dyn, uint16_t base) {
  if (!dyn) return 1;
  uint32_t v = (uint32_t)cur * base / dyn;
  if (v) return (uint16_t)v;
  return cur ? 1 : 0;   // never round a living Pokemon down to 0 HP
}
uint8_t rollTeraType(uint8_t r) { return (uint8_t)(1 + r % 18); }
