#include "seasonal.h"
EventEffects eventsFor(uint8_t m, uint8_t d, uint8_t bm, uint8_t bd) {
  EventEffects e = {};
  if (m == 10) { e.flags |= EV_HALLOWEEN; e.ghostBoost = true; }
  if (m == 12) { e.flags |= EV_WINTER; e.iceBoost = e.delibirdGift = e.stantlerSpawns = true; }
  if (m == 2 && d == 27) { e.flags |= EV_POKEMON_DAY; e.shinyDoubled = e.guaranteedRareEncounter = true; }
  if (bm && bd && m == bm && d == bd) { e.flags |= EV_BIRTHDAY; e.birthdayEgg = true; e.birthdayJoy = 25; }
  return e;
}
uint16_t shinyDenominatorWithEvents(uint16_t den, const EventEffects &e) {
  if (!e.shinyDoubled) return den;
  uint16_t v = den / 2; return v ? v : 1;
}
bool featuredSpecies(uint16_t dex, const EventEffects &e) {
  if (e.flags & EV_HALLOWEEN) if (dex == 710 || dex == 708 || dex == 607) return true;   // Pumpkaboo, Phantump, Litwick
  if (e.flags & EV_WINTER) if (dex == 225 || dex == 234) return true;                    // Delibird, Stantler
  return false;
}
