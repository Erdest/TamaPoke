#pragma once
// Photo mode composition limits and BMP export (feature list section 41).
#include <stdint.h>
#include <stddef.h>

constexpr uint16_t PHOTO_W = 466, PHOTO_H = 466;
constexpr uint8_t PHOTO_BACKGROUNDS = 8, PHOTO_STICKER_TYPES = 12, PHOTO_FRAMES = 8;
constexpr uint8_t PHOTO_MAX_STICKERS = 8, PHOTO_MAX_SAVED = 50;

struct Sticker { uint8_t type; int16_t x, y; };
struct PhotoScene { uint8_t bg, frame, stickerCount; Sticker stickers[PHOTO_MAX_STICKERS]; int16_t petX, petY; };

void photoSceneInit(PhotoScene &s);
void photoCycleBg(PhotoScene &s);
void photoCycleFrame(PhotoScene &s);
bool photoAddSticker(PhotoScene &s, uint8_t type, int16_t x, int16_t y);   // false at 8
void photoMovePet(PhotoScene &s, int16_t x, int16_t y);                     // clamped to canvas

uint32_t bmpRowBytes();                       // 24-bit rows padded to 4 bytes (1400)
uint32_t bmpFileSize();                       // 54 + rows
void bmpHeader(uint8_t out[54]);              // BITMAPFILEHEADER + INFOHEADER, bottom-up 24-bit
void rgb565ToBgr24(uint16_t px, uint8_t *bgr);
// Writes one bottom-up BMP row (screen row `y`, top = 0) from an RGB565 framebuffer row.
void bmpEncodeRow(const uint16_t *fbRow, uint8_t *outRow);
int photoNextFreeSlot(uint64_t usedMask);     // 1..50, or -1 when full
void photoFilename(int slot, char *out, size_t n);   // photo_001.bmp
