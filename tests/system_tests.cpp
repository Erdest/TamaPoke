#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../party.h"
#include "../photo.h"
#include "../blenotify.h"
#include "../watchface.h"
#include "../dex.h"

static uint8_t fakeType(uint16_t sp) { return (uint8_t)(sp % 3 + 1); }
static StoredMon mk(uint16_t sp, uint8_t lv) { StoredMon m; memset(&m, 0, sizeof(m)); m.species = sp; m.level = lv; m.joy = 80; m.food = 80; return m; }

int main() {
  // ---- party ----
  assert(sizeof(StoredMon) == 48);
  Party p; memset(&p, 0, sizeof(p));
  for (int i = 0; i < 6; i++) assert(partyAdd(p, mk(10 + i, 5)));
  assert(!partyAdd(p, mk(99, 1)) && p.count == 6);
  assert(partySetActive(p, 3) && p.mon[0].species == 13 && p.mon[3].species == 10);
  assert(!partySetActive(p, 6) && partyRemove(p, 3) && p.count == 5 && p.mon[3].species == 14);
  StoredMon m = mk(25, 10); m.bond = 10; m.joy = 100; m.food = 100;
  partyTickInactive(m, 240);                                   // 1 level, 2 bond
  assert(m.level == 11 && m.bond == 12 && m.idleMinutes == 0);
  partyTickInactive(m, 100); partyTickInactive(m, 100); partyTickInactive(m, 100);   // 300 min -> carry
  assert(m.level == 12 && m.idleMinutes == 60);
  assert(m.joy == 30 && m.food == 34);                         // joy floors at 30; food: -30, then -12 x3
  StoredMon low = mk(1, 5); low.joy = 20; partyTickInactive(low, 400); assert(low.joy == 20);   // already below floor: unchanged
  assert(partyEvolutionQueued(m, 12) && !partyEvolutionQueued(m, 13) && !partyEvolutionQueued(m, 0));
  StoredMon cap = mk(1, 99); partyTickInactive(cap, 240 * 5); assert(cap.level == 100);
  // ---- box ----
  static BoxIndex b; boxInit(b);
  assert(sizeof(BoxEntry) == 8 && boxIndexFileSize(b) == 4);
  for (uint16_t i = 0; i < BOX_CAPACITY; i++) assert(boxDeposit(b, {(uint16_t)(1 + i % 900), (uint8_t)(1 + i % 100), 0, i}));
  assert(!boxDeposit(b, {1, 1, 0, 9999}) && boxIndexFileSize(b) == 4 + 8 * 960);
  BoxEntry out; assert(boxRemove(b, 0, &out) && out.serial == 0 && b.count == 959 && b.e[0].serial == 959);   // swap-with-last
  assert(!boxRemove(b, 959, nullptr));
  static BoxIndex s; boxInit(s);
  boxDeposit(s, {30, 50, 0, 1}); boxDeposit(s, {10, 70, 0, 2}); boxDeposit(s, {20, 50, 0, 3}); boxDeposit(s, {40, 5, 0, 4});
  uint16_t ord[4];
  boxSortView(s, BS_DEX, nullptr, ord);    assert(s.e[ord[0]].species == 10 && s.e[ord[3]].species == 40);
  boxSortView(s, BS_LEVEL, nullptr, ord);  assert(s.e[ord[0]].level == 70 && s.e[ord[3]].level == 5);
  boxSortView(s, BS_NEWEST, nullptr, ord); assert(s.e[ord[0]].serial == 4 && s.e[ord[3]].serial == 1);
  boxSortView(s, BS_TYPE, fakeType, ord);  assert(fakeType(s.e[ord[0]].species) <= fakeType(s.e[ord[3]].species));

  // ---- photo ----
  PhotoScene sc; photoSceneInit(sc);
  assert(sc.petX == 233 && sc.stickerCount == 0);
  for (int i = 0; i < 8; i++) { photoCycleBg(sc); }
  assert(sc.bg == 0);
  for (int i = 0; i < 9; i++) { photoCycleFrame(sc); }
  assert(sc.frame == 1);
  for (int i = 0; i < 8; i++) assert(photoAddSticker(sc, (uint8_t)i, 1, 1));
  assert(!photoAddSticker(sc, 0, 0, 0)); PhotoScene s2; photoSceneInit(s2); assert(!photoAddSticker(s2, 12, 0, 0));
  photoMovePet(sc, -50, 9999); assert(sc.petX == 0 && sc.petY == 465);
  assert(bmpRowBytes() == 1400 && bmpFileSize() == 54 + 1400 * 466 && bmpFileSize() / 1000 == 652);
  uint8_t h[54]; bmpHeader(h);
  assert(h[0] == 'B' && h[1] == 'M' && h[10] == 54 && h[18] == (466 & 255) && h[19] == (466 >> 8) && h[28] == 24);
  assert((uint32_t)h[2] + ((uint32_t)h[3] << 8) + ((uint32_t)h[4] << 16) == bmpFileSize());
  uint8_t px[3]; rgb565ToBgr24(0xF800, px); assert(px[2] == 255 && px[1] == 0 && px[0] == 0);   // red
  rgb565ToBgr24(0x07E0, px); assert(px[1] == 255 && px[2] == 0); rgb565ToBgr24(0x001F, px); assert(px[0] == 255 && px[1] == 0);
  rgb565ToBgr24(0xFFFF, px); assert(px[0] == 255 && px[1] == 255 && px[2] == 255);
  static uint16_t row[466]; static uint8_t enc[1400]; for (int i = 0; i < 466; i++) row[i] = 0xF800;
  memset(enc, 7, sizeof(enc)); bmpEncodeRow(row, enc); assert(enc[2] == 255 && enc[1398] == 0 && enc[1399] == 0);
  assert(photoNextFreeSlot(0) == 1 && photoNextFreeSlot(0x3) == 3 && photoNextFreeSlot((1ull << 50) - 1) == -1);
  char fn[20]; photoFilename(7, fn, sizeof(fn)); assert(!strcmp(fn, "photo_007.bmp"));

  // ---- BLE notify ----
  NotifySettings ns; notifyDefaults(ns);
  assert(sizeof(NotifySettings) == 16 && !notifyShouldReact(ns, NC_CALL, 12));       // disabled by default
  ns.enabled = 1;
  assert(notifyShouldReact(ns, NC_CALL, 12) && !notifyShouldReact(ns, NC_CALL, 23) && !notifyShouldReact(ns, NC_CALL, 6) && notifyShouldReact(ns, NC_CALL, 7));
  ns.category[NC_EMAIL] = 0; assert(!notifyShouldReact(ns, NC_EMAIL, 12) && !notifyShouldReact(ns, 9, 12));
  assert(notifySoundAllowed(ns, 12) && !notifySoundAllowed(ns, 2)); ns.soundOn = 0; assert(!notifySoundAllowed(ns, 12));
  NotifyRing r = {}; for (uint8_t i = 0; i < 6; i++) notifyRingPush(r, {i, 0});
  NotifyEvent ev; assert(r.count == 4 && notifyRingPop(r, &ev) && ev.category == 2);      // 0,1 overwritten
  assert(notifyRingPop(r, &ev) && ev.category == 3 && notifyRingPop(r, &ev) && notifyRingPop(r, &ev) && ev.category == 5 && !notifyRingPop(r, &ev));
  assert(bleIconState(false, true) == BLE_ICON_GREY && bleIconState(true, false) == BLE_ICON_BLUE_PULSE && bleIconState(true, true) == BLE_ICON_BLUE_SOLID);

  // ---- watch face ----
  assert(statDot(100) == DOT_GREEN && statDot(41) == DOT_GREEN && statDot(40) == DOT_ORANGE && statDot(21) == DOT_ORANGE);
  assert(statDot(20) == DOT_RED && statDot(11) == DOT_RED && statDot(10) == DOT_RED_PULSE && statDot(0) == DOT_RED_PULSE);
  assert(!watchFaceShouldActivate(599, true, false) && watchFaceShouldActivate(600, true, false) && !watchFaceShouldActivate(900, false, false) && watchFaceShouldActivate(0, false, true));
  assert(petCritical(100, 100, 10, 100) && !petCritical(11, 11, 11, 11));
  assert(watchInfoAt(0, 5) == WI_STEPS && watchInfoAt(5, 5) == WI_BATTERY && watchInfoAt(20, 5) == WI_STEPS);
  char clk[10]; formatClock(9, 5, 30, true, clk); assert(!strcmp(clk, "09:05")); formatClock(9, 5, 30, false, clk); assert(!strcmp(clk, "09 05"));
  puts("system tests passed"); return 0;
}
