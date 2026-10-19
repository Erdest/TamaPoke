#include "gyms.h"
#include <string.h>
#include "dex.h"   // TYPE_* ids

static const char *const REGIONS[REGION_COUNT] = {
  "Kanto", "Johto", "Hoenn", "Sinnoh", "Unova", "Kalos", "Alola", "Galar", "Paldea"};

#define G(n, t) {n, TYPE_##t}
static const TrainerDef GYMS[REGION_COUNT][GYMS_PER_REGION] = {
  {G("Brock",ROCK),G("Misty",WATER),G("Lt. Surge",ELECTRIC),G("Erika",GRASS),G("Koga",POISON),G("Sabrina",PSYCHIC),G("Blaine",FIRE),G("Giovanni",GROUND)},
  {G("Falkner",FLYING),G("Bugsy",BUG),G("Whitney",NORMAL),G("Morty",GHOST),G("Chuck",FIGHTING),G("Jasmine",STEEL),G("Pryce",ICE),G("Clair",DRAGON)},
  {G("Roxanne",ROCK),G("Brawly",FIGHTING),G("Wattson",ELECTRIC),G("Flannery",FIRE),G("Norman",NORMAL),G("Winona",FLYING),G("Tate&Liza",PSYCHIC),G("Wallace",WATER)},
  {G("Roark",ROCK),G("Gardenia",GRASS),G("Maylene",FIGHTING),G("Wake",WATER),G("Fantina",GHOST),G("Byron",STEEL),G("Candice",ICE),G("Volkner",ELECTRIC)},
  {G("Cilan",GRASS),G("Lenora",NORMAL),G("Burgh",BUG),G("Elesa",ELECTRIC),G("Clay",GROUND),G("Skyla",FLYING),G("Brycen",ICE),G("Drayden",DRAGON)},
  {G("Viola",BUG),G("Grant",ROCK),G("Korrina",FIGHTING),G("Ramos",GRASS),G("Clemont",ELECTRIC),G("Valerie",FAIRY),G("Olympia",PSYCHIC),G("Wulfric",ICE)},
  {G("Ilima",NORMAL),G("Lana",WATER),G("Kiawe",FIRE),G("Mallow",GRASS),G("Sophocles",ELECTRIC),G("Acerola",GHOST),G("Nanu",DARK),G("Hapu",GROUND)},
  {G("Milo",GRASS),G("Nessa",WATER),G("Kabu",FIRE),G("Bea",FIGHTING),G("Allister",GHOST),G("Gordie",ROCK),G("Melony",ICE),G("Raihan",DRAGON)},
  {G("Katy",BUG),G("Brassius",GRASS),G("Iono",ELECTRIC),G("Kofu",WATER),G("Larry",NORMAL),G("Ryme",GHOST),G("Tulip",PSYCHIC),G("Grusha",ICE)},
};
static const TrainerDef E4[REGION_COUNT][E4_PER_REGION] = {
  {G("Lorelei",ICE),G("Bruno",FIGHTING),G("Agatha",GHOST),G("Will",PSYCHIC)},
  {G("Will",PSYCHIC),G("Koga",POISON),G("Bruno",FIGHTING),G("Karen",DARK)},
  {G("Sidney",DARK),G("Phoebe",GHOST),G("Glacia",ICE),G("Drake",DRAGON)},
  {G("Aaron",BUG),G("Bertha",GROUND),G("Flint",FIRE),G("Lucian",PSYCHIC)},
  {G("Shauntal",GHOST),G("Marshal",FIGHTING),G("Grimsley",DARK),G("Caitlin",PSYCHIC)},
  {G("Malva",FIRE),G("Siebold",WATER),G("Wikstrom",STEEL),G("Drasna",DRAGON)},
  {G("Hala",FIGHTING),G("Olivia",ROCK),G("Molayne",STEEL),G("Kahili",FLYING)},
  {G("Marnie",DARK),G("Hop",NORMAL),G("Bede",FAIRY),G("Klara",POISON)},
  {G("Rika",GROUND),G("Poppy",STEEL),G("Larry",FLYING),G("Hassel",DRAGON)},
};
#undef G
// Mixed-type champions: Lance (Kanto), Red (Johto, bonus), then one per region.
static const TrainerDef CHAMPS[REGION_COUNT] = {
  {"Lance",0},{"Red",0},{"Steven",0},{"Cynthia",0},{"Alder",0},{"Diantha",0},{"Kukui",0},{"Leon",0},{"Geeta",0}};

