#pragma once
// Seasonal events (feature list section 36), driven by the RTC calendar date.
#include <stdint.h>

enum EventFlag : uint8_t {
  EV_HALLOWEEN = 1, EV_WINTER = 2, EV_POKEMON_DAY = 4, EV_BIRTHDAY = 8,
};
struct EventEffects {
  uint8_t flags;
  bool ghostBoost, iceBoost, delibirdGift, stantlerSpawns;
  bool shinyDoubled, guaranteedRareEncounter;
  bool birthdayEgg;        // free rare egg once per birthday
  int8_t birthdayJoy;      // +25
};
// birthdayMonth/Day == 0 means unset
EventEffects eventsFor(uint8_t month, uint8_t day, uint8_t birthdayMonth, uint8_t birthdayDay);
uint16_t shinyDenominatorWithEvents(uint16_t denom, const EventEffects &e);   // doubled odds = half the denominator, floor 1
bool featuredSpecies(uint16_t dex, const EventEffects &e);   // Pumpkaboo/Phantump/Litwick, Delibird/Stantler
