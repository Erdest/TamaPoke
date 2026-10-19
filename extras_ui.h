// On-device screens and trainer fights for the Gen 9 feature set.
// Included once by TamaPoke.ino (single translation unit) and compiled only
// with -DTAMAPOKE_EXTRAS. All text is ASCII (the display font has no accents).
#pragma once
#ifdef TAMAPOKE_EXTRAS

#include "extras.h"

// ---- strings (EN/ES/FR/DE/IT; PT falls back to EN) ----
enum XStr : uint8_t {
  XS_TRAINER, XS_BADGES, XS_E4, XS_CHAMP, XS_FIGHT, XS_REMATCH, XS_GARDEN, XS_EMPTY, XS_GROWING,
  XS_READY, XS_WILTED, XS_SEEDS, XS_BAG, XS_BALLS, XS_ACH, XS_ACC, XS_NEXT, XS_WON, XS_LOST,
  XS_NOSEEDS, XS_WATERED, XS_TOOSOON, XS_HARVEST, XS_FED, XS_NOBERRY, XS_REGION_DONE, XS_NOTNOW,
  XS_NEWACH, XS_MOVES, XS_EQUIP, XS_UNEQUIP, XS_NOTOWNED, XS_MULCHED, XS_LEGEGG, XS_NEXTFIGHT,
  XS_COUNT
};
static const char *const X_TXT[XS_COUNT][5] = {
  // ES, EN, FR, DE, IT
  {"ENTRENADOR", "TRAINER", "DRESSEUR", "TRAINER", "ALLENATORE"},
  {"Medallas", "Badges", "Badges", "Orden", "Medaglie"},
  {"Alto Mando", "Elite Four", "Conseil 4", "Top Vier", "Elite 4"},
  {"Campeon", "Champion", "Maitre", "Champ", "Campione"},
  {"RETAR", "CHALLENGE", "DEFI", "KAMPF", "SFIDA"},
  {"Revancha", "Rematch", "Revanche", "Rematch", "Rivincita"},
  {"HUERTO", "GARDEN", "JARDIN", "GARTEN", "GIARDINO"},
  {"vacio", "empty", "vide", "leer", "vuoto"},
  {"crece", "growing", "pousse", "waechst", "cresce"},
  {"LISTA", "READY", "PRETE", "REIF", "PRONTA"},
  {"marchita", "wilted", "fanee", "welk", "appassita"},
  {"Semillas", "Seeds", "Graines", "Samen", "Semi"},
  {"MOCHILA", "BAG", "SAC", "BEUTEL", "ZAINO"},
  {"Pokeballs", "Poke Balls", "Poke Balls", "Pokebaelle", "Poke Ball"},
  {"LOGROS", "ACHIEVEMENTS", "SUCCES", "ERFOLGE", "TRAGUARDI"},
  {"ACCESORIOS", "ACCESSORIES", "ACCESSOIRES", "ZUBEHOER", "ACCESSORI"},
  {"Siguiente", "Next", "Suivant", "Naechster", "Prossimo"},
  {"Victoria!", "Victory!", "Victoire!", "Sieg!", "Vittoria!"},
  {"Derrota", "Defeat", "Defaite", "Niederlage", "Sconfitta"},
  {"Sin semillas", "No seeds", "Pas de graines", "Keine Samen", "Nessun seme"},
  {"Regada", "Watered", "Arrosee", "Gegossen", "Annaffiata"},
  {"Aun no", "Too soon", "Trop tot", "Zu frueh", "Troppo presto"},
  {"Cosechada", "Harvested", "Recoltee", "Geerntet", "Raccolta"},
  {"Comio", "Ate it", "A mange", "Gefressen", "Mangiata"},
  {"Sin bayas", "No berries", "Pas de baies", "Keine Beeren", "Nessuna bacca"},
  {"Region completa", "Region clear", "Region finie", "Region fertig", "Regione finita"},
  {"Ahora no", "Not now", "Pas Maint.", "Nicht jetzt", "Non ora"},
  {"Logro", "Unlocked", "Debloque", "Erreicht", "Sbloccato"},
  {"Ataques", "Moves", "Attaques", "Attacken", "Mosse"},
  {"Equipar", "Equip", "Equiper", "Anlegen", "Equipa"},
  {"Quitar", "Remove", "Retirer", "Ablegen", "Rimuovi"},
  {"No lo tienes", "Not owned", "Non obtenu", "Fehlt", "Non posseduto"},
  {"Abono", "Mulched", "Paillis", "Duenger", "Pacciamata"},
  {"Huevo legendario!", "Legendary egg!", "Oeuf legendaire!", "Legendaeres Ei!", "Uovo leggendario!"},
  {"Siguiente combate", "Next fight", "Prochain duel", "Naechster Kampf", "Prossimo scontro"},
};
static const char *xt(XStr id) {
  uint8_t l = (gLang < LANG_COUNT) ? (uint8_t)gLang : (uint8_t)LANG_DEFAULT;
  if (l == LANG_PT) l = LANG_EN;
  return X_TXT[id][l];
}

