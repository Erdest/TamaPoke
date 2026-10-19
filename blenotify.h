#pragma once
// Optional BLE notification bridge logic (feature list section 34).
// The BLE/GATT transport itself lives in firmware, behind TAMAPOKE_BLE_NOTIFY.
#include <stdint.h>

enum NotifyCategory : uint8_t { NC_CALL = 0, NC_MESSAGE, NC_EMAIL, NC_SOCIAL, NC_CALENDAR, NC_OTHER, NC_COUNT };
enum NotifyReaction : uint8_t { NR_URGENT_RING = 0, NR_HAPPY_BOUNCE, NR_NOD, NR_WAVE, NR_BELL_CHIRP, NR_GENERIC_CHIRP };
struct NotifySettings {                     // 16 bytes
  uint8_t enabled;
  uint8_t category[NC_COUNT];               // per-category toggles
  uint8_t soundOn;
  uint8_t quietStart, quietEnd;             // default 22 / 7
  uint8_t reactionSeconds;                  // default 3
  uint8_t showAppName;
  uint8_t reserved[4];
};
void notifyDefaults(NotifySettings &s);
bool inQuietHours(const NotifySettings &s, uint8_t hour);
bool notifyShouldReact(const NotifySettings &s, uint8_t category, uint8_t hour);
bool notifySoundAllowed(const NotifySettings &s, uint8_t hour);
uint8_t notifyReactionFor(uint8_t category);

constexpr uint8_t NOTIFY_RING = 4;
struct NotifyEvent { uint8_t category; uint8_t hour; };
struct NotifyRing { NotifyEvent ev[NOTIFY_RING]; uint8_t head, count; };
void notifyRingPush(NotifyRing &r, const NotifyEvent &e);   // newest overwrites oldest when full
bool notifyRingPop(NotifyRing &r, NotifyEvent *e);          // oldest first
enum BleIcon : uint8_t { BLE_ICON_GREY = 0, BLE_ICON_BLUE_PULSE, BLE_ICON_BLUE_SOLID };
uint8_t bleIconState(bool enabled, bool connected);
