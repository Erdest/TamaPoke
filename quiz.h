#pragma once
// Quiz minigame (feature list section 33). Question building and scoring.
#include <stdint.h>

enum QuizMode : uint8_t { QM_SILHOUETTE = 0, QM_TYPE, QM_CRY };
enum QuizDifficulty : uint8_t { QD_EASY = 0, QD_MEDIUM, QD_HARD, QD_EXPERT };
constexpr uint8_t QUIZ_ROUNDS = 10;
constexpr uint8_t QUIZ_MAX_CHOICES = 6;

uint8_t quizChoices(uint8_t diff);               // 4,4,6,6
uint16_t quizTimerMs(uint8_t diff);              // 8000, 6000, 4500, 3000
bool quizSeenOnly(uint8_t diff);                 // easy: seen species only
bool quizShinySilhouettes(uint8_t diff);         // expert

struct QuizQuestion { uint16_t choices[QUIZ_MAX_CHOICES]; uint8_t n; uint8_t correctIdx; };
// Species questions (silhouette / cry). `pool` = candidate dex numbers (seen species for Easy).
// Returns false if the pool cannot supply `n` distinct choices.
bool quizBuildSpecies(uint16_t correctDex, uint8_t n, const uint16_t *pool, uint16_t poolLen,
                      uint32_t seed, QuizQuestion &q);
// Type questions: choices are attacking type ids; correct one is super-effective vs the defender.
bool quizBuildType(uint8_t defType1, uint8_t defType2, uint8_t n, uint32_t seed, QuizQuestion &q);

uint16_t quizPoints(uint16_t msLeft, uint16_t msTotal, uint8_t streak);   // 10 + up to 10 speed + 5 at streak>=5
uint8_t quizStars(uint8_t correct);              // 0..3 for a 10-question session (3 = perfect)
struct QuizReward { uint8_t joy, bond; bool seedDrop; };
QuizReward quizSessionReward(uint8_t correct, uint8_t seedRoll /*0..99*/);

struct QuizStats { uint16_t high[3]; uint16_t lifetimeCorrect; uint8_t perfectRounds; };   // 9 bytes of data
bool quizRecordSession(QuizStats &s, uint8_t mode, uint16_t score, uint8_t correct);       // true on new high score