// ---- state ----
static uint8_t xRegion = 0, xSeed = 0, xAchPage = 0, xAccPage = 0;
static TrainerRun xRun = {};
static uint16_t xCarryHp = 0;
static char xMsg[32] = "";
static char xTrainerMsg[32] = "";
static int8_t xLastAch = -1;
static uint32_t xNextSync = 0;
static uint16_t xPool[DEX_COUNT];
static const uint8_t X_PAGE_FIRST = 9;
static const uint8_t X_ROWS = 5;

static void xSetMsg(const char *s) { strncpy(xMsg, s, sizeof(xMsg) - 1); xMsg[sizeof(xMsg) - 1] = 0; }

static uint16_t xBuildPool(uint8_t type, bool allowLegendary) {
  uint16_t n = 0;
  for (int i = 1; i <= DEX_COUNT; i++) {
    const DexEntry &d = DEX_TBL[i];
    if (!allowLegendary && d.rarity == R_LEGENDARIO) continue;
    if (type && d.type1 != type && d.type2 != type) continue;
    xPool[n++] = (uint16_t)i;
  }
  if (n == 0) for (int i = 1; i <= DEX_COUNT; i++) xPool[n++] = (uint16_t)i;
  return n;
}

// next unbeaten target of a region
struct XTarget { bool any; TrainerKind kind; uint8_t index; const char *name; uint8_t type; };
static XTarget xNextTarget(uint8_t r) {
  XTarget t = {false, TK_GYM, 0, "", 0};
  const GymProgress &g = pet.extras.gyms;
  for (uint8_t i = 0; i < GYMS_PER_REGION; i++)
    if (!(g.region[r] & BIT_GYM(i))) {
      t = {true, TK_GYM, i, gymLeader(r, i).name, gymLeader(r, i).type};
      return t;
    }
  int8_t e = nextE4(g, r);
  if (e >= 0) { t = {true, TK_E4, (uint8_t)e, eliteFour(r, e).name, eliteFour(r, e).type}; return t; }
  if (!(g.region[r] & BIT_CHAMP)) { t = {true, TK_CHAMPION, 0, champion(r).name, champion(r).type}; return t; }
  return t;
}

// ---- trainer fights ----
static void xStartMember() {
  const TeamMon &m = xRun.team[xRun.pos];
  startBattleWith(m.dex, m.level, 0);
  if (!battleOpen) { xRun.active = 0; xSetMsg(xt(XS_NOTNOW)); return; }
  if (xCarryHp > 0) {
    battleRun.playerHp = xCarryHp > battleRun.playerMaxHp ? battleRun.playerMaxHp : xCarryHp;
    battleDirty = true;
  }
  xTrainerMsg[0] = 0;
  battleCatchOffered = false;
}