const char *regionName(uint8_t r) { return REGIONS[r < REGION_COUNT ? r : 0]; }
const TrainerDef &gymLeader(uint8_t r, uint8_t g) { return GYMS[r < REGION_COUNT ? r : 0][g < GYMS_PER_REGION ? g : 0]; }
const TrainerDef &eliteFour(uint8_t r, uint8_t i) { return E4[r < REGION_COUNT ? r : 0][i < E4_PER_REGION ? i : 0]; }
const TrainerDef &champion(uint8_t r) { return CHAMPS[r < REGION_COUNT ? r : 0]; }

uint8_t gymMinPlayerLevel(uint8_t g) { return (uint8_t)(10 + 5 * (g < GYMS_PER_REGION ? g : 7)); }
uint8_t gymBaseLevel(uint8_t g) { return (uint8_t)(gymMinPlayerLevel(g) + 2); }
uint8_t gymTeamSize(uint8_t g) { static const uint8_t S[8] = {3, 3, 4, 4, 5, 5, 6, 6}; return S[g < 8 ? g : 7]; }

uint8_t scaledLevel(uint8_t base, uint8_t playerLevel, RematchTier tier) {
  int lv = base;
  if (playerLevel > base) lv = (base + playerLevel) / 2;
  lv += tier == RM_HARD ? 10 : tier == RM_ELITE ? 20 : 0;
  return (uint8_t)(lv > 100 ? 100 : lv);
}
uint16_t e4HealAmount(uint16_t maxHp) { return (uint16_t)(maxHp / 5); }

void progressInit(GymProgress &p) { memset(&p, 0, sizeof(p)); }
static uint8_t popcnt(uint16_t v) { uint8_t n = 0; while (v) { n += v & 1; v >>= 1; } return n; }
uint8_t badgesIn(const GymProgress &p, uint8_t r) { return r < REGION_COUNT ? popcnt(p.region[r] & 0xFF) : 0; }
uint8_t totalBadges(const GymProgress &p) { uint8_t n = 0; for (uint8_t r = 0; r < REGION_COUNT; r++) n += badgesIn(p, r); return n; }
uint8_t e4BeatenIn(const GymProgress &p, uint8_t r) { return r < REGION_COUNT ? popcnt((p.region[r] >> 8) & 0xF) : 0; }
uint8_t championsBeaten(const GymProgress &p) { uint8_t n = 0; for (uint8_t r = 0; r < REGION_COUNT; r++) n += (p.region[r] & BIT_CHAMP) != 0; return n; }
bool regionComplete(const GymProgress &p, uint8_t r) { return r < REGION_COUNT && (p.region[r] & ALL_BITS) == ALL_BITS; }
bool grandChampion(const GymProgress &p) { return championsBeaten(p) == REGION_COUNT; }

ChallengeResult canChallengeGym(const GymProgress &p, uint8_t r, uint8_t g, uint8_t lv) {
  if (r >= REGION_COUNT || g >= GYMS_PER_REGION) return CH_BAD_INDEX;
  if (p.region[r] & BIT_GYM(g)) return CH_ALREADY_BEATEN;
  if (g > 0 && !(p.region[r] & BIT_GYM(g - 1))) return CH_PREV_GYM_MISSING;
  if (lv < gymMinPlayerLevel(g)) return CH_LEVEL_TOO_LOW;
  return CH_OK;
}
ChallengeResult canChallengeE4(const GymProgress &p, uint8_t r) {
  if (r >= REGION_COUNT) return CH_BAD_INDEX;
  if (badgesIn(p, r) < GYMS_PER_REGION) return CH_NEED_ALL_BADGES;
  return nextE4(p, r) < 0 ? CH_ALREADY_BEATEN : CH_OK;
}
int8_t nextE4(const GymProgress &p, uint8_t r) {
  if (r >= REGION_COUNT) return -1;
  for (uint8_t i = 0; i < E4_PER_REGION; i++) if (!(p.region[r] & BIT_E4(i))) return (int8_t)i;
  return -1;
}
ChallengeResult canChallengeChampion(const GymProgress &p, uint8_t r) {
  if (r >= REGION_COUNT) return CH_BAD_INDEX;
  if (badgesIn(p, r) < GYMS_PER_REGION) return CH_NEED_ALL_BADGES;
  if (p.region[r] & BIT_CHAMP) return CH_ALREADY_BEATEN;
  return e4BeatenIn(p, r) < E4_PER_REGION ? CH_NEED_E4 : CH_OK;
}

