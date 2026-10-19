#include "moves.h"
#include <string.h>
#include "dex.h"   // TYPE_* ids
#include "battle.h"  // battleTypeEffectPct

// Rows: name, type, category, power, accuracy, pp, status, chance%, priority, flags.
// Move id = (type - 1) * 10 + index. Status codes follow enum StatusEffect.
static const MoveDef MOVES[MOVE_COUNT] = {
  {"Tackle", TYPE_NORMAL, MOVE_PHYSICAL, 40, 100, 35, 0, 0, 0, MF_CONTACT},
  {"Quick Attack", TYPE_NORMAL, MOVE_PHYSICAL, 40, 100, 30, 0, 0, 1, MF_CONTACT},
  {"Swords Dance", TYPE_NORMAL, MOVE_STATUS, 0, 100, 20, 0, 0, 0, MF_SELFBUFF},
  {"Swift", TYPE_NORMAL, MOVE_SPECIAL, 60, 100, 20, 0, 0, 0, 0},
  {"Slash", TYPE_NORMAL, MOVE_PHYSICAL, 70, 100, 20, 0, 0, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Body Slam", TYPE_NORMAL, MOVE_PHYSICAL, 85, 100, 15, 2, 30, 0, MF_CONTACT},
  {"Hyper Voice", TYPE_NORMAL, MOVE_SPECIAL, 90, 100, 10, 0, 0, 0, MF_SOUND},
  {"Return", TYPE_NORMAL, MOVE_PHYSICAL, 100, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Double-Edge", TYPE_NORMAL, MOVE_PHYSICAL, 120, 100, 15, 0, 0, 0, MF_CONTACT | MF_RECOIL},
  {"Hyper Beam", TYPE_NORMAL, MOVE_SPECIAL, 150, 90, 5, 0, 0, 0, 0},
  {"Ember", TYPE_FIRE, MOVE_SPECIAL, 40, 100, 25, 1, 10, 0, 0},
  {"Flame Charge", TYPE_FIRE, MOVE_PHYSICAL, 50, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Fire Fang", TYPE_FIRE, MOVE_PHYSICAL, 65, 95, 15, 1, 10, 0, MF_CONTACT},
  {"Fire Punch", TYPE_FIRE, MOVE_PHYSICAL, 75, 100, 15, 1, 10, 0, MF_CONTACT},
  {"Flamethrower", TYPE_FIRE, MOVE_SPECIAL, 90, 100, 15, 1, 10, 0, 0},
  {"Lava Plume", TYPE_FIRE, MOVE_SPECIAL, 80, 100, 15, 1, 30, 0, 0},
  {"Flare Blitz", TYPE_FIRE, MOVE_PHYSICAL, 120, 100, 15, 1, 10, 0, MF_CONTACT | MF_RECOIL},
  {"Fire Blast", TYPE_FIRE, MOVE_SPECIAL, 110, 85, 5, 1, 10, 0, 0},
  {"Blast Burn", TYPE_FIRE, MOVE_SPECIAL, 150, 90, 5, 0, 0, 0, 0},
  {"Overheat", TYPE_FIRE, MOVE_SPECIAL, 130, 90, 5, 0, 0, 0, 0},
  {"Water Gun", TYPE_WATER, MOVE_SPECIAL, 40, 100, 25, 0, 0, 0, 0},
  {"Aqua Jet", TYPE_WATER, MOVE_PHYSICAL, 40, 100, 20, 0, 0, 1, MF_CONTACT},
  {"Bubble Beam", TYPE_WATER, MOVE_SPECIAL, 65, 100, 20, 0, 0, 0, 0},
  {"Waterfall", TYPE_WATER, MOVE_PHYSICAL, 80, 100, 15, 6, 20, 0, MF_CONTACT},
  {"Scald", TYPE_WATER, MOVE_SPECIAL, 80, 100, 15, 1, 30, 0, 0},
  {"Surf", TYPE_WATER, MOVE_SPECIAL, 90, 100, 15, 0, 0, 0, 0},
  {"Liquidation", TYPE_WATER, MOVE_PHYSICAL, 85, 100, 10, 0, 0, 0, MF_CONTACT | MF_LOWERDEF},
  {"Aqua Tail", TYPE_WATER, MOVE_PHYSICAL, 90, 90, 10, 0, 0, 0, MF_CONTACT},
  {"Hydro Pump", TYPE_WATER, MOVE_SPECIAL, 110, 80, 5, 0, 0, 0, 0},
  {"Hydro Cannon", TYPE_WATER, MOVE_SPECIAL, 150, 90, 5, 0, 0, 0, 0},
  {"Thunder Shock", TYPE_ELECTRIC, MOVE_SPECIAL, 40, 100, 30, 2, 10, 0, 0},
  {"Nuzzle", TYPE_ELECTRIC, MOVE_PHYSICAL, 20, 100, 20, 2, 100, 0, MF_CONTACT},
  {"Spark", TYPE_ELECTRIC, MOVE_PHYSICAL, 65, 100, 20, 2, 30, 0, MF_CONTACT},
  {"Thunder Fang", TYPE_ELECTRIC, MOVE_PHYSICAL, 65, 95, 15, 2, 10, 0, MF_CONTACT},
  {"Discharge", TYPE_ELECTRIC, MOVE_SPECIAL, 80, 100, 15, 2, 30, 0, 0},
  {"Thunder Punch", TYPE_ELECTRIC, MOVE_PHYSICAL, 75, 100, 15, 2, 10, 0, MF_CONTACT},
  {"Thunderbolt", TYPE_ELECTRIC, MOVE_SPECIAL, 90, 100, 15, 2, 10, 0, 0},
  {"Wild Charge", TYPE_ELECTRIC, MOVE_PHYSICAL, 90, 100, 15, 0, 0, 0, MF_CONTACT | MF_RECOIL},
  {"Thunder", TYPE_ELECTRIC, MOVE_SPECIAL, 110, 70, 10, 2, 30, 0, 0},
  {"Zap Cannon", TYPE_ELECTRIC, MOVE_SPECIAL, 120, 50, 5, 2, 100, 0, 0},
  {"Vine Whip", TYPE_GRASS, MOVE_PHYSICAL, 45, 100, 25, 0, 0, 0, MF_CONTACT},
  {"Bullet Seed", TYPE_GRASS, MOVE_PHYSICAL, 25, 100, 30, 0, 0, 0, MF_MULTIHIT},
  {"Razor Leaf", TYPE_GRASS, MOVE_PHYSICAL, 55, 95, 25, 0, 0, 0, MF_HIGHCRIT},
  {"Giga Drain", TYPE_GRASS, MOVE_SPECIAL, 75, 100, 10, 0, 0, 0, MF_HEAL},
  {"Seed Bomb", TYPE_GRASS, MOVE_PHYSICAL, 80, 100, 15, 0, 0, 0, 0},
  {"Energy Ball", TYPE_GRASS, MOVE_SPECIAL, 90, 100, 10, 0, 0, 0, MF_LOWERDEF},
  {"Leaf Blade", TYPE_GRASS, MOVE_PHYSICAL, 90, 100, 15, 0, 0, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Wood Hammer", TYPE_GRASS, MOVE_PHYSICAL, 120, 100, 15, 0, 0, 0, MF_CONTACT | MF_RECOIL},
  {"Solar Beam", TYPE_GRASS, MOVE_SPECIAL, 120, 100, 10, 0, 0, 0, 0},
  {"Frenzy Plant", TYPE_GRASS, MOVE_SPECIAL, 150, 90, 5, 0, 0, 0, 0},
  {"Powder Snow", TYPE_ICE, MOVE_SPECIAL, 40, 100, 25, 0, 0, 0, 0},
  {"Ice Shard", TYPE_ICE, MOVE_PHYSICAL, 40, 100, 30, 0, 0, 1, 0},
  {"Icy Wind", TYPE_ICE, MOVE_SPECIAL, 55, 95, 15, 0, 0, 0, MF_LOWERDEF},
  {"Ice Fang", TYPE_ICE, MOVE_PHYSICAL, 65, 95, 15, 6, 10, 0, MF_CONTACT},
  {"Ice Punch", TYPE_ICE, MOVE_PHYSICAL, 75, 100, 15, 0, 0, 0, MF_CONTACT},
  {"Ice Beam", TYPE_ICE, MOVE_SPECIAL, 90, 100, 10, 0, 0, 0, 0},
  {"Avalanche", TYPE_ICE, MOVE_PHYSICAL, 90, 100, 10, 0, 0, 0, MF_CONTACT},
  {"Icicle Crash", TYPE_ICE, MOVE_PHYSICAL, 85, 90, 10, 6, 30, 0, 0},
  {"Blizzard", TYPE_ICE, MOVE_SPECIAL, 110, 70, 5, 0, 0, 0, 0},
  {"Freeze-Dry", TYPE_ICE, MOVE_SPECIAL, 70, 100, 20, 0, 0, 0, 0},
  {"Karate Chop", TYPE_FIGHTING, MOVE_PHYSICAL, 50, 100, 25, 0, 0, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Mach Punch", TYPE_FIGHTING, MOVE_PHYSICAL, 40, 100, 30, 0, 0, 1, MF_CONTACT},
  {"Low Kick", TYPE_FIGHTING, MOVE_PHYSICAL, 60, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Brick Break", TYPE_FIGHTING, MOVE_PHYSICAL, 75, 100, 15, 0, 0, 0, MF_CONTACT},
  {"Drain Punch", TYPE_FIGHTING, MOVE_PHYSICAL, 75, 100, 10, 0, 0, 0, MF_CONTACT | MF_HEAL},
  {"Aura Sphere", TYPE_FIGHTING, MOVE_SPECIAL, 80, 100, 20, 0, 0, 0, 0},
  {"Close Combat", TYPE_FIGHTING, MOVE_PHYSICAL, 120, 100, 5, 0, 0, 0, MF_CONTACT | MF_RECOIL},
  {"Cross Chop", TYPE_FIGHTING, MOVE_PHYSICAL, 100, 80, 5, 0, 0, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Focus Blast", TYPE_FIGHTING, MOVE_SPECIAL, 120, 70, 5, 0, 0, 0, 0},
  {"Dynamic Punch", TYPE_FIGHTING, MOVE_PHYSICAL, 100, 50, 5, 5, 100, 0, MF_CONTACT},
  {"Poison Sting", TYPE_POISON, MOVE_PHYSICAL, 15, 100, 35, 3, 30, 0, 0},
  {"Acid", TYPE_POISON, MOVE_SPECIAL, 40, 100, 30, 0, 0, 0, MF_LOWERDEF},
  {"Smog", TYPE_POISON, MOVE_SPECIAL, 30, 70, 20, 3, 40, 0, 0},
  {"Poison Fang", TYPE_POISON, MOVE_PHYSICAL, 50, 100, 15, 3, 50, 0, MF_CONTACT},
  {"Sludge", TYPE_POISON, MOVE_SPECIAL, 65, 100, 20, 3, 30, 0, 0},
  {"Cross Poison", TYPE_POISON, MOVE_PHYSICAL, 70, 100, 20, 3, 10, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Sludge Bomb", TYPE_POISON, MOVE_SPECIAL, 90, 100, 10, 3, 30, 0, 0},
  {"Poison Jab", TYPE_POISON, MOVE_PHYSICAL, 80, 100, 20, 3, 30, 0, MF_CONTACT},
  {"Gunk Shot", TYPE_POISON, MOVE_PHYSICAL, 120, 80, 5, 3, 30, 0, 0},
  {"Belch", TYPE_POISON, MOVE_SPECIAL, 120, 90, 10, 0, 0, 0, 0},
  {"Mud-Slap", TYPE_GROUND, MOVE_SPECIAL, 20, 100, 10, 0, 0, 0, MF_LOWERDEF},
  {"Sand Attack", TYPE_GROUND, MOVE_STATUS, 0, 100, 15, 0, 0, 0, 0},
  {"Bulldoze", TYPE_GROUND, MOVE_PHYSICAL, 60, 100, 20, 0, 0, 0, 0},
  {"Mud Shot", TYPE_GROUND, MOVE_SPECIAL, 55, 95, 15, 0, 0, 0, 0},
  {"Dig", TYPE_GROUND, MOVE_PHYSICAL, 80, 100, 10, 0, 0, 0, MF_CONTACT},
  {"Bone Club", TYPE_GROUND, MOVE_PHYSICAL, 65, 85, 20, 6, 10, 0, 0},
  {"Earth Power", TYPE_GROUND, MOVE_SPECIAL, 90, 100, 10, 0, 0, 0, MF_LOWERDEF},
  {"Earthquake", TYPE_GROUND, MOVE_PHYSICAL, 100, 100, 10, 0, 0, 0, 0},
  {"Drill Run", TYPE_GROUND, MOVE_PHYSICAL, 80, 95, 10, 0, 0, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Precipice Blades", TYPE_GROUND, MOVE_PHYSICAL, 120, 85, 10, 0, 0, 0, 0},
  {"Gust", TYPE_FLYING, MOVE_SPECIAL, 40, 100, 35, 0, 0, 0, 0},
  {"Peck", TYPE_FLYING, MOVE_PHYSICAL, 35, 100, 35, 0, 0, 0, MF_CONTACT},
  {"Wing Attack", TYPE_FLYING, MOVE_PHYSICAL, 60, 100, 35, 0, 0, 0, MF_CONTACT},
  {"Air Cutter", TYPE_FLYING, MOVE_SPECIAL, 60, 95, 25, 0, 0, 0, MF_HIGHCRIT},
  {"Aerial Ace", TYPE_FLYING, MOVE_PHYSICAL, 60, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Air Slash", TYPE_FLYING, MOVE_SPECIAL, 75, 95, 15, 6, 30, 0, 0},
  {"Drill Peck", TYPE_FLYING, MOVE_PHYSICAL, 80, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Brave Bird", TYPE_FLYING, MOVE_PHYSICAL, 120, 100, 15, 0, 0, 0, MF_CONTACT | MF_RECOIL},
  {"Hurricane", TYPE_FLYING, MOVE_SPECIAL, 110, 70, 10, 5, 30, 0, 0},
  {"Sky Attack", TYPE_FLYING, MOVE_PHYSICAL, 140, 90, 5, 6, 30, 0, MF_HIGHCRIT},
  {"Confusion", TYPE_PSYCHIC, MOVE_SPECIAL, 50, 100, 25, 5, 10, 0, 0},
  {"Calm Mind", TYPE_PSYCHIC, MOVE_STATUS, 0, 100, 20, 0, 0, 0, MF_SELFBUFF},
  {"Psybeam", TYPE_PSYCHIC, MOVE_SPECIAL, 65, 100, 20, 5, 10, 0, 0},
  {"Zen Headbutt", TYPE_PSYCHIC, MOVE_PHYSICAL, 80, 90, 15, 6, 20, 0, MF_CONTACT},
  {"Psychic", TYPE_PSYCHIC, MOVE_SPECIAL, 90, 100, 10, 0, 0, 0, MF_LOWERDEF},
  {"Psyshock", TYPE_PSYCHIC, MOVE_SPECIAL, 80, 100, 10, 0, 0, 0, 0},
  {"Future Sight", TYPE_PSYCHIC, MOVE_SPECIAL, 120, 100, 10, 0, 0, 0, 0},
  {"Psycho Cut", TYPE_PSYCHIC, MOVE_PHYSICAL, 70, 100, 20, 0, 0, 0, MF_HIGHCRIT},
  {"Extrasensory", TYPE_PSYCHIC, MOVE_SPECIAL, 80, 100, 20, 6, 10, 0, 0},
  {"Psystrike", TYPE_PSYCHIC, MOVE_SPECIAL, 100, 100, 10, 0, 0, 0, 0},
  {"String Shot", TYPE_BUG, MOVE_STATUS, 0, 95, 40, 0, 0, 0, MF_LOWERDEF},
  {"Bug Bite", TYPE_BUG, MOVE_PHYSICAL, 60, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Fury Cutter", TYPE_BUG, MOVE_PHYSICAL, 40, 95, 20, 0, 0, 0, MF_CONTACT | MF_MULTIHIT},
  {"Twineedle", TYPE_BUG, MOVE_PHYSICAL, 25, 100, 20, 3, 20, 0, MF_MULTIHIT},
  {"Signal Beam", TYPE_BUG, MOVE_SPECIAL, 75, 100, 15, 5, 10, 0, 0},
  {"X-Scissor", TYPE_BUG, MOVE_PHYSICAL, 80, 100, 15, 0, 0, 0, MF_CONTACT},
  {"Leech Life", TYPE_BUG, MOVE_PHYSICAL, 80, 100, 10, 0, 0, 0, MF_CONTACT | MF_HEAL},
  {"U-turn", TYPE_BUG, MOVE_PHYSICAL, 70, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Megahorn", TYPE_BUG, MOVE_PHYSICAL, 120, 85, 10, 0, 0, 0, MF_CONTACT},
  {"Bug Buzz", TYPE_BUG, MOVE_SPECIAL, 90, 100, 10, 0, 0, 0, MF_SOUND | MF_LOWERDEF},
  {"Rock Throw", TYPE_ROCK, MOVE_PHYSICAL, 50, 90, 15, 0, 0, 0, 0},
  {"Smack Down", TYPE_ROCK, MOVE_PHYSICAL, 50, 100, 15, 0, 0, 0, 0},
  {"Rock Tomb", TYPE_ROCK, MOVE_PHYSICAL, 60, 95, 15, 0, 0, 0, MF_LOWERDEF},
  {"Ancient Power", TYPE_ROCK, MOVE_SPECIAL, 60, 100, 5, 0, 0, 0, 0},
  {"Rock Slide", TYPE_ROCK, MOVE_PHYSICAL, 75, 90, 10, 6, 30, 0, 0},
  {"Power Gem", TYPE_ROCK, MOVE_SPECIAL, 80, 100, 20, 0, 0, 0, 0},
  {"Rock Blast", TYPE_ROCK, MOVE_PHYSICAL, 25, 90, 10, 0, 0, 0, MF_MULTIHIT},
  {"Stone Edge", TYPE_ROCK, MOVE_PHYSICAL, 100, 80, 5, 0, 0, 0, MF_HIGHCRIT},
  {"Head Smash", TYPE_ROCK, MOVE_PHYSICAL, 150, 80, 5, 0, 0, 0, MF_CONTACT | MF_RECOIL},
  {"Rock Wrecker", TYPE_ROCK, MOVE_PHYSICAL, 150, 90, 5, 0, 0, 0, 0},
  {"Lick", TYPE_GHOST, MOVE_PHYSICAL, 30, 100, 30, 2, 30, 0, MF_CONTACT},
  {"Astonish", TYPE_GHOST, MOVE_PHYSICAL, 30, 100, 15, 6, 30, 0, MF_CONTACT},
  {"Night Shade", TYPE_GHOST, MOVE_SPECIAL, 50, 100, 15, 0, 0, 0, 0},
  {"Shadow Sneak", TYPE_GHOST, MOVE_PHYSICAL, 40, 100, 30, 0, 0, 1, MF_CONTACT},
  {"Shadow Claw", TYPE_GHOST, MOVE_PHYSICAL, 70, 100, 15, 0, 0, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Hex", TYPE_GHOST, MOVE_SPECIAL, 65, 100, 10, 0, 0, 0, 0},
  {"Shadow Ball", TYPE_GHOST, MOVE_SPECIAL, 80, 100, 15, 0, 0, 0, MF_LOWERDEF},
  {"Phantom Force", TYPE_GHOST, MOVE_PHYSICAL, 90, 100, 10, 0, 0, 0, MF_CONTACT},
  {"Shadow Bone", TYPE_GHOST, MOVE_PHYSICAL, 85, 100, 10, 0, 0, 0, MF_LOWERDEF},
  {"Moongeist Beam", TYPE_GHOST, MOVE_SPECIAL, 100, 100, 5, 0, 0, 0, 0},
  {"Dragon Rage", TYPE_DRAGON, MOVE_SPECIAL, 60, 100, 10, 0, 0, 0, 0},
  {"Breaking Swipe", TYPE_DRAGON, MOVE_PHYSICAL, 60, 100, 15, 0, 0, 0, MF_CONTACT | MF_LOWERDEF},
  {"Twister", TYPE_DRAGON, MOVE_SPECIAL, 40, 100, 20, 6, 20, 0, 0},
  {"Dragon Breath", TYPE_DRAGON, MOVE_SPECIAL, 60, 100, 20, 2, 30, 0, 0},
  {"Dragon Claw", TYPE_DRAGON, MOVE_PHYSICAL, 80, 100, 15, 0, 0, 0, MF_CONTACT},
  {"Dragon Pulse", TYPE_DRAGON, MOVE_SPECIAL, 85, 100, 10, 0, 0, 0, 0},
  {"Dragon Tail", TYPE_DRAGON, MOVE_PHYSICAL, 60, 90, 10, 0, 0, 0, MF_CONTACT},
  {"Dragon Rush", TYPE_DRAGON, MOVE_PHYSICAL, 100, 75, 10, 6, 20, 0, MF_CONTACT},
  {"Outrage", TYPE_DRAGON, MOVE_PHYSICAL, 120, 100, 10, 5, 100, 0, MF_CONTACT},
  {"Draco Meteor", TYPE_DRAGON, MOVE_SPECIAL, 130, 90, 5, 0, 0, 0, 0},
  {"Bite", TYPE_DARK, MOVE_PHYSICAL, 60, 100, 25, 6, 30, 0, MF_CONTACT},
  {"Pursuit", TYPE_DARK, MOVE_PHYSICAL, 40, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Snarl", TYPE_DARK, MOVE_SPECIAL, 55, 95, 15, 0, 0, 0, MF_SOUND | MF_LOWERDEF},
  {"Feint Attack", TYPE_DARK, MOVE_PHYSICAL, 60, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Crunch", TYPE_DARK, MOVE_PHYSICAL, 80, 100, 15, 0, 0, 0, MF_CONTACT | MF_LOWERDEF},
  {"Dark Pulse", TYPE_DARK, MOVE_SPECIAL, 80, 100, 15, 6, 20, 0, 0},
  {"Knock Off", TYPE_DARK, MOVE_PHYSICAL, 65, 100, 20, 0, 0, 0, MF_CONTACT},
  {"Night Slash", TYPE_DARK, MOVE_PHYSICAL, 70, 100, 15, 0, 0, 0, MF_CONTACT | MF_HIGHCRIT},
  {"Foul Play", TYPE_DARK, MOVE_PHYSICAL, 95, 100, 15, 0, 0, 0, MF_CONTACT},
  {"Hyperspace Fury", TYPE_DARK, MOVE_PHYSICAL, 100, 100, 5, 0, 0, 0, MF_LOWERDEF},
  {"Metal Claw", TYPE_STEEL, MOVE_PHYSICAL, 50, 95, 35, 0, 0, 0, MF_CONTACT | MF_SELFBUFF},
  {"Bullet Punch", TYPE_STEEL, MOVE_PHYSICAL, 40, 100, 30, 0, 0, 1, MF_CONTACT},
  {"Iron Defense", TYPE_STEEL, MOVE_STATUS, 0, 100, 15, 0, 0, 0, MF_SELFBUFF},
  {"Flash Cannon", TYPE_STEEL, MOVE_SPECIAL, 80, 100, 10, 0, 0, 0, MF_LOWERDEF},
  {"Iron Head", TYPE_STEEL, MOVE_PHYSICAL, 80, 100, 15, 6, 30, 0, MF_CONTACT},
  {"Steel Wing", TYPE_STEEL, MOVE_PHYSICAL, 70, 90, 25, 0, 0, 0, MF_CONTACT | MF_SELFBUFF},
  {"Gyro Ball", TYPE_STEEL, MOVE_PHYSICAL, 80, 100, 5, 0, 0, 0, MF_CONTACT},
  {"Meteor Mash", TYPE_STEEL, MOVE_PHYSICAL, 90, 90, 10, 0, 0, 0, MF_CONTACT | MF_SELFBUFF},
  {"Heavy Slam", TYPE_STEEL, MOVE_PHYSICAL, 100, 100, 10, 0, 0, 0, MF_CONTACT},
  {"Iron Tail", TYPE_STEEL, MOVE_PHYSICAL, 100, 75, 15, 0, 0, 0, MF_CONTACT | MF_LOWERDEF},
  {"Fairy Wind", TYPE_FAIRY, MOVE_SPECIAL, 40, 100, 30, 0, 0, 0, 0},
  {"Baby-Doll Eyes", TYPE_FAIRY, MOVE_STATUS, 0, 100, 30, 0, 0, 1, MF_LOWERDEF},
  {"Disarming Voice", TYPE_FAIRY, MOVE_SPECIAL, 40, 100, 15, 0, 0, 0, MF_SOUND},
  {"Draining Kiss", TYPE_FAIRY, MOVE_SPECIAL, 50, 100, 10, 0, 0, 0, MF_CONTACT | MF_HEAL},
  {"Dazzling Gleam", TYPE_FAIRY, MOVE_SPECIAL, 80, 100, 10, 0, 0, 0, 0},
  {"Play Rough", TYPE_FAIRY, MOVE_PHYSICAL, 90, 90, 10, 0, 0, 0, MF_CONTACT | MF_LOWERDEF},
  {"Moonblast", TYPE_FAIRY, MOVE_SPECIAL, 95, 100, 15, 0, 0, 0, MF_LOWERDEF},
  {"Misty Explosion", TYPE_FAIRY, MOVE_SPECIAL, 100, 100, 5, 0, 0, 0, 0},
  {"Spirit Break", TYPE_FAIRY, MOVE_PHYSICAL, 75, 100, 15, 0, 0, 0, MF_CONTACT | MF_LOWERDEF},
  {"Fleur Cannon", TYPE_FAIRY, MOVE_SPECIAL, 130, 90, 5, 0, 0, 0, 0},
};

const MoveDef &moveDef(uint8_t id) { return MOVES[id < MOVE_COUNT ? id : 0]; }
uint8_t moveId(uint8_t type, uint8_t index) {
  if (type < 1 || type > 18 || index >= MOVES_PER_TYPE) return MOVE_NONE;
  return (uint8_t)((type - 1) * MOVES_PER_TYPE + index);
}

uint16_t stabPct(uint8_t mt, uint8_t t1, uint8_t t2, uint8_t tera) {
  bool same = mt == t1 || (t2 != TYPE_NONE && mt == t2) || (tera != TYPE_NONE && mt == tera);
  return same ? 150 : 100;
}

uint16_t stagePct(int8_t s) {
  if (s > 6) s = 6;
  if (s < -6) s = -6;
  return s >= 0 ? (uint16_t)(100 * (2 + s) / 2) : (uint16_t)(100 * 2 / (2 - s));
}

uint16_t moveDamage(uint16_t atk, uint16_t def, uint8_t level, const MoveDef &m,
                    uint16_t typePct, uint16_t stab, bool crit, uint8_t rollPct) {
  if (m.category == MOVE_STATUS || m.power == 0) return 0;
  if (def == 0) def = 1;
  uint32_t base = ((2u * level / 5u + 2u) * m.power * atk / def) / 50u + 2u;
  base = base * stab / 100u;
  base = base * typePct / 100u;
  if (crit) base = base * 3u / 2u;
  if (rollPct < 85) rollPct = 85;
  if (rollPct > 100) rollPct = 100;
  base = base * rollPct / 100u;
  if (typePct > 0 && base == 0) base = 1;
  return base > 9999 ? 9999 : (uint16_t)base;
}

bool critHit(const MoveDef &m, uint8_t roll) {
  // roll 0..999 -> 6.25% = <62.5, 12.5% = <125
  return roll < ((m.flags & MF_HIGHCRIT) ? 125 : 62);
}
uint8_t multiHitCount(uint8_t roll) { return 2 + roll % 4; }

uint16_t statusDot(uint8_t s, uint16_t maxHp) {
  uint16_t pct = s == ST_BURN ? 6 : s == ST_POISON ? 8 : 0;
  uint16_t d = (uint16_t)((uint32_t)maxHp * pct / 100);
  return (pct && d == 0) ? 1 : d;
}
uint16_t statusAtkPct(uint8_t s) { return s == ST_BURN ? 75 : 100; }
uint16_t statusSpePct(uint8_t s) { return s == ST_PARALYZE ? 50 : 100; }

void inflictStatus(StatusState &s, uint8_t status, uint8_t r) {
  if (s.status != ST_NONE || status == ST_NONE) return;
  s.status = status;
  switch (status) {
    case ST_SLEEP: s.turns = 1 + r % 3; break;        // 1..3
    case ST_CONFUSE: s.turns = 2 + r % 4; break;      // 2..5
    case ST_FLINCH: s.turns = 1; break;
    default: s.turns = 0; break;                       // until battle ends
  }
}

bool statusBlocksAction(StatusState &s, uint8_t roll, bool &selfHit) {
  selfHit = false;
  switch (s.status) {
    case ST_PARALYZE: return roll < 25;
    case ST_SLEEP:
      if (s.turns > 0) s.turns--;
      if (s.turns == 0) { s.status = ST_NONE; return false; }   // wakes up and acts
      return true;
    case ST_FLINCH: s = StatusState(); return true;
    case ST_CONFUSE:
      if (s.turns > 0) s.turns--;
      if (s.turns == 0) s.status = ST_NONE;
      if (roll < 33) { selfHit = true; return true; }
      return false;
    default: return false;
  }
}
void statusOnDamage(StatusState &s) { if (s.status == ST_SLEEP) s = StatusState(); }

// --- learnsets ---
void movesetInit(MoveSet &ms) { for (auto &s : ms.slot) s = {MOVE_NONE, 0}; }

static uint8_t basicMove(uint8_t type) { return moveId(type, 0); }

uint8_t hatchMoves(MoveSet &ms, uint8_t t1, uint8_t t2, uint8_t level) {
  movesetInit(ms);
  uint8_t n = 0;
  if (t1 != TYPE_NONE) { movesetAdd(ms, basicMove(t1)); n++; }
  if (t2 != TYPE_NONE && t2 != t1) { movesetAdd(ms, basicMove(t2)); n++; }
  if (level >= 5) { if (movesetAdd(ms, moveId(TYPE_NORMAL, 0)) >= 0) n++; }  // Normal utility: Tackle
  if (n < 2) { if (movesetAdd(ms, moveId(TYPE_NORMAL, 0)) >= 0 || movesetAdd(ms, moveId(TYPE_NORMAL, 1)) >= 0) n++; }
  return n;
}

static const uint8_t COVERAGE[] = {TYPE_ICE, TYPE_GROUND, TYPE_FIGHTING, TYPE_ROCK, TYPE_DARK,
                                   TYPE_ELECTRIC, TYPE_STEEL, TYPE_PSYCHIC};

uint8_t learnOfferFor(uint8_t level, uint8_t t1, uint8_t t2, uint8_t roll) {
  if (level == 0 || level % 5 != 0) return MOVE_NONE;
  uint8_t pool[3] = {t1, t2, TYPE_NORMAL};
  uint8_t type = pool[roll % 3];
  if (type == TYPE_NONE) type = t1;
  if (level >= 30 && roll % 4 == 3) type = COVERAGE[(roll / 4) % (sizeof(COVERAGE))];
  uint16_t maxP = 50 + (uint16_t)level * 12 / 10, minP = maxP * 4 / 10;
  uint8_t cand[MOVES_PER_TYPE], c = 0, best = MOVE_NONE;
  for (uint8_t i = 0; i < MOVES_PER_TYPE; i++) {
    uint8_t id = moveId(type, i);
    const MoveDef &m = moveDef(id);
    if (m.power == 0) continue;
    if (m.power <= maxP && (best == MOVE_NONE || m.power > moveDef(best).power)) best = id;
    if (m.power <= maxP && m.power > minP) cand[c++] = id;
  }
  return c ? cand[(roll / 3) % c] : best;
}

int8_t movesetAdd(MoveSet &ms, uint8_t id) {
  if (id >= MOVE_COUNT) return -1;
  for (auto &s : ms.slot) if (s.id == id) return -1;
  for (uint8_t i = 0; i < MOVESET_SIZE; i++)
    if (ms.slot[i].id == MOVE_NONE) { ms.slot[i] = {id, moveDef(id).pp}; return (int8_t)i; }
  return -2;
}
bool movesetReplace(MoveSet &ms, uint8_t i, uint8_t id) {
  if (i >= MOVESET_SIZE || id >= MOVE_COUNT) return false;
  for (auto &s : ms.slot) if (s.id == id) return false;
  ms.slot[i] = {id, moveDef(id).pp};
  return true;
}
bool movesetUse(MoveSet &ms, uint8_t i) {
  if (i >= MOVESET_SIZE || ms.slot[i].id == MOVE_NONE || ms.slot[i].pp == 0) return false;
  ms.slot[i].pp--;
  return true;
}
void movesetRestoreAfterWin(MoveSet &ms) {
  for (auto &s : ms.slot) {
    if (s.id == MOVE_NONE) continue;
    uint8_t mx = moveDef(s.id).pp, add = (mx + 1) / 2;
    s.pp = (uint8_t)(s.pp + add > mx ? mx : s.pp + add);
  }
}
void movesetRestoreFull(MoveSet &ms) {
  for (auto &s : ms.slot) if (s.id != MOVE_NONE) s.pp = moveDef(s.id).pp;
}

uint8_t aiPickMove(const MoveSet &ms, uint8_t d1, uint8_t d2, uint8_t smart, uint8_t roll) {
  int best = -1, bestScore = -1, usable = 0, firstUsable = -1;
  for (uint8_t i = 0; i < MOVESET_SIZE; i++) {
    if (ms.slot[i].id == MOVE_NONE || ms.slot[i].pp == 0) continue;
    usable++; if (firstUsable < 0) firstUsable = i;
    const MoveDef &m = moveDef(ms.slot[i].id);
    int score = (int)m.power * battleTypeEffectPct(m.type, d1, d2) / 100;
    if (score > bestScore) { bestScore = score; best = i; }
  }
  if (!usable) return MOVE_NONE;
  if (roll < smart) return (uint8_t)best;        // smart pick with probability smart%
  // otherwise random among usable
  int k = roll % usable;
  for (uint8_t i = 0; i < MOVESET_SIZE; i++)
    if (ms.slot[i].id != MOVE_NONE && ms.slot[i].pp > 0 && k-- == 0) return i;
  return (uint8_t)firstUsable;
}