static void xStartFight(TrainerKind kind, uint8_t idx) {
  const XTarget tg = xNextTarget(xRegion);
  uint8_t type = tg.any ? tg.type : 0;
  uint16_t n = xBuildPool(type, kind == TK_CHAMPION);
  ChallengeResult c = trainerBegin(pet.extras, xRun, kind, xRegion, idx, pet.level(),
                                   (uint32_t)esp_random(), xPool, n);
  if (c != CH_OK) { xSetMsg(challengeText(c)); cardDirty = true; sfxPlay(SFX_DENY); return; }
  xCarryHp = 0;
  cardOpen = false;
  markUiDirty();
  lockTouchBrief();
  xStartMember();
  if (!xRun.active) cardDirty = true;
}

// called from finishBattle() while a trainer run is active
static void xOnTrainerMemberDone(bool won) {
  bool closeWin = battleRun.playerHp <= battleRun.playerMaxHp / 3;
  xTrainerMsg[0] = 0;
  if (won) {
    battleReward = pet.applyBattleWin(battleDex, closeWin);
    uint16_t heal = e4HealAmount(battleRun.playerMaxHp);
    xCarryHp = (uint16_t)(battleRun.playerHp + heal);
    TrainerOutcome o = trainerMemberResult(pet.extras, xRun, pet.balls, true,
                                           (uint8_t)random(100), (uint32_t)esp_random());
    if (o.finished) {
      pet.joy = (uint8_t)min(100, (int)pet.joy + o.rewards.joy);
      pet.gainBond(o.rewards.bond);
      pet.syncAchievements();
      pet.commit();
      snprintf(xTrainerMsg, sizeof(xTrainerMsg), "%s", xt(XS_WON));
      if (o.rewards.legendaryEgg) snprintf(xTrainerMsg, sizeof(xTrainerMsg), "%s", xt(XS_LEGEGG));
      sfxPlay(SFX_BATTLE_WIN);
    } else {
      snprintf(xTrainerMsg, sizeof(xTrainerMsg), "%s %u/%u", xt(XS_NEXT), xRun.pos + 1, xRun.len);
      sfxPlay(SFX_BATTLE_WIN);
    }
  } else {
    battleReward = {};
    pet.applyBattleLoss();
    trainerMemberResult(pet.extras, xRun, pet.balls, false, 0, 0);
    snprintf(xTrainerMsg, sizeof(xTrainerMsg), "%s", xt(XS_LOST));
    sfxPlay(SFX_BATTLE_LOSS);
  }
  battleCatchOffered = false;
  battleRespectCatch = false;
}

// OK pressed on a resolved trainer battle with more members to go
static bool xContinueRun() {
  if (!xRun.active) return false;
  xStartMember();
  return true;
}
static void xAbortRun() { xRun.active = 0; xTrainerMsg[0] = 0; xCarryHp = 0; }