Rewards awardGymWin(GymProgress &p, uint8_t r, uint8_t g) {
  Rewards w = {};
  if (r >= REGION_COUNT || g >= GYMS_PER_REGION || (p.region[r] & BIT_GYM(g))) return w;  // no double reward
  p.region[r] |= BIT_GYM(g);
  w.joy = 15; w.bond = 10; w.ultraBalls = 2; w.accessoryChancePct = 25; w.trainingBoost = true;
  if ((g + 1) % 4 == 0) w.luxuryBalls = 1;                    // every 4th badge
  if (badgesIn(p, r) == GYMS_PER_REGION) w.legendaryEgg = true;   // full 8 badges
  return w;
}
Rewards awardE4Win(GymProgress &p, uint8_t r, uint8_t i) {
  Rewards w = {};
  if (r >= REGION_COUNT || i >= E4_PER_REGION || (p.region[r] & BIT_E4(i))) return w;
  p.region[r] |= BIT_E4(i);
  w.joy = 10; w.bond = 5;
  return w;
}
Rewards awardChampionWin(GymProgress &p, uint8_t r, bool first) {
  Rewards w = {};
  if (r >= REGION_COUNT || (p.region[r] & BIT_CHAMP)) return w;
  p.region[r] |= BIT_CHAMP;
  w.joy = 25; w.bond = 15; w.accessoryGuaranteedRare = true; w.legendaryEgg = true;
  if (first) w.masterBalls = 1;
  if (grandChampion(p)) { w.shinyLegendaryEgg = true; w.grandChampionAchievement = true; }
  return w;
}
RematchTier rematchTier(const GymProgress &p, uint8_t r) { return r < REGION_COUNT ? (RematchTier)p.tier[r] : RM_NORMAL; }
bool setRematchTier(GymProgress &p, uint8_t r, RematchTier t) {
  if (r >= REGION_COUNT || !regionComplete(p, r) || t > RM_ELITE) return false;
  p.tier[r] = t; return true;
}

static uint32_t rnd(uint32_t &s) { s = s * 1664525u + 1013904223u; return s >> 8; }

uint8_t buildTeam(TrainerKind kind, uint8_t r, uint8_t idx, uint8_t playerLevel, RematchTier tier,
                  const uint16_t *pool, uint16_t poolLen, uint32_t seed, TeamMon out[TEAM_MAX]) {
  if (!pool || poolLen == 0) return 0;
  uint8_t size, base;
  if (kind == TK_GYM) { size = gymTeamSize(idx); base = gymBaseLevel(idx); }
  else if (kind == TK_E4) { size = E4_TEAM_SIZE; base = E4_BASE_LEVEL; }
  else { size = CHAMPION_TEAM_SIZE; base = CHAMPION_BASE_LEVEL; }
  uint32_t s = seed ^ ((uint32_t)r * 7919u) ^ ((uint32_t)kind * 104729u) ^ ((uint32_t)idx * 31u);
  uint8_t n = 0;
  uint32_t attempts = 0, maxAttempts = (uint32_t)poolLen * 8u + 16u;
  while (n < size && attempts++ < maxAttempts) {        // distinct species while the pool allows
    uint16_t dex = pool[rnd(s) % poolLen];
    bool dup = false;
    for (uint8_t j = 0; j < n; j++) if (out[j].dex == dex) dup = true;
    if (dup) continue;
    // earlier members are slightly lower level; the ace (last) gets +2
    uint8_t lv = scaledLevel(base, playerLevel, tier);
    lv = (uint8_t)(lv - (size - 1 - n) / 2);
    if (n == size - 1) lv = (uint8_t)(lv + 2 > 100 ? 100 : lv + 2);
    out[n++] = {dex, lv};
  }
  while (n < size) {   // small pool: allow repeats
    out[n] = {pool[rnd(s) % poolLen], scaledLevel(base, playerLevel, tier)};
    n++;
  }
  return n;
}
