#pragma once
// Move system (feature list section 39): 180 moves, STAB, status effects,
// stat stages, learnsets. Pure logic, host-testable.
#include <stdint.h>

enum MoveCategory : uint8_t { MOVE_PHYSICAL = 0, MOVE_SPECIAL, MOVE_STATUS };
enum StatusEffect : uint8_t { ST_NONE = 0, ST_BURN, ST_PARALYZE, ST_POISON, ST_SLEEP, ST_CONFUSE, ST_FLINCH };
enum MoveFlags : uint16_t {
  MF_CONTACT = 1, MF_SOUND = 2, MF_RECOIL = 4, MF_HEAL = 8,
  MF_MULTIHIT = 16, MF_HIGHCRIT = 32, MF_SELFBUFF = 64, MF_LOWERDEF = 128,
};

constexpr uint8_t MOVE_COUNT = 180;       // 10 per type x 18 types
constexpr uint8_t MOVES_PER_TYPE = 10;
constexpr uint8_t MOVE_NONE = 255;
constexpr uint8_t MOVESET_SIZE = 4;

struct MoveDef {
  const char *name;
  uint8_t type;          // TYPE_NORMAL..TYPE_FAIRY (1..18)
  uint8_t category;
  uint8_t power;         // 0 for status moves
  uint8_t accuracy;      // %
  uint8_t pp;
  uint8_t status;        // StatusEffect inflicted
  uint8_t statusChance;  // %
  int8_t priority;
  uint16_t flags;
};

struct MoveSlot { uint8_t id; uint8_t pp; };       // 2 bytes
struct MoveSet { MoveSlot slot[MOVESET_SIZE]; };   // 8 bytes NVS

const MoveDef &moveDef(uint8_t id);
uint8_t moveId(uint8_t type, uint8_t index);       // index 0..9, basic -> strong

// Combat maths
uint16_t stabPct(uint8_t moveType, uint8_t type1, uint8_t type2, uint8_t teraType);  // 100 or 150
uint16_t stagePct(int8_t stage);                    // clamp -6..+6; 25%..400%
uint16_t moveDamage(uint16_t atk, uint16_t def, uint8_t level, const MoveDef &m,
                    uint16_t typePct, uint16_t stab, bool crit, uint8_t rollPct /*85..100*/);
bool critHit(const MoveDef &m, uint8_t roll /*0..999*/);  // 6.25% / 12.5%
uint8_t multiHitCount(uint8_t roll);                // 2..5

// Status effects
struct StatusState { uint8_t status; uint8_t turns; };
uint16_t statusDot(uint8_t status, uint16_t maxHp);           // burn 6%, poison 8%
uint16_t statusAtkPct(uint8_t status);                         // burn 75
uint16_t statusSpePct(uint8_t status);                         // paralyze 50
void inflictStatus(StatusState &s, uint8_t status, uint8_t durationRoll);  // ignored if already afflicted
// returns true if the combatant loses its action; confusion self-hit is reported via selfHit
bool statusBlocksAction(StatusState &s, uint8_t roll /*0..99*/, bool &selfHit);
void statusOnDamage(StatusState &s);                           // sleep wakes on hit

// Learnsets
void movesetInit(MoveSet &ms);
uint8_t hatchMoves(MoveSet &ms, uint8_t type1, uint8_t type2, uint8_t level);  // returns count
uint8_t learnOfferFor(uint8_t level, uint8_t type1, uint8_t type2, uint8_t roll);  // level % 5 == 0
int8_t movesetAdd(MoveSet &ms, uint8_t id);        // slot index, -1 if known, -2 if full
bool movesetReplace(MoveSet &ms, uint8_t slotIdx, uint8_t id);
bool movesetUse(MoveSet &ms, uint8_t slotIdx);     // consumes 1 PP, false if empty/no PP
void movesetRestoreAfterWin(MoveSet &ms);          // +50% of max PP
void movesetRestoreFull(MoveSet &ms);
uint8_t aiPickMove(const MoveSet &ms, uint8_t defType1, uint8_t defType2, uint8_t smartness /*0..100*/,
                   uint8_t roll /*0..99*/);
