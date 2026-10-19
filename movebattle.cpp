#include "movebattle.h"
#include "dex.h"

void moveCombatantInit(MoveCombatant &c, const BattleStats &s, const MoveSet &ms) {
  c = MoveCombatant();
  c.stats = s;
  c.hp = c.maxHp = s.hp;
  c.moves = ms;
}

uint16_t effectiveAtk(const MoveCombatant &c) {
  return (uint16_t)((uint32_t)c.stats.atk * stagePct(c.atkStage) / 100 * statusAtkPct(c.status.status) / 100);
}
uint16_t effectiveSpe(const MoveCombatant &c) {
  return (uint16_t)((uint32_t)c.stats.spe * stagePct(c.speStage) / 100 * statusSpePct(c.status.status) / 100);
}
static uint16_t effectiveDef(const MoveCombatant &c) {
  uint32_t d = (uint32_t)c.stats.def * stagePct(c.defStage) / 100;
  return d ? (uint16_t)d : 1;
}

static void pickSlot(MoveCombatant &me, const MoveCombatant &foe, uint8_t requested,
                     uint8_t smart, RollFn roll, uint8_t &slot) {
  if (requested < MOVESET_SIZE && me.moves.slot[requested].id != MOVE_NONE &&
      me.moves.slot[requested].pp > 0) {
    slot = requested;
    return;
  }
  slot = aiPickMove(me.moves, foe.stats.type1, foe.stats.type2, smart, (uint8_t)roll(100));
}

static void damageTo(MoveCombatant &c, uint16_t d) { c.hp = d >= c.hp ? 0 : (uint16_t)(c.hp - d); }

static void act(MoveCombatant &me, MoveCombatant &foe, uint8_t slot, MoveAction &out, RollFn roll) {
  out = MoveAction();
  out.slot = slot;
  bool selfHit = false;
  if (statusBlocksAction(me.status, (uint8_t)roll(100), selfHit)) {
    out.blocked = true;
    out.selfHit = selfHit;
    if (selfHit) {
      MoveDef confusion = {"Confusion", 0, MOVE_PHYSICAL, 40, 100, 0, 0, 0, 0, 0};
      uint16_t d = moveDamage(me.stats.atk, effectiveDef(me), me.stats.level, confusion, 100, 100,
                              false, 90);
      damageTo(me, d);
      out.selfHitDamage = d;
    }
    return;
  }
  out.acted = true;
  MoveDef fallback = {"Struggle", TYPE_NORMAL, MOVE_PHYSICAL, 40, 100, 0, 0, 0, 0, MF_RECOIL};
  const MoveDef *m = &fallback;
  if (slot != MOVE_NONE) {
    out.moveId = me.moves.slot[slot].id;
    m = &moveDef(out.moveId);
    movesetUse(me.moves, slot);
  }
  if (roll(100) >= m->accuracy) { out.missed = true; return; }

  if (m->category == MOVE_STATUS || m->power == 0) {
    if (m->flags & MF_SELFBUFF) { if (me.atkStage < 6) me.atkStage++; }
    if (m->status) {
      uint8_t before = foe.status.status;
      inflictStatus(foe.status, m->status, (uint8_t)roll(100));
      if (foe.status.status != before) out.statusInflicted = m->status;
    }
    return;
  }

  uint8_t hits = (m->flags & MF_MULTIHIT) ? multiHitCount((uint8_t)roll(100)) : 1;
  uint16_t typePct = battleTypeEffectPct(m->type, foe.stats.type1, foe.stats.type2);
  out.typePct = (uint8_t)(typePct > 255 ? 255 : typePct);
  uint16_t stab = stabPct(m->type, me.stats.type1, me.stats.type2, me.teraType);
  uint16_t total = 0;
  for (uint8_t h = 0; h < hits && foe.hp > 0; h++) {
    bool crit = critHit(*m, (uint16_t)roll(1000));
    uint8_t rollPct = (uint8_t)(85 + roll(16));
    uint16_t d = moveDamage(effectiveAtk(me), effectiveDef(foe), me.stats.level, *m, typePct, stab,
                            crit, rollPct);
    if (crit) out.crit = true;
    total = (uint16_t)(total + d);
    damageTo(foe, d);
    out.hits++;
    statusOnDamage(foe.status);
  }
  out.damage = total;
  if (m->status && m->statusChance && foe.hp > 0 && roll(100) < m->statusChance) {
    uint8_t before = foe.status.status;
    inflictStatus(foe.status, m->status, (uint8_t)roll(100));
    if (foe.status.status != before) out.statusInflicted = m->status;
  }
  if ((m->flags & MF_LOWERDEF) && foe.hp > 0 && roll(100) < 30 && foe.defStage > -6) foe.defStage--;
  if (m->flags & MF_HEAL) {
    uint16_t h = (uint16_t)(total / 2);
    uint16_t room = (uint16_t)(me.maxHp - me.hp);
    out.healed = h > room ? room : h;
    me.hp = (uint16_t)(me.hp + out.healed);
  }
  if (m->flags & MF_RECOIL) {
    uint16_t r = (uint16_t)(total / 4);
    if (r == 0 && total > 0) r = 1;
    out.recoil = r;
    damageTo(me, r);
  }
}

MoveTurnReport stepMoveTurn(MoveCombatant &p, MoveCombatant &e, uint8_t playerSlot,
                            uint8_t enemySmartness, RollFn roll) {
  MoveTurnReport rep = MoveTurnReport();
  uint8_t ps, es;
  pickSlot(p, e, playerSlot, 100, roll, ps);
  pickSlot(e, p, MOVE_NONE, enemySmartness, roll, es);
  int8_t pPrio = ps == MOVE_NONE ? 0 : moveDef(p.moves.slot[ps].id).priority;
  int8_t ePrio = es == MOVE_NONE ? 0 : moveDef(e.moves.slot[es].id).priority;
  bool pFirst;
  if (pPrio != ePrio) pFirst = pPrio > ePrio;
  else {
    uint16_t sp = effectiveSpe(p), se = effectiveSpe(e);
    pFirst = sp != se ? sp > se : roll(2) == 0;
  }
  rep.playerFirst = pFirst;
  if (pFirst) {
    act(p, e, ps, rep.a[0], roll);
    if (e.hp > 0 && p.hp > 0) act(e, p, es, rep.a[1], roll);
  } else {
    act(e, p, es, rep.a[1], roll);
    if (p.hp > 0 && e.hp > 0) act(p, e, ps, rep.a[0], roll);
  }
  MoveCombatant *side[2] = {&p, &e};
  for (int i = 0; i < 2; i++) {
    if (side[i]->hp == 0) continue;
    uint16_t d = statusDot(side[i]->status.status, side[i]->maxHp);
    rep.dot[i] = d;
    damageTo(*side[i], d);
  }
  rep.playerFainted = p.hp == 0;
  rep.enemyFainted = e.hp == 0;
  return rep;
}