// ---- shared drawing ----
static void xHeader(const char *title) {
  gfx->setTextColor(UI_INK);
  gfx->setTextSize(3);
  gfx->setCursor(CX - (int)strlen(title) * 9, 42);
  gfx->print(title);
}
static void xRow(int y, uint16_t border, const char *left, const char *right, uint16_t rightCol) {
  gfx->fillRoundRect(58, y, 350, 34, 9, UI_WHITE);
  gfx->drawRoundRect(58, y, 350, 34, 9, border);
  gfx->setTextSize(2);
  gfx->setTextColor(UI_INK);
  gfx->setCursor(70, y + 10);
  gfx->print(left);
  if (right && right[0]) {
    gfx->setTextColor(rightCol);
    gfx->setCursor(396 - (int)strlen(right) * 12, y + 10);
    gfx->print(right);
  }
}
static void xPager(int pages, int page) {
  gfx->fillRoundRect(76, 348, 94, 38, 11, page > 0 ? UI_TRACK : C565(0xe4, 0xe8, 0xee));
  gfx->fillRoundRect(296, 348, 94, 38, 11, page + 1 < pages ? UI_TRACK : C565(0xe4, 0xe8, 0xee));
  gfx->setTextColor(UI_BG_DAY);
  gfx->setTextSize(3);
  gfx->setCursor(111, 357);
  gfx->print("<");
  gfx->setCursor(331, 357);
  gfx->print(">");
  char pg[12];
  snprintf(pg, sizeof(pg), "%d/%d", page + 1, pages);
  gfx->setTextColor(UI_TRACK);
  gfx->setTextSize(2);
  gfx->setCursor(CX - (int)strlen(pg) * 6, 360);
  gfx->print(pg);
}
static void xDrawMsg() {
  if (!xMsg[0]) return;
  gfx->setTextColor(UI_BAR_WARN);
  gfx->setTextSize(2);
  gfx->setCursor(CX - (int)strlen(xMsg) * 6, 326);
  gfx->print(xMsg);
}
static void xFmtTime(char *buf, size_t n, uint32_t s) {
  if (s >= 3600) snprintf(buf, n, "%luh%02lum", (unsigned long)(s / 3600), (unsigned long)((s % 3600) / 60));
  else snprintf(buf, n, "%lum", (unsigned long)((s + 59) / 60));
}

