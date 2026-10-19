#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <initializer_list>
#include "../moves.h"
#include "../dex.h"

int main() {
  // table integrity: 180 moves, 10 per type, ids line up with types
  for (uint8_t t = 1; t <= 18; t++) {
    int strong = 0;
    for (uint8_t i = 0; i < 10; i++) {
      uint8_t id = moveId(t, i);
      const MoveDef &m = moveDef(id);
      assert(m.name && m.type == t && m.pp > 0 && m.accuracy > 0 && m.accuracy <= 100);
      assert((m.category == MOVE_STATUS) == (m.power == 0));
      if (m.power >= 100) strong++;
    }
    assert(strong >= 1 || t == TYPE_POISON || t == TYPE_BUG);
  }
  assert(moveId(0, 0) == MOVE_NONE && moveId(19, 0) == MOVE_NONE && moveId(1, 10) == MOVE_NONE);
  assert(!strcmp(moveDef(moveId(TYPE_FIRE, 0)).name, "Ember") && moveDef(moveId(TYPE_FIRE, 0)).power == 40);
  assert(!strcmp(moveDef(moveId(TYPE_FAIRY, 6)).name, "Moonblast") && moveDef(moveId(TYPE_FAIRY, 6)).power == 95);
  assert(moveDef(moveId(TYPE_DRAGON, 9)).power == 130);   // Draco Meteor

  // STAB / stages
  assert(stabPct(TYPE_FIRE, TYPE_FIRE, TYPE_NONE, 0) == 150);
  assert(stabPct(TYPE_FLYING, TYPE_FIRE, TYPE_FLYING, 0) == 150);
  assert(stabPct(TYPE_WATER, TYPE_FIRE, TYPE_NONE, 0) == 100);
  assert(stabPct(TYPE_WATER, TYPE_FIRE, TYPE_NONE, TYPE_WATER) == 150);   // Tera
  assert(stagePct(0) == 100 && stagePct(3) == 250 && stagePct(6) == 400);
  assert(stagePct(-3) == 40 && stagePct(-6) == 25 && stagePct(9) == 400 && stagePct(-9) == 25);

  // damage
  const MoveDef &ember = moveDef(moveId(TYPE_FIRE, 0));
  uint16_t d1 = moveDamage(50, 50, 20, ember, 100, 100, false, 100);
  uint16_t d2 = moveDamage(50, 50, 20, ember, 100, 150, false, 100);
  assert(d2 > d1 && d1 > 0);
  assert(moveDamage(50, 50, 20, ember, 100, 100, true, 100) > d1);
  assert(moveDamage(50, 50, 20, moveDef(moveId(TYPE_NORMAL, 2)), 100, 100, false, 100) == 0);  // status
  assert(moveDamage(50, 50, 20, ember, 70, 100, false, 100) < d1);

  // crits: 6.25% vs 12.5%
  const MoveDef &slash = moveDef(moveId(TYPE_NORMAL, 4));
  assert(slash.flags & MF_HIGHCRIT);
  assert(critHit(ember, 61) && !critHit(ember, 62) && critHit(slash, 124) && !critHit(slash, 125));
  assert(multiHitCount(0) == 2 && multiHitCount(3) == 5);

  // status
  assert(statusDot(ST_BURN, 100) == 6 && statusDot(ST_POISON, 100) == 8 && statusDot(ST_SLEEP, 100) == 0);
  assert(statusDot(ST_BURN, 5) == 1);
  assert(statusAtkPct(ST_BURN) == 75 && statusSpePct(ST_PARALYZE) == 50);
  StatusState s = {}; bool self = false;
  inflictStatus(s, ST_PARALYZE, 0); inflictStatus(s, ST_BURN, 0); assert(s.status == ST_PARALYZE);
  assert(statusBlocksAction(s, 24, self) && !statusBlocksAction(s, 25, self));
  s = {}; inflictStatus(s, ST_SLEEP, 1); assert(s.turns == 2);
  assert(statusBlocksAction(s, 0, self) && s.status == ST_SLEEP);
  assert(!statusBlocksAction(s, 0, self) && s.status == ST_NONE);   // wakes after its turns
  s = {}; inflictStatus(s, ST_SLEEP, 2); statusOnDamage(s); assert(s.status == ST_NONE);
  s = {}; inflictStatus(s, ST_FLINCH, 0); assert(statusBlocksAction(s, 99, self) && s.status == ST_NONE);
  s = {}; inflictStatus(s, ST_CONFUSE, 0); assert(s.turns == 2);
  assert(statusBlocksAction(s, 10, self) && self);
  assert(!statusBlocksAction(s, 50, self) && !self && s.status == ST_NONE);

  // learnsets
  MoveSet ms;
  assert(hatchMoves(ms, TYPE_FIRE, TYPE_FLYING, 1) == 2);   // two basics below level 5
  assert(hatchMoves(ms, TYPE_FIRE, TYPE_FLYING, 5) == 3);   // + Normal utility at level >= 5
  assert(ms.slot[0].id == moveId(TYPE_FIRE, 0) && ms.slot[1].id == moveId(TYPE_FLYING, 0));
  for (uint8_t lv : {1, 5, 20}) for (uint8_t t : {TYPE_WATER, TYPE_NORMAL}) {
    uint8_t n = hatchMoves(ms, t, TYPE_NONE, lv); assert(n >= 2 && n <= 3);   // spec: 2-3 moves
  }
  assert(learnOfferFor(7, TYPE_FIRE, TYPE_NONE, 0) == MOVE_NONE);
  for (uint8_t lv = 5; lv <= 100; lv += 5)
    for (uint8_t r = 0; r < 60; r++) {
      uint8_t id = learnOfferFor(lv, TYPE_FIRE, TYPE_FLYING, r);
      assert(id < MOVE_COUNT);
      assert(moveDef(id).power <= 50 + lv * 12 / 10 + 0 || moveDef(id).power == 0);
    }
  // moveset: add, duplicate, full, replace, PP
  hatchMoves(ms, TYPE_FIRE, TYPE_FLYING, 5);               // 3 moves, 1 free slot
  assert(movesetAdd(ms, moveId(TYPE_FIRE, 0)) == -1);
  assert(movesetAdd(ms, moveId(TYPE_FIRE, 4)) == 3);
  assert(movesetAdd(ms, moveId(TYPE_FIRE, 5)) == -2);
  assert(movesetReplace(ms, 0, moveId(TYPE_FIRE, 5)) && ms.slot[0].pp == moveDef(moveId(TYPE_FIRE, 5)).pp);
  uint8_t mx = ms.slot[0].pp;
  for (int i = 0; i < mx; i++) assert(movesetUse(ms, 0));
  assert(!movesetUse(ms, 0) && ms.slot[0].pp == 0);
  movesetRestoreAfterWin(ms); assert(ms.slot[0].pp == (mx + 1) / 2);
  movesetRestoreFull(ms); assert(ms.slot[0].pp == mx);
  // AI prefers super-effective: Water vs Fire defender beats Normal move
  MoveSet ai; movesetInit(ai);
  movesetAdd(ai, moveId(TYPE_NORMAL, 0)); movesetAdd(ai, moveId(TYPE_WATER, 5));
  assert(aiPickMove(ai, TYPE_FIRE, TYPE_NONE, 100, 50) == 1);
  assert(aiPickMove(ai, TYPE_FIRE, TYPE_NONE, 0, 0) == 0);   // dumb roll -> random pick
  MoveSet empty; movesetInit(empty); assert(aiPickMove(empty, 1, 0, 100, 0) == MOVE_NONE);
  assert(sizeof(MoveSet) == 8);
  puts("move tests passed"); return 0;
}
