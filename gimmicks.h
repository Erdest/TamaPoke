#pragma once
// Battle gimmicks: Mega Evolution, Dynamax/Gigantamax, Terastallization
// (feature list section 7). Pure logic, host-testable.
#include <stdint.h>

enum Gimmick : uint8_t { GM_NONE = 0, GM_MEGA, GM_DYNAMAX, GM_TERA };
constexpr uint8_t MEGA_BOND = 20, DYNAMAX_BOND = 25, TERA_BOND = 30;
constexpr uint8_t DYNAMAX_TURNS = 3;

// bitmask of what the player may activate right now (1<<GM_*)
constexpr uint8_t GMF_MEGA = 1 << GM_MEGA, GMF_DYNAMAX = 1 << GM_DYNAMAX, GMF_TERA = 1 << GM_TERA;

struct GimmickState { uint8_t used; uint8_t active; uint8_t turnsLeft; uint8_t teraType; };

bool hasMega(uint16_t dex);          // species with at least one Mega form
bool hasMegaXY(uint16_t dex);        // Charizard / Mewtwo have X and Y forms
bool hasGigantamax(uint16_t dex);
uint8_t megaFormCount();             // total Mega forms (48)
uint8_t gmaxSpeciesCount();          // 32
uint8_t availableGimmicks(uint16_t dex, uint8_t bond, const GimmickState &s);
// Activates; `tera`-type must be valid (1..18). Returns false if not allowed.
bool activateGimmick(GimmickState &s, uint16_t dex, uint8_t bond, Gimmick g);
// End of turn: returns true when Dynamax just wore off.
bool gimmickEndTurn(GimmickState &s);
void gimmickBattleEnd(GimmickState &s);    // clears active/used (tera type persists)
uint16_t megaStatPct(Gimmick active, bool isSpeed, bool isAtkDef);   // 130 / 120 / 100
uint16_t dynamaxMaxHp(uint16_t baseMax);                              // doubles
uint16_t dynamaxEndHp(uint16_t curHp, uint16_t dynMax, uint16_t baseMax);  // keeps HP ratio, >= 1
uint8_t rollTeraType(uint8_t roll);        // 1..18 assigned at hatch