// ---- page 9: trainer / gyms ----
static void renderCardTrainer() {
  xHeader(xt(XS_TRAINER));
  const GymProgress &g = pet.extras.gyms;
  char buf[40];
  gfx->setTextSize(3);
  gfx->setTextColor(UI_INK);
  snprintf(buf, sizeof(buf), "< %s >", regionName(xRegion));
  gfx->setCursor(CX - (int)strlen(buf) * 9, 76);
  gfx->print(buf);

  gfx->setTextSize(2);
  gfx->setTextColor(UI_TRACK);
  snprintf(buf, sizeof(buf), "%s %u/8  E4 %u/4  %s", xt(XS_BADGES), badgesIn(g, xRegion), e4BeatenIn(g, xRegion),
           (g.region[xRegion] & BIT_CHAMP) ? "OK" : "-");
  gfx->setCursor(CX - (int)strlen(buf) * 6, 116);
  gfx->print(buf);

  XTarget t = xNextTarget(xRegion);
  if (!t.any) {
    gfx->setTextColor(UI_BAR_OK);
    gfx->setCursor(CX - (int)strlen(xt(XS_REGION_DONE)) * 6, 170);
    gfx->print(xt(XS_REGION_DONE));
    RematchTier tier = rematchTier(g, xRegion);
    snprintf(buf, sizeof(buf), "%s +%u", xt(XS_REMATCH), tier * 10);
    gfx->fillRoundRect(82, 220, 302, 52, 14, UI_BAR_WARN);
    gfx->setTextColor(UI_BG_DAY);
    gfx->setCursor(CX - (int)strlen(buf) * 6, 238);
    gfx->print(buf);
  } else {
    const char *kind = t.kind == TK_GYM ? "Gym" : (t.kind == TK_E4 ? xt(XS_E4) : xt(XS_CHAMP));
    snprintf(buf, sizeof(buf), "%s %u", kind, t.kind == TK_CHAMPION ? 1 : t.index + 1);
    gfx->setTextColor(UI_TRACK);
    gfx->setCursor(CX - (int)strlen(buf) * 6, 150);
    gfx->print(buf);
    gfx->setTextSize(3);
    gfx->setTextColor(UI_INK);
    gfx->setCursor(CX - (int)strlen(t.name) * 9, 178);
    gfx->print(t.name);
    gfx->setTextSize(2);
    if (t.type) {
      gfx->setTextColor(battleTypeColor(t.type));
      char tn[20];
      snprintf(tn, sizeof(tn), "Type %u", t.type);
      gfx->setCursor(CX - (int)strlen(tn) * 6, 214);
      gfx->print(tn);
    }
    ChallengeResult c = t.kind == TK_GYM ? canChallengeGym(g, xRegion, t.index, pet.level())
                      : (t.kind == TK_E4 ? canChallengeE4(g, xRegion) : canChallengeChampion(g, xRegion));
    bool ok = c == CH_OK;
    if (t.kind == TK_GYM) snprintf(buf, sizeof(buf), "Lv>=%u  x%u", gymMinPlayerLevel(t.index), gymTeamSize(t.index));
    else snprintf(buf, sizeof(buf), "%s", challengeText(c));
    gfx->setTextColor(ok ? UI_BAR_OK : UI_BAR_BAD);
    gfx->setCursor(CX - (int)strlen(buf) * 6, 244);
    gfx->print(buf);
    if (!ok && t.kind == TK_GYM) {
      gfx->setTextColor(UI_BAR_BAD);
      gfx->setCursor(CX - (int)strlen(challengeText(c)) * 6, 266);
      gfx->print(challengeText(c));
    }
    gfx->fillRoundRect(82, 286, 302, 52, 14, ok ? UI_BAR_OK : UI_TRACK);
    gfx->setTextColor(UI_BG_DAY);
    gfx->setTextSize(3);
    gfx->setCursor(CX - (int)strlen(xt(XS_FIGHT)) * 9, 301);
    gfx->print(xt(XS_FIGHT));
  }
  xDrawMsg();
}
static void xTrainerTap(int x, int y) {
  GymProgress &g = pet.extras.gyms;
  if (y >= 60 && y <= 98) {
    if (x < 150) xRegion = (uint8_t)((xRegion + REGION_COUNT - 1) % REGION_COUNT);
    else if (x > 316) xRegion = (uint8_t)((xRegion + 1) % REGION_COUNT);
    else return;
    xMsg[0] = 0; cardDirty = true; sfxPlay(SFX_TAP);
    return;
  }
  XTarget t = xNextTarget(xRegion);
  if (!t.any) {
    if (y >= 220 && y <= 272) {
      RematchTier nt = (RematchTier)((rematchTier(g, xRegion) + 1) % 3);
      setRematchTier(g, xRegion, nt);
      pet.commit();
      cardDirty = true; sfxPlay(SFX_TAP);
    }
    return;
  }
  if (y >= 286 && y <= 338 && x >= 82 && x <= 384) xStartFight(t.kind, t.index);
}

