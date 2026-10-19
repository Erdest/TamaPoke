#include "watchface.h"
#include <stdio.h>
uint8_t statDot(uint8_t v) { return v > 40 ? DOT_GREEN : v > 20 ? DOT_ORANGE : v > 10 ? DOT_RED : DOT_RED_PULSE; }
bool watchFaceShouldActivate(uint32_t idle, bool onMain, bool manual) { return manual || (onMain && idle >= WATCH_IDLE_SECONDS); }
bool petCritical(uint8_t f, uint8_t j, uint8_t e, uint8_t h) { return f <= 10 || j <= 10 || e <= 10 || h <= 10; }
uint8_t watchInfoAt(uint32_t sec, uint8_t per) { return (uint8_t)((per ? sec / per : 0) % WI_COUNT); }
void formatClock(uint8_t h, uint8_t m, uint8_t s, bool colon, char out[10]) {
  snprintf(out, 10, "%02u%c%02u", h % 24u, colon ? ':' : ' ', m % 60u); (void)s;
}
