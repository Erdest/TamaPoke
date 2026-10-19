#pragma once
// Pedometer milestones and Trail Rank (feature list section 12), plus
// Wonder Trade limits and PvP rewards (section 8).
#include <stdint.h>

enum StepMilestone : uint8_t { SM_NONE = 0, SM_BRONZE, SM_SILVER, SM_GOLD };
enum TrailRank : uint8_t { TR_HATCHLING = 0, TR_WALKER, TR_JOGGER, TR_HIKER, TR_EXPLORER };
uint8_t milestoneReached(uint32_t dailySteps);             // highest reached: 1000 / 3000 / 5000
uint32_t milestoneSteps(uint8_t m);
struct StepReward { bool snack, energyDrink, trainingWeight; uint8_t premierBalls; };
StepReward milestoneReward(uint8_t m);
uint8_t trailRank(uint32_t lifetimeSteps);                 // 0 / 5k / 25k / 100k / 250k
uint8_t trailShinyReduction(uint8_t rank);                 // Jogger 4, Explorer 8
uint8_t trailCatchBonusPct(uint8_t rank);                  // +2..+5
bool trailGivesWalkJoy(uint8_t rank);                      // Walker and above
bool trailExplorerBag(uint8_t rank);                       // accessory unlocked

constexpr uint8_t WONDER_TRADES_PER_DAY = 3;
struct WonderState { uint16_t dayStamp; uint8_t used; };
bool wonderTradeAllowed(WonderState &w, uint16_t today);   // resets on a new day
bool wonderTradeConsume(WonderState &w, uint16_t today);
struct PvpReward { uint8_t joy, bond; bool trainingBoost; };
PvpReward pvpReward(bool won);                             // +15 JOY +10 bond / +5 JOY