// ---- page 10: berry garden ----
static void renderCardGarden() {
  xHeader(xt(XS_GARDEN));
  Garden &gd = pet.extras.garden;
  uint32_t now = expeditionNowEpoch();
  char buf[40];
  // seed selector
  snprintf(buf, sizeof(buf), "< %s x%u >", berryDef(xSeed).name, gd.seeds[xSeed]);
  gfx->setTextSize(2);
  gfx->setTextColor(gd.seeds[xSeed] ? UI_INK : UI_TRACK);
  gfx->setCursor(CX - (int)strlen(buf) * 6, 70);
  gfx->print(buf);
  for (uint8_t i = 0; i < GARDEN_SLOTS; i++) {
    int y = 94 + i * 38;
    SlotState st = gardenState(gd, i, now);
    char right[16] = "";
    uint16_t col = UI_TRACK;
    const char *name = "-";
    if (st == SLOT_GROWING) { xFmtTime(right, sizeof(right), gardenSecondsLeft(gd, i, now)); col = UI_BAR_WARN; }
    else if (st == SLOT_READY) { snprintf(right, sizeof(right), "%s", xt(XS_READY)); col = UI_BAR_OK; }
    else if (st == SLOT_WILTED) { snprintf(right, sizeof(right), "%s", xt(XS_WILTED)); col = UI_BAR_BAD; }
    else snprintf(right, sizeof(right), "%s", xt(XS_EMPTY));
    if (st != SLOT_EMPTY) name = berryDef(gd.slot[i].type).name;
    char left[24];
    snprintf(left, sizeof(left), "%u %s", i + 1, name);
    xRow(y, st == SLOT_READY ? UI_BAR_OK : UI_TRACK, left, right, col);
  }
  // feed buttons for the three basic berries
  const char *lab[3] = {"R", "B", "G"};
  for (uint8_t b = 0; b < 3; b++) {
    int x = 76 + b * 104;
    gfx->fillRoundRect(x, 326, 94, 36, 11, gd.berries[b] ? UI_BAR_OK : UI_TRACK);
    gfx->setTextColor(UI_BG_DAY);
    gfx->setTextSize(2);
    snprintf(buf, sizeof(buf), "%s x%u", lab[b], gd.berries[b]);
    gfx->setCursor(x + (94 - (int)strlen(buf) * 12) / 2, 336);
    gfx->print(buf);
  }
  if (xMsg[0]) {
    gfx->setTextColor(UI_BAR_WARN);
    gfx->setTextSize(2);
    gfx->setCursor(CX - (int)strlen(xMsg) * 6, 372);
    gfx->print(xMsg);
  }
}
static void xGardenTap(int x, int y) {
  Garden &gd = pet.extras.garden;
  uint32_t now = expeditionNowEpoch();
  if (y >= 62 && y <= 90) {
    xSeed = (uint8_t)((xSeed + (x < CX ? BERRY_COUNT - 1 : 1)) % BERRY_COUNT);
    xMsg[0] = 0; cardDirty = true; sfxPlay(SFX_TAP);
    return;
  }
  if (y >= 326 && y <= 362) {
    for (uint8_t b = 0; b < 3; b++) {
      int bx = 76 + b * 104;
      if (x >= bx && x <= bx + 94) {
        if (gardenTakeBerry(gd, b)) { pet.feedBerry(b); xSetMsg(xt(XS_FED)); sfxPlay(SFX_EAT); }
        else { xSetMsg(xt(XS_NOBERRY)); sfxPlay(SFX_DENY); }
        pet.commit();
        cardDirty = true;
        return;
      }
    }
    return;
  }
  int row = (y - 94) / 38;
  if (row < 0 || row >= GARDEN_SLOTS || y > 94 + row * 38 + 34) return;
  SlotState st = gardenState(gd, (uint8_t)row, now);
  if (st == SLOT_EMPTY) {
    if (gardenPlant(gd, (uint8_t)row, xSeed, now)) { xMsg[0] = 0; sfxPlay(SFX_MENU); }
    else { xSetMsg(xt(XS_NOSEEDS)); sfxPlay(SFX_DENY); }
  } else if (st == SLOT_GROWING) {
    if (x > 330) { if (gardenMulch(gd, (uint8_t)row, now)) { xSetMsg(xt(XS_MULCHED)); sfxPlay(SFX_MENU); } else { xSetMsg(xt(XS_TOOSOON)); sfxPlay(SFX_DENY); } }
    else if (gardenWater(gd, (uint8_t)row, now)) { xSetMsg(xt(XS_WATERED)); sfxPlay(SFX_MENU); }
    else { xSetMsg(xt(XS_TOOSOON)); sfxPlay(SFX_DENY); }
  } else {
    HarvestResult r = extrasHarvest(pet.extras, (uint8_t)row, now, (uint8_t)random(100));
    if (r.ok) { xSetMsg(xt(XS_HARVEST)); sfxPlay(SFX_MEDAL); }
  }
  pet.syncAchievements();
  pet.commit();
  cardDirty = true;
}

