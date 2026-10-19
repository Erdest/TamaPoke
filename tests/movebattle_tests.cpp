#include <assert.h>
#include <stdio.h>
#include "../movebattle.h"
#include "../dex.h"

// Scripted rolls: every call returns `fixedRoll` clamped below max (0 = always
// hit, no crit, min variation), unless overridden.
static uint32_t gRoll = 0;
static uint32_t rollFn(uint32_t max) { return gRoll < max ? gRoll : max - 1; }

static MoveCombatant make(uint8_t t1, uint16_t hp, uint16_t atk, uint16_t def, uint16_t spe,
                          uint8_t m0, uint8_t m1 = MOVE_NONE) {
  BattleStats s = {hp, atk, def, spe, 30, t1, 0};
  MoveSet ms;
  movesetInit(ms);
  ms.slot[0] = {m0, moveDef(m0).pp};
  if (m1 != MOVE_NONE) ms.slot[1] = {m1, moveDef(m1).pp};
  MoveCombatant c;
  moveCombatantInit(c, s, ms);
  return c;
}

int main() {
  const uint8_t ember = moveId(TYPE_FIRE, 0), tackle = moveId(TYPE_NORMAL, 0);
  const uint8_t waterGun = moveId(TYPE_WATER, 0), quick = moveId(TYPE_NORMAL, 1);
  const uint8_t swords = moveId(TYPE_NORMAL, 2);

  // faster side acts first, damage lands, PP is spent
  gRoll = 50;
  MoveCombatant p = make(TYPE_FIRE, 100, 60, 50, 80, ember);
  MoveCombatant e = make(TYPE_GRASS, 100, 50, 50, 40, tackle);
  MoveTurnReport r = stepMoveTurn(p, e, 0, 100, rollFn);
  assert(r.playerFirst);
  assert(r.a[0].acted && r.a[0].damage > 0 && r.a[0].typePct == battleTypeEffectPct(TYPE_FIRE, TYPE_GRASS, 0) && r.a[0].typePct > 100);
  assert(e.hp < 100 && p.hp < 100);
  assert(p.moves.slot[0].pp == moveDef(ember).pp - 1);

  // type matchup: super effective beats resisted
  MoveCombatant p2 = make(TYPE_FIRE, 100, 60, 50, 80, ember);
  MoveCombatant waterFoe = make(TYPE_WATER, 100, 50, 50, 40, tackle);
  stepMoveTurn(p2, waterFoe, 0, 100, rollFn);
  assert(100 - waterFoe.hp < 100 - e.hp);

  // priority beats speed
  MoveCombatant slow = make(TYPE_NORMAL, 100, 50, 50, 10, quick);
  MoveCombatant fast = make(TYPE_NORMAL, 100, 50, 50, 99, tackle);
  r = stepMoveTurn(slow, fast, 0, 100, rollFn);
  assert(r.playerFirst);

  // a miss deals nothing
  gRoll = 99;  // accuracy 100 -> roll 99 still hits; use a low-accuracy move
  MoveCombatant hb = make(TYPE_NORMAL, 100, 80, 50, 80, moveId(TYPE_NORMAL, 9));  // Hyper Beam 90%
  MoveCombatant tgt = make(TYPE_NORMAL, 100, 50, 50, 40, tackle);
  r = stepMoveTurn(hb, tgt, 0, 100, rollFn);
  assert(r.a[0].missed && r.a[0].damage == 0);

  // Swords Dance raises attack stage
  gRoll = 0;
  MoveCombatant buffer = make(TYPE_NORMAL, 100, 50, 50, 80, swords);
  MoveCombatant dummy = make(TYPE_NORMAL, 100, 50, 50, 40, tackle);
  stepMoveTurn(buffer, dummy, 0, 100, rollFn);
  assert(buffer.atkStage == 1 && effectiveAtk(buffer) > 50);

  // fainting ends the turn: the KO'd side does not act
  gRoll = 50;
  MoveCombatant big = make(TYPE_FIRE, 500, 200, 50, 80, ember);
  MoveCombatant weak = make(TYPE_GRASS, 5, 50, 50, 40, tackle);
  r = stepMoveTurn(big, weak, 0, 100, rollFn);
  assert(r.enemyFainted && !r.a[1].acted && big.hp == 500);

  // burn: chip damage at end of turn and reduced attack
  MoveCombatant burned = make(TYPE_NORMAL, 100, 80, 50, 80, tackle);
  burned.status.status = ST_BURN;
  assert(effectiveAtk(burned) == 60);
  MoveCombatant foe = make(TYPE_NORMAL, 100, 50, 50, 40, tackle);
  r = stepMoveTurn(burned, foe, 0, 100, rollFn);
  assert(r.dot[0] == statusDot(ST_BURN, 100));

  // empty moveset -> struggle, still acts
  MoveCombatant none = make(TYPE_NORMAL, 100, 50, 50, 80, tackle);
  none.moves.slot[0].pp = 0;
  r = stepMoveTurn(none, foe, 0, 100, rollFn);
  assert(r.a[0].acted && r.a[0].slot == MOVE_NONE && r.a[0].recoil <= r.a[0].damage);

  // determinism: same rolls -> same result
  MoveCombatant a1 = make(TYPE_FIRE, 100, 60, 50, 80, ember), b1 = make(TYPE_GRASS, 100, 50, 50, 40, tackle);
  MoveCombatant a2 = a1, b2 = b1;
  gRoll = 33; stepMoveTurn(a1, b1, 0, 50, rollFn);
  gRoll = 33; stepMoveTurn(a2, b2, 0, 50, rollFn);
  assert(a1.hp == a2.hp && b1.hp == b2.hp);
  (void)waterGun;

  puts("movebattle tests passed");
  return 0;
}
