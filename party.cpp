#include "party.h"
#include <string.h>

bool partyAdd(Party &p, const StoredMon &m) { if (p.count >= PARTY_SLOTS) return false; p.mon[p.count++] = m; return true; }
bool partySetActive(Party &p, uint8_t slot) {
  if (slot == 0 || slot >= p.count) return slot == 0 && p.count > 0;
  StoredMon t = p.mon[0]; p.mon[0] = p.mon[slot]; p.mon[slot] = t; return true;
}
bool partyRemove(Party &p, uint8_t slot) {
  if (slot >= p.count) return false;
  for (uint8_t i = slot; i + 1 < p.count; i++) p.mon[i] = p.mon[i + 1];
  p.count--; return true;
}

void partyTickInactive(StoredMon &m, uint32_t minutes) {
  uint32_t c = m.idleMinutes;
  uint32_t lv = (c + minutes) / 240 - c / 240;
  uint32_t bd = (c + minutes) / 120 - c / 120;
  m.idleMinutes = (uint8_t)((c + minutes) % 240);
  m.level = (uint8_t)(m.level + lv > 100 ? 100 : m.level + lv);
  m.bond = (uint8_t)(m.bond + bd > 100 ? 100 : m.bond + bd);
  uint32_t dj = minutes / 4, df = minutes / 8;
  if (m.joy > 30) m.joy = (uint8_t)(m.joy - dj < 30 ? 30 : m.joy - dj);
  if (m.food > 30) m.food = (uint8_t)(m.food - df < 30 ? 30 : m.food - df);
  m.ageHours = (uint16_t)(m.ageHours + (minutes + 59) / 60 > 65535 ? 65535 : m.ageHours + (minutes + 59) / 60);
}
bool partyEvolutionQueued(const StoredMon &m, uint8_t evoLevel) { return evoLevel && m.level >= evoLevel; }

void boxInit(BoxIndex &b) { memset(&b, 0, sizeof(b)); }
bool boxDeposit(BoxIndex &b, const BoxEntry &e) { if (b.count >= BOX_CAPACITY) return false; b.e[b.count++] = e; return true; }
bool boxRemove(BoxIndex &b, uint16_t i, BoxEntry *out) {
  if (i >= b.count) return false;
  if (out) *out = b.e[i];
  b.e[i] = b.e[b.count - 1]; b.count--; return true;
}
uint32_t boxIndexFileSize(const BoxIndex &b) { return 4u + 8u * b.count; }

void boxSortView(const BoxIndex &b, BoxSort mode, TypeOf typeOf, uint16_t *order) {
  for (uint16_t i = 0; i < b.count; i++) order[i] = i;
  auto key = [&](uint16_t i) -> uint32_t {
    const BoxEntry &e = b.e[i];
    switch (mode) {
      case BS_LEVEL: return (uint32_t)(255 - e.level) << 16 | e.species;
      case BS_TYPE: return (uint32_t)(typeOf ? typeOf(e.species) : 0) << 16 | e.species;
      case BS_NEWEST: return 0xFFFFFFFFu - e.serial;
      default: return e.species;
    }
  };
  for (uint16_t i = 1; i < b.count; i++) {            // stable insertion sort (b.count <= 960)
    uint16_t v = order[i]; uint32_t kv = key(v); int32_t j = (int32_t)i - 1;
    while (j >= 0 && key(order[j]) > kv) { order[j + 1] = order[j]; j--; }
    order[j + 1] = v;
  }
}
