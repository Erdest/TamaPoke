#pragma once
// Party (6 slots) and Box (SD storage) logic (feature list section 38).
#include <stdint.h>
#include "moves.h"

constexpr uint8_t PARTY_SLOTS = 6;
constexpr uint16_t BOX_CAPACITY = 960;
constexpr uint8_t NICKNAME_LEN = 12;     // 11 chars + NUL

struct StoredMon {                       // exactly 48 bytes
  uint16_t species;
  uint8_t level;
  uint16_t ageHours;
  uint8_t food, joy, ene, hyg;
  uint8_t bond, weight, poops;
  uint8_t flags;                         // bit0 shiny, bit1 sleeping, bit2 berryKnown
  uint8_t geneAtk, geneDef, geneSpe;
  uint8_t trainAtk, trainDef, trainSpe;
  uint8_t nature, teraType, medals;
  uint16_t wins, losses; uint8_t streak;
  char nickname[NICKNAME_LEN];
  MoveSet moves;                         // 8 bytes
  uint8_t idleMinutes;                   // inactive-party accumulator, 0..239
} __attribute__((packed));
static_assert(sizeof(StoredMon) == 48, "StoredMon must be 48 bytes");

enum MonFlag : uint8_t { MF_SHINY = 1, MF_SLEEPING = 2, MF_BERRY_KNOWN = 4 };
struct Party { StoredMon mon[PARTY_SLOTS]; uint8_t count; };

bool partyAdd(Party &p, const StoredMon &m);                // false when full
bool partySetActive(Party &p, uint8_t slot);                // swaps `slot` with slot 0
bool partyRemove(Party &p, uint8_t slot);                   // slot 0 cannot be removed while others exist? -> compacts
// Inactive party rates: +1 level / 240 min, +1 bond / 120 min, joy -1/4 min, food -1/8 min, floor 30.
void partyTickInactive(StoredMon &m, uint32_t minutes);
// Evolution is queued for inactive members until swapped in: returns true if a level-up passed `evoLevel`.
bool partyEvolutionQueued(const StoredMon &m, uint8_t evoLevel);

// Box index: 4-byte header + 8 bytes per entry
struct BoxEntry { uint16_t species; uint8_t level; uint8_t flags; uint32_t serial; };   // 8 bytes
struct BoxIndex { uint16_t count; uint16_t reserved; BoxEntry e[BOX_CAPACITY]; };
enum BoxSort : uint8_t { BS_DEX = 0, BS_LEVEL, BS_TYPE, BS_NEWEST };
void boxInit(BoxIndex &b);
bool boxDeposit(BoxIndex &b, const BoxEntry &e);            // false when full
bool boxRemove(BoxIndex &b, uint16_t idx, BoxEntry *out);   // swap-with-last, O(1)
typedef uint8_t (*TypeOf)(uint16_t species);
// Returns a view order (indices into b.e) written to `order` (size >= count).
void boxSortView(const BoxIndex &b, BoxSort mode, TypeOf typeOf, uint16_t *order);
uint32_t boxIndexFileSize(const BoxIndex &b);               // 4 + 8 * count
