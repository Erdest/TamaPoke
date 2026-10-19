#pragma once
// Gym Challenge, Elite Four and Champions (feature list section 10).
// Data + progression rules + team building. Pure logic, host-testable.
#include <stdint.h>

constexpr uint8_t REGION_COUNT = 9;   // Kanto..Paldea
constexpr uint8_t GYMS_PER_REGION = 8;
constexpr uint8_t E4_PER_REGION = 4;
constexpr uint8_t TEAM_MAX = 6;

enum TrainerKind : uint8_t { TK_GYM = 0, TK_E4, TK_CHAMPION };
enum RematchTier : uint8_t { RM_NORMAL = 0, RM_HARD, RM_ELITE };  // +0 / +10 / +20 levels

struct TrainerDef { const char *name; uint8_t type; };  // type 0 = mixed team

const char *regionName(uint8_t region);
const TrainerDef &gymLeader(uint8_t region, uint8_t gym);
const TrainerDef &eliteFour(uint8_t region, uint8_t idx);
const TrainerDef &champion(uint8_t region);

uint8_t gymMinPlayerLevel(uint8_t gym);          // 10,15,20,...,45
uint8_t gymBaseLevel(uint8_t gym);
uint8_t gymTeamSize(uint8_t gym);                // 3..6
constexpr uint8_t E4_BASE_LEVEL = 52, E4_TEAM_SIZE = 5;
constexpr uint8_t CHAMPION_BASE_LEVEL = 60, CHAMPION_TEAM_SIZE = 6;
uint8_t scaledLevel(uint8_t baseLevel, uint8_t playerLevel, RematchTier tier);
uint16_t e4HealAmount(uint16_t maxHp);           // 20% between E4 members
constexpr uint8_t GAUNTLET_REST_CHARGES = 2;

// Progress: bits 0-7 gyms, 8-11 Elite Four, 12 Champion, per region.
struct GymProgress { uint16_t region[REGION_COUNT]; uint8_t tier[REGION_COUNT]; };  // 27 bytes
constexpr uint16_t BIT_GYM(uint8_t g) { return (uint16_t)(1u << g); }
constexpr uint16_t BIT_E4(uint8_t i) { return (uint16_t)(1u << (8 + i)); }
constexpr uint16_t BIT_CHAMP = 1u << 12;
constexpr uint16_t ALL_BITS = 0x1FFF;

enum ChallengeResult : uint8_t {
  CH_OK = 0, CH_BAD_INDEX, CH_ALREADY_BEATEN, CH_PREV_GYM_MISSING,
  CH_LEVEL_TOO_LOW, CH_NEED_ALL_BADGES, CH_NEED_E4,
};

void progressInit(GymProgress &p);
uint8_t badgesIn(const GymProgress &p, uint8_t region);   // 0..8
uint8_t totalBadges(const GymProgress &p);                // 0..72
uint8_t e4BeatenIn(const GymProgress &p, uint8_t region);
uint8_t championsBeaten(const GymProgress &p);            // 0..9
bool regionComplete(const GymProgress &p, uint8_t region);   // enables rematches
bool grandChampion(const GymProgress &p);                 // all 9 champions
ChallengeResult canChallengeGym(const GymProgress &p, uint8_t region, uint8_t gym, uint8_t playerLevel);
ChallengeResult canChallengeE4(const GymProgress &p, uint8_t region);
int8_t nextE4(const GymProgress &p, uint8_t region);      // first unbeaten member, -1 if all beaten
ChallengeResult canChallengeChampion(const GymProgress &p, uint8_t region);

struct Rewards {
  uint8_t joy, bond;
  uint8_t ultraBalls, luxuryBalls, masterBalls;
  uint8_t accessoryChancePct;        // gym drops (25%)
  bool accessoryGuaranteedRare;      // champion
  bool trainingBoost;
  bool legendaryEgg, shinyLegendaryEgg;
  bool grandChampionAchievement;
};
// Records the win in `p` and returns the rewards due (call once per victory).
Rewards awardGymWin(GymProgress &p, uint8_t region, uint8_t gym);
Rewards awardE4Win(GymProgress &p, uint8_t region, uint8_t idx);
Rewards awardChampionWin(GymProgress &p, uint8_t region, bool firstChampionEver);
RematchTier rematchTier(const GymProgress &p, uint8_t region);
bool setRematchTier(GymProgress &p, uint8_t region, RematchTier t);

struct TeamMon { uint16_t dex; uint8_t level; };
// Builds a team from `pool` (dex numbers matching the trainer's type; for mixed
// champion teams pass a pool of any types). Deterministic given `seed`.
uint8_t buildTeam(TrainerKind kind, uint8_t region, uint8_t index, uint8_t playerLevel,
                  RematchTier tier, const uint16_t *pool, uint16_t poolLen,
                  uint32_t seed, TeamMon out[TEAM_MAX]);
