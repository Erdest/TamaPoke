#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../dex.h"
#include "../dex_gen9.h"
#include "../dexall.h"
#include "../gyms.h"

static const DexEntry &E(int dex) { return DEX9_TBL[dex - DEX9_FIRST]; }

int main() {
  assert(DEX9_FIRST == DEX_COUNT + 1 && DEX9_LAST == 1025 && DEX9_COUNT == 639);   // contiguous with the 386 table
  // Spot checks against known species
  assert(!strcmp(E(387).name, "TURTWIG") && E(387).type1 == TYPE_GRASS && E(387).type2 == TYPE_NONE);
  assert(!strcmp(E(658).name, "GRENINJA") && E(658).type1 == TYPE_WATER && E(658).type2 == TYPE_DARK);
  assert(!strcmp(E(1025).name, "PECHARUNT") && E(1025).type1 == TYPE_POISON && E(1025).type2 == TYPE_GHOST);
  assert(!strcmp(E(448).name, "LUCARIO") && E(448).type1 == TYPE_FIGHTING && E(448).type2 == TYPE_STEEL);
  assert(!strcmp(E(700).name, "SYLVEON") && E(700).type1 == TYPE_FAIRY);
  assert(!strcmp(DEX_TBL[384].name, "RAYQUAZA"));
  assert(E(493).rarity == R_LEGENDARIO && E(489).rarity == R_LEGENDARIO);   // Arceus, Phione
  assert(E(388).rarity == R_EVO);
  assert(E(387).evolvesTo == 388 && E(388).evolvesTo == 389 && E(389).evolvesTo == 0);
  // Table-wide sanity
  int legend = 0, rare = 0, common = 0, evo = 0;
  for (int d = DEX9_FIRST; d <= DEX9_LAST; d++) {
    const DexEntry &e = E(d);
    assert(e.name && e.name[0] && strcmp(e.name, "?") != 0);
    assert(e.type1 >= TYPE_NORMAL && e.type1 <= TYPE_FAIRY && e.type2 <= TYPE_FAIRY && e.type2 != e.type1);
    assert(e.bHp > 0 && e.bAtk > 0 && e.bDef > 0 && e.bSpe > 0);
    assert(e.evolvesTo <= DEX9_LAST && e.evolvesTo != d);   // babies may evolve into older species
    assert(e.biome <= 5);
    for (const char *c = e.name; *c; c++) assert((unsigned char)*c < 128);
    for (int l = 0; l < DEX_LANG_COUNT; l++) {
      const char *n = DEX9_NAMES[l][d - DEX9_FIRST];
      assert(n && n[0]);
      for (const char *c = n; *c; c++) assert((unsigned char)*c >= 32 && (unsigned char)*c < 127);
    }
    legend += e.rarity == R_LEGENDARIO; rare += e.rarity == R_RARO; common += e.rarity == R_COMUN; evo += e.rarity == R_EVO;
  }
  assert(legend > 50 && rare > 20 && common > 100 && evo > 100);
  // PT falls back to EN, other languages differ for at least some species
  int diff = 0;
  for (int i = 0; i < DEX9_COUNT; i++) { assert(!strcmp(DEX9_NAMES[5][i], DEX9_NAMES[1][i])); diff += strcmp(DEX9_NAMES[2][i], DEX9_NAMES[1][i]) != 0; }
  assert(diff > 100);
  // Evolution rules: valid ids and conditions, every cross-generation link present
  bool magnezone = false, munchlax = false;
  for (int i = 0; i < EVOLUTION_RULE_GEN9_COUNT; i++) {
    const EvolutionRule &r = EVOLUTION_RULES_GEN9[i];
    assert(r.from >= 1 && r.from <= DEX9_LAST && r.to >= 1 && r.to <= DEX9_LAST && r.from != r.to);
    assert(r.condition <= EVO_ATK_EQ_DEF && r.minLevel >= 1);
    if (r.from == 446 && r.to == 143) munchlax = true;   // baby -> Gen 1 species
    if (r.from == 82 && r.to == 462) magnezone = true;     // Magneton -> Magnezone (cross-gen)
  }
  assert(magnezone && munchlax && EVOLUTION_RULE_GEN9_COUNT > 250);
  // ---- unified accessor ----
  assert(dexAllEntry(0) == nullptr && dexAllEntry(1026) == nullptr);
  assert(!strcmp(dexAllEntry(1)->name, "BULBASAUR") && !strcmp(dexAllEntry(386)->name, "DEOXYS") && !strcmp(dexAllEntry(387)->name, "TURTWIG"));
  assert(!strcmp(dexAllName(25, 1), "PIKACHU") && !strcmp(dexAllName(1025, 1), "PECHARUNT") && !strcmp(dexAllName(0, 1), "?"));
  static uint16_t pool[1025];
  uint16_t nfire = dexAllPoolForType(TYPE_FIRE, false, 1025, pool, 1025);
  assert(nfire > 60); for (uint16_t i = 0; i < nfire; i++) { const DexEntry *e = dexAllEntry(pool[i]); assert(e->type1 == TYPE_FIRE || e->type2 == TYPE_FIRE); assert(e->rarity != R_LEGENDARIO); }
  assert(dexAllPoolForType(TYPE_FIRE, false, 386, pool, 1025) < nfire);              // Gen 1-3 only is smaller
  // gym team from real species: Kanto gym 1 (Rock) at level 12 vs the Paldea champion
  uint16_t rock = dexAllPoolForType(TYPE_ROCK, false, 1025, pool, 1025);
  TeamMon team[TEAM_MAX];
  assert(buildTeam(TK_GYM, 0, 0, 12, RM_NORMAL, pool, rock, 42, team) == 3);
  for (int i = 0; i < 3; i++) { const DexEntry *e = dexAllEntry(team[i].dex); assert(e && (e->type1 == TYPE_ROCK || e->type2 == TYPE_ROCK)); }
  puts("dex9 tests passed"); return 0;
}
