#include "quiz.h"
#include "battle.h"
#include "dex.h"   // TYPE_*

uint8_t quizChoices(uint8_t d) { return d >= QD_HARD ? 6 : 4; }
uint16_t quizTimerMs(uint8_t d) { static const uint16_t T[4] = {8000, 6000, 4500, 3000}; return T[d < 4 ? d : 3]; }
bool quizSeenOnly(uint8_t d) { return d == QD_EASY; }
bool quizShinySilhouettes(uint8_t d) { return d == QD_EXPERT; }

static uint32_t rnd(uint32_t &s) { s = s * 1664525u + 1013904223u; return s >> 8; }

bool quizBuildSpecies(uint16_t correct, uint8_t n, const uint16_t *pool, uint16_t len, uint32_t seed, QuizQuestion &q) {
  if (n < 2 || n > QUIZ_MAX_CHOICES || !pool) return false;
  uint16_t distinct = 0;                                  // count pool entries different from the answer
  for (uint16_t i = 0; i < len; i++) distinct += pool[i] != correct;
  // Distinct values only: duplicates in `pool` would not be fillable, so walk with a uniqueness check.
  q.n = 0;
  uint32_t s = seed, guard = 0;
  uint8_t pos = (uint8_t)(rnd(s) % n);
  while (q.n < n && guard++ < (uint32_t)len * 8u + 64u) {
    uint16_t cand = pool[rnd(s) % len];
    if (cand == correct) continue;
    bool dup = false;
    for (uint8_t j = 0; j < q.n; j++) if (q.choices[j] == cand) dup = true;
    if (!dup) q.choices[q.n++] = cand;
    if (q.n == n - 1) break;
  }
  if (q.n < n - 1) return false;
  // insert the correct answer at `pos`
  for (uint8_t k = q.n; k > pos; k--) q.choices[k] = q.choices[k - 1];
  q.choices[pos] = correct; q.n++; q.correctIdx = pos;
  (void)distinct;
  return true;
}

bool quizBuildType(uint8_t d1, uint8_t d2, uint8_t n, uint32_t seed, QuizQuestion &q) {
  if (n < 2 || n > QUIZ_MAX_CHOICES) return false;
  uint8_t good[18], bad[18], g = 0, b = 0;
  for (uint8_t t = TYPE_NORMAL; t <= TYPE_FAIRY; t++) {
    if (battleTypeEffectPct(t, d1, d2) > 100) good[g++] = t; else bad[b++] = t;
  }
  if (!g || b < (uint8_t)(n - 1)) return false;
  uint32_t s = seed;
  uint8_t correct = good[rnd(s) % g];
  q.n = 0;
  uint8_t pos = (uint8_t)(rnd(s) % n);
  uint8_t used = 0; uint32_t usedMask = 0;
  while (used < n - 1) {
    uint8_t cand = bad[rnd(s) % b];
    if (usedMask & (1u << cand)) continue;
    usedMask |= 1u << cand; q.choices[q.n++] = cand; used++;
  }
  for (uint8_t k = q.n; k > pos; k--) q.choices[k] = q.choices[k - 1];
  q.choices[pos] = correct; q.n++; q.correctIdx = pos;
  return true;
}

uint16_t quizPoints(uint16_t left, uint16_t total, uint8_t streak) {
  uint16_t bonus = total ? (uint16_t)((10u * (left > total ? total : left) + total - 1) / total) : 0;   // ceil, 0..10
  return (uint16_t)(10 + bonus + (streak >= 5 ? 5 : 0));
}
uint8_t quizStars(uint8_t c) { return c >= QUIZ_ROUNDS ? 3 : c >= 7 ? 2 : c >= 4 ? 1 : 0; }
QuizReward quizSessionReward(uint8_t c, uint8_t seedRoll) {
  uint8_t st = quizStars(c);
  QuizReward r = {(uint8_t)(st * 5), (uint8_t)st, false};
  r.seedDrop = (c >= QUIZ_ROUNDS) && seedRoll < 50;       // perfect 10/10: 50% seed
  return r;
}
bool quizRecordSession(QuizStats &s, uint8_t mode, uint16_t score, uint8_t correct) {
  if (mode > 2) return false;
  s.lifetimeCorrect = (uint16_t)(s.lifetimeCorrect + correct > 65535 ? 65535 : s.lifetimeCorrect + correct);
  if (correct >= QUIZ_ROUNDS && s.perfectRounds < 255) s.perfectRounds++;
  if (score > s.high[mode]) { s.high[mode] = score; return true; }
  return false;
}