// ---- page 11: bag (balls + moves) ----
static void renderCardBag() {
  xHeader(xt(XS_BAG));
  static const char *const BN[BALL_COUNT] = {"Poke", "Great", "Ultra", "Net", "Dusk", "Timer", "Quick", "Luxury", "Premier", "Master"};
  gfx->setTextSize(2);
  gfx->setTextColor(UI_TRACK);
  gfx->setCursor(72, 72);
  gfx->print(xt(XS_BALLS));
  for (uint8_t i = 0; i < BALL_COUNT; i++) {
    int col = i / 5, row = i % 5;
    int x = 62 + col * 178, y = 96 + row * 32;
    char b[24];
    if (i == BALL_POKE) snprintf(b, sizeof(b), "%s  inf", BN[i]);
    else snprintf(b, sizeof(b), "%s x%u", BN[i], pet.balls.count[i]);
    gfx->setTextColor(pet.balls.count[i] || i == BALL_POKE ? UI_INK : UI_TRACK);
    gfx->setCursor(x, y);
    gfx->print(b);
  }
#ifdef TAMAPOKE_MOVES
  gfx->setTextColor(UI_TRACK);
  gfx->setCursor(72, 264);
  gfx->print(xt(XS_MOVES));
  for (uint8_t i = 0; i < MOVESET_SIZE; i++) {
    int col = i / 2, row = i % 2;
    int x = 62 + col * 178, y = 288 + row * 28;
    if (pet.moves.slot[i].id == MOVE_NONE) continue;
    const MoveDef &m = moveDef(pet.moves.slot[i].id);
    char b[24];
    snprintf(b, sizeof(b), "%.9s %u", m.name, pet.moves.slot[i].pp);
    gfx->setTextColor(battleTypeColor(m.type));
    gfx->setCursor(x, y);
    gfx->print(b);
  }
#endif
}

// ---- page 12: achievements ----
static void renderCardAch() {
  xHeader(xt(XS_ACH));
  const AchState &a = pet.extras.ach;
  char buf[32];
  snprintf(buf, sizeof(buf), "%u/%u", achUnlockedCount(a), ACH_COUNT);
  gfx->setTextSize(2);
  gfx->setTextColor(UI_TRACK);
  gfx->setCursor(72, 70);
  gfx->print(buf);
  if (xLastAch >= 0) {
    snprintf(buf, sizeof(buf), "%s: %.12s", xt(XS_NEWACH), achDef((uint8_t)xLastAch).name);
    gfx->setTextColor(UI_BAR_OK);
    gfx->setTextSize(1);
    gfx->setCursor(150, 76);
    gfx->print(buf);
  }
  int pages = (ACH_COUNT + X_ROWS - 1) / X_ROWS;
  if (xAchPage >= pages) xAchPage = 0;
  for (uint8_t r = 0; r < X_ROWS; r++) {
    uint8_t id = (uint8_t)(xAchPage * X_ROWS + r);
    if (id >= ACH_COUNT) break;
    const AchDef &d = achDef(id);
    bool done = achUnlocked(a, id);
    char right[14];
    if (done) snprintf(right, sizeof(right), "OK");
    else snprintf(right, sizeof(right), "%u/%u", a.progress[id], d.goal);
    char left[24];
    snprintf(left, sizeof(left), "%.14s", d.name);
    xRow(94 + r * 42, done ? UI_BAR_OK : UI_TRACK, left, right, done ? UI_BAR_OK : UI_TRACK);
  }
  xPager(pages, xAchPage);
}
static void xAchTap(int x, int y) {
  int pages = (ACH_COUNT + X_ROWS - 1) / X_ROWS;
  if (y >= 348 && y <= 386) {
    if (x >= 76 && x <= 170 && xAchPage > 0) xAchPage--;
    else if (x >= 296 && x <= 390 && xAchPage + 1 < pages) xAchPage++;
    else return;
    xLastAch = -1; cardDirty = true; sfxPlay(SFX_TAP);
  }
}

