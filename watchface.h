#pragma once
// Watch face / clock mode logic (feature list section 29).
#include <stdint.h>

enum StatDot : uint8_t { DOT_GREEN = 0, DOT_ORANGE, DOT_RED, DOT_RED_PULSE };
constexpr uint16_t WATCH_IDLE_SECONDS = 600;     // 10 minutes
constexpr uint8_t WATCH_BRIGHTNESS = 15;
constexpr uint8_t WATCH_FPS = 1;
uint8_t statDot(uint8_t value);                  // >40 green, 20..40 orange, <=20 red, <=10 pulsing
bool watchFaceShouldActivate(uint32_t idleSeconds, bool onMainScreen, bool manual);
bool petCritical(uint8_t food, uint8_t joy, uint8_t ene, uint8_t hyg);   // any stat <= 10
// Bottom rotation: steps / battery / dex count / streak
enum WatchInfo : uint8_t { WI_STEPS = 0, WI_BATTERY, WI_DEX, WI_STREAK, WI_COUNT };
uint8_t watchInfoAt(uint32_t seconds, uint8_t perItemSeconds);
void formatClock(uint8_t h, uint8_t m, uint8_t s, bool colonOn, char out[10]);   // "HH:MM" + seconds suffix
