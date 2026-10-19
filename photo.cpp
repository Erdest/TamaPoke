#include "photo.h"
#include <stdio.h>
#include <string.h>

void photoSceneInit(PhotoScene &s) { memset(&s, 0, sizeof(s)); s.petX = PHOTO_W / 2; s.petY = PHOTO_H / 2; }
void photoCycleBg(PhotoScene &s) { s.bg = (uint8_t)((s.bg + 1) % PHOTO_BACKGROUNDS); }
void photoCycleFrame(PhotoScene &s) { s.frame = (uint8_t)((s.frame + 1) % PHOTO_FRAMES); }
bool photoAddSticker(PhotoScene &s, uint8_t type, int16_t x, int16_t y) {
  if (s.stickerCount >= PHOTO_MAX_STICKERS || type >= PHOTO_STICKER_TYPES) return false;
  s.stickers[s.stickerCount++] = {type, x, y}; return true;
}
void photoMovePet(PhotoScene &s, int16_t x, int16_t y) {
  s.petX = x < 0 ? 0 : x >= PHOTO_W ? PHOTO_W - 1 : x;
  s.petY = y < 0 ? 0 : y >= PHOTO_H ? PHOTO_H - 1 : y;
}

uint32_t bmpRowBytes() { return ((uint32_t)PHOTO_W * 3 + 3) & ~3u; }
uint32_t bmpFileSize() { return 54 + bmpRowBytes() * PHOTO_H; }
static void le32(uint8_t *p, uint32_t v) { p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }
void bmpHeader(uint8_t h[54]) {
  memset(h, 0, 54);
  h[0] = 'B'; h[1] = 'M';
  le32(h + 2, bmpFileSize()); le32(h + 10, 54);
  le32(h + 14, 40); le32(h + 18, PHOTO_W); le32(h + 22, PHOTO_H);
  h[26] = 1; h[28] = 24;                         // planes, bpp
  le32(h + 34, bmpRowBytes() * PHOTO_H);
  le32(h + 38, 2835); le32(h + 42, 2835);
}
void rgb565ToBgr24(uint16_t px, uint8_t *bgr) {
  uint8_t r = (px >> 11) & 31, g = (px >> 5) & 63, b = px & 31;
  bgr[0] = (uint8_t)((b << 3) | (b >> 2));
  bgr[1] = (uint8_t)((g << 2) | (g >> 4));
  bgr[2] = (uint8_t)((r << 3) | (r >> 2));
}
void bmpEncodeRow(const uint16_t *fb, uint8_t *out) {
  for (uint16_t x = 0; x < PHOTO_W; x++) rgb565ToBgr24(fb[x], out + x * 3);
  for (uint32_t i = PHOTO_W * 3; i < bmpRowBytes(); i++) out[i] = 0;
}
int photoNextFreeSlot(uint64_t used) {
  for (int s = 1; s <= PHOTO_MAX_SAVED; s++) if (!((used >> (s - 1)) & 1u)) return s;
  return -1;
}
void photoFilename(int slot, char *out, size_t n) { snprintf(out, n, "photo_%03d.bmp", slot); }
