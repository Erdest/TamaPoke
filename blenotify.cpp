#include "blenotify.h"
#include <string.h>
void notifyDefaults(NotifySettings &s) {
  memset(&s, 0, sizeof(s));
  s.enabled = 0; for (auto &c : s.category) c = 1;
  s.soundOn = 1; s.quietStart = 22; s.quietEnd = 7; s.reactionSeconds = 3; s.showAppName = 0;
}
bool inQuietHours(const NotifySettings &s, uint8_t h) {
  if (s.quietStart == s.quietEnd) return false;
  return s.quietStart < s.quietEnd ? (h >= s.quietStart && h < s.quietEnd) : (h >= s.quietStart || h < s.quietEnd);
}
bool notifyShouldReact(const NotifySettings &s, uint8_t c, uint8_t h) {
  return s.enabled && c < NC_COUNT && s.category[c] && !inQuietHours(s, h);
}
bool notifySoundAllowed(const NotifySettings &s, uint8_t h) { return s.enabled && s.soundOn && !inQuietHours(s, h); }
uint8_t notifyReactionFor(uint8_t c) { return c < NC_COUNT ? c : (uint8_t)NR_GENERIC_CHIRP; }   // one reaction per category
void notifyRingPush(NotifyRing &r, const NotifyEvent &e) {
  if (r.count == NOTIFY_RING) { r.ev[r.head] = e; r.head = (uint8_t)((r.head + 1) % NOTIFY_RING); return; }
  r.ev[(r.head + r.count) % NOTIFY_RING] = e; r.count++;
}
bool notifyRingPop(NotifyRing &r, NotifyEvent *e) {
  if (!r.count) return false;
  if (e) *e = r.ev[r.head];
  r.head = (uint8_t)((r.head + 1) % NOTIFY_RING); r.count--; return true;
}
uint8_t bleIconState(bool en, bool conn) { return !en ? BLE_ICON_GREY : conn ? BLE_ICON_BLUE_SOLID : BLE_ICON_BLUE_PULSE; }