// ---- page 13: accessories ----
static void renderCardAcc() {
  xHeader(xt(XS_ACC));
  AccState &a = pet.extras.acc;
  uint8_t owned = 0;
  for (uint8_t i = 0; i < ACC_COUNT; i++) owned += accOwned(a, i) ? 1 : 0;
  char buf[24];
  snprintf(buf, sizeof(buf), "%u/%u", owned, ACC_COUNT);
  gfx->setTextSize(2);
  gfx->setTextColor(UI_TRACK);
  gfx->setCursor(72, 70);
  gfx->print(buf);
  int pages = (ACC_COUNT + X_ROWS - 1) / X_ROWS;
  if (xAccPage >= pages) xAccPage = 0;
  static const uint16_t RC[4] = {UI_TRACK, UI_BAR_OK, UI_BAR_WARN, UI_BAR_BAD};
  for (uint8_t r = 0; r < X_ROWS; r++) {
    uint8_t id = (uint8_t)(xAccPage * X_ROWS + r);
    if (id >= ACC_COUNT) break;
    const AccDef &d = accDef(id);
    bool has = accOwned(a, id);
    char left[24];
    snprintf(left, sizeof(left), "%.13s", has ? d.name : "???");
    xRow(94 + r * 42, RC[d.rarity & 3], left, accEquipped(a, id) ? "ON" : (has ? "" : "-"), UI_BAR_OK);
  }
  xPager(pages, xAccPage);
  xDrawMsg();
}
static void xAccTap(int x, int y) {
  AccState &a = pet.extras.acc;
  int pages = (ACC_COUNT + X_ROWS - 1) / X_ROWS;
  if (y >= 348 && y <= 386) {
    if (x >= 76 && x <= 170 && xAccPage > 0) xAccPage--;
    else if (x >= 296 && x <= 390 && xAccPage + 1 < pages) xAccPage++;
    else return;
    xMsg[0] = 0; cardDirty = true; sfxPlay(SFX_TAP);
    return;
  }
  int row = (y - 94) / 42;
  if (row < 0 || row >= X_ROWS || y > 94 + row * 42 + 34) return;
  uint8_t id = (uint8_t)(xAccPage * X_ROWS + row);
  if (id >= ACC_COUNT) return;
  if (!accOwned(a, id)) { xSetMsg(xt(XS_NOTOWNED)); sfxPlay(SFX_DENY); }
  else { accToggleEquip(a, id); xSetMsg(accEquipped(a, id) ? xt(XS_EQUIP) : xt(XS_UNEQUIP)); sfxPlay(SFX_MENU); pet.commit(); }
  cardDirty = true;
}

// ---- dispatch ----
void renderExtrasCard(uint8_t page) {
  if (page == 9) renderCardTrainer();
  else if (page == 10) renderCardGarden();
  else if (page == 11) renderCardBag();
  else if (page == 12) renderCardAch();
  else renderCardAcc();
}
bool extrasCardTap(uint8_t page, int x, int y) {
  if (y >= 400) return false;   // let the caller close the card
  if (page == 9) xTrainerTap(x, y);
  else if (page == 10) xGardenTap(x, y);
  else if (page == 12) xAchTap(x, y);
  else if (page == 13) xAccTap(x, y);
  return true;
}

// periodic work: achievements, daily seed, unlock sound
void extrasLoop(uint32_t now) {
  if ((int32_t)(now - xNextSync) < 0) return;
  xNextSync = now + 30000UL;
  uint32_t epoch = expeditionNowEpoch();
  if (epoch > 86400UL * 365UL) extrasDailySeed(pet.extras, epoch / 86400UL, (uint8_t)random(3));
  if (pet.syncAchievements()) {
    uint8_t id;
    while (achPopBanner(pet.extras.ach, &id)) xLastAch = (int8_t)id;
    sfxPlay(SFX_MEDAL);
    if (cardOpen) cardDirty = true;
  }
}

#endif  // TAMAPOKE_EXTRAS
