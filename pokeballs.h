#pragma once
// Poke Ball types and catch formula (feature list section 40).
// Pure logic, no hardware access, so it can be unit-tested on the host.
#include <stdint.h>

enum BallType : uint8_t {
  BALL_POKE = 0, BALL_GREAT, BALL_ULTRA, BALL_NET, BALL_DUSK,
  BALL_TIMER, BALL_QUICK, BALL_LUXURY, BALL_PREMIER, BALL_MASTER,
  BALL_COUNT
};

constexpr uint8_t BALL_MAX_STACK = 99;
constexpr uint8_t BALL_STARTER_GREAT = 5;
constexpr uint8_t CRIT_CATCH_PCT = 4;

struct BallInventory { uint8_t count[BALL_COUNT]; };  // 10 bytes NVS

struct CatchContext {
  uint8_t petLevel;
  uint8_t wildLevel;
  uint8_t wildHpPct;     // 0..100
  uint8_t wildType1;
  uint8_t wildType2;
  uint8_t round;         // 1-based throw/battle round
  uint8_t hour;          // 0..23 from RTC
  uint8_t trailBonusPct; // 0, 2..5 from Trail Rank
};

struct CatchOutcome {
  bool caught;
  bool critical;
  uint8_t shakes;        // 0..3 shown before result
  uint8_t ratePct;       // final clamped rate
  uint8_t bondBonus;     // Luxury Ball: +10 bond
  bool shinyRoll;        // Premier Ball: 5% shiny
};

void ballInventoryInit(BallInventory &inv);  // empty + starter gift
bool ballAvailable(const BallInventory &inv, BallType b);
bool ballConsume(BallInventory &inv, BallType b);  // Poke Ball never consumed
bool ballAdd(BallInventory &inv, BallType b, uint8_t n);  // caps at 99
bool isNightHour(uint8_t hour);  // Dusk Ball window 18:00-05:59
uint16_t ballModifierPct(BallType b, const CatchContext &c);  // 100 = 1.0x
uint8_t catchRatePct(BallType b, const CatchContext &c);
// rolls: crit 0..99, shake[3] and finalCheck 0..99, shiny 0..99 (deterministic)
CatchOutcome resolveCatch(BallType b, const CatchContext &c, uint8_t critRoll,
                          const uint8_t shakeRolls[3], uint8_t finalRoll,
                          uint8_t shinyRoll);
