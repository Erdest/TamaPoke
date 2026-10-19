#pragma once
// Turn engine for move-based battles (feature list section 39), built on
// moves.h. Pure logic: all randomness comes through RollFn so tests are
// deterministic. Not wired into the legacy 5-button battle unless the
// firmware is built with TAMAPOKE_MOVES.
#include <stdint.h>
#include "battle.h"
#include "moves.h"

typedef uint32_t (*RollFn)(uint32_t maxExclusive);

struct MoveCombatant {
  BattleStats stats;      // atk/def/spe/level/types
  uint16_t hp, maxHp;
  int8_t atkStage, defStage, speStage;
  StatusState status;
  MoveSet moves;
  uint8_t teraType;       // 0 = none
};

struct MoveAction {       // what one side did this turn
  uint8_t slot;           // moveset slot used, MOVE_NONE if none usable (struggle)
  uint8_t moveId;
  bool acted, missed, crit;
  bool blocked, selfHit;  // status stopped the action
  uint8_t hits;
  uint8_t typePct;
  uint16_t damage;        // dealt to the opponent
  uint16_t recoil, healed, selfHitDamage;
  uint8_t statusInflicted;
};

struct MoveTurnReport {
  MoveAction a[2];        // [0] player, [1] enemy
  bool playerFirst;
  uint16_t dot[2];        // end-of-turn burn/poison damage
  bool playerFainted, enemyFainted;
};

void moveCombatantInit(MoveCombatant &c, const BattleStats &s, const MoveSet &ms);
uint16_t effectiveAtk(const MoveCombatant &c);
uint16_t effectiveSpe(const MoveCombatant &c);
// playerSlot 0..3, or MOVE_NONE for the AI to also pick for the player (auto)
MoveTurnReport stepMoveTurn(MoveCombatant &player, MoveCombatant &enemy, uint8_t playerSlot,
                            uint8_t enemySmartness, RollFn roll);
