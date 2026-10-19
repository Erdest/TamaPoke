# TamaPoke Gen 9 — Complete Feature List

Everything this firmware does, grouped by system. Values are straight from the source code.

---

## 1. Pokédex & Species

- **1,025 Pokémon** (National Dex #001–#1025, Generations 1–9)
- **18 types**: Normal, Fire, Water, Electric, Grass, Ice, Fighting, Poison, Ground, Flying, Psychic, Bug, Rock, Ghost, Dragon, Dark, Steel, Fairy
- **Dual typing** — every species has a primary and optional secondary type
- **Shiny variants** for all 1,025 species (separate sprites loaded from SD)
- **Pokédex tracking** with three independent bitmaps:
  - **Raised** — species you've raised from an egg
  - **Caught** — wild species caught after battle
  - **Shiny** — shiny variants raised or caught
- **Pokédex gallery** — thumbnail grid view of all species, swipeable
  - Filter by: All / Raised / Caught
  - Tap any entry for a **detail view** with full animated PMD sprite and localized name
  - Caught mark (Pokéball icon) on species you've caught wild
  - Type badge shown on each entry
- **Pokédex counter** — "X / 1025" shown on screen

---

## 2. Starter Selection & Regions

- **9 regions**, each with 3 starters (first-run choice):
  - **Kanto**: Bulbasaur / Charmander / Squirtle
  - **Johto**: Chikorita / Cyndaquil / Totodile
  - **Hoenn**: Treecko / Torchic / Mudkip
  - **Sinnoh**: Turtwig / Chimchar / Piplup
  - **Unova**: Snivy / Tepig / Oshawott
  - **Kalos**: Chespin / Fennekin / Froakie
  - **Alola**: Rowlet / Litten / Popplio
  - **Galar**: Grookey / Scorbunny / Sobble
  - **Paldea**: Sprigatito / Fuecoco / Quaxly
- Two-step picker: choose region first, then one of three starters
- After the first pet, all subsequent Pokémon come from random eggs

---

## 3. Core Tamagotchi Care (4 Stats, 0–100)

### Stat Decay (per minute while awake)
| Stat | Base Drain | Modifiers |
|------|-----------|-----------|
| **FOOD** | −2/min | −1 instead at night (22:00–06:00); nature foodDrainMod (−1 to +1) |
| **JOY** | −1/min | −2 extra if FOOD < 30; −2 extra if HYG < 30; nature joyDrainMod |
| **ENERGY** | −1/min | −1 extra if overweight (weight > 50) |
| **HYGIENE** | −1/min | −4 extra per poop on screen (max 3 poops) |

### During Sleep
- ENE recovers **+6/min** (modified by nature sleepRecovMod: −1 to +3)
- All other drains reduced to ~¼ speed with floors (FOOD 30 / JOY 35 / HYG 45)
- No poops, no slip-ups, no run-away while sleeping

### Care Actions
- 🍎 **Feed Berry** (3 flavors: Red, Blue, Green): +25 FOOD each
  - Each species has a **hidden favorite flavor** (determined by `dex % 3`)
  - Favorite berry: +35 FOOD, +10 JOY, heart animation, bond boost, flavor revealed permanently
  - **Nature berry preference**: if the berry matches the nature's taste (even if not species favourite), +5 FOOD, +3 JOY
  - **Garden berry bonus**: if fed from the Berry Garden inventory, +5 FOOD / +2 JOY extra on top
- 🍬 **Feed Candy**: +10 FOOD, +12 JOY, **+12 weight** (fattening)
- 🫧 **Bath**: animated foam scene, clears all poops, HYG → 100
- 👆 **Pet/Caress** (tap the creature): +5 JOY, bond boost, species cry plays
- 🌙 **Sleep/Wake toggle**: puts pet to sleep (ENE recovery mode) or wakes it

### Poops
- ~15% chance per minute to poop (only if FOOD > 40); modified by nature poopChanceMod (−4 to +4, minimum 1%)
- Max 3 poops on screen at once
- Each poop adds −4 HYG/min drain on top of normal decay
- Bath clears all poops instantly

### Care Slip-ups
- Triggered when **any stat hits ≤ 10** (30-minute cooldown between counts)
- Each slip-up **delays evolution by 1 level**
- Each slip-up cools the bond

---

## 4. Leveling & Evolution

### Leveling
- **1 real minute = 1 in-game minute**
- **+1 level every 60 real minutes** (purely time-based)
- Level cap: **100**
- Levels tick even while powered off via RTC (catches up to 2 weeks)

### Evolution System
- Triggers when **level ≥ species evolution level** AND **at least 3 of 4 stats > 40**
- Never automatic — a **red CTA button** appears; you tap to witness the evolution
- **Evolution animation**: halo, rays, sparkles, flicker between old and new form (~5.2 seconds)
- Evolution cry plays at the moment of transformation
- Each care slip-up delays evolution by 1 extra level
- Cannot evolve while sleeping

### Evolution Methods (mapped to TamaPoke triggers)
| Original Method | TamaPoke Equivalent |
|----------------|-------------------|
| Level-up | Level 16 / 32 / 36 (standard) |
| Friendship | `EVO_BOND` — bond ≥ 30, checked at level ≥ threshold |
| Friendship + Day | `EVO_BOND_DAY` — bond ≥ 30 + daytime (06:00–18:00) |
| Friendship + Night | `EVO_BOND_NIGHT` — bond ≥ 30 + nighttime (18:00–06:00) |
| Stone / Item | Level ~30 equivalent |
| Trade | Level ~40 equivalent (single-player fallback); **instant when actually traded via multiplayer** |
| Trade with item | Level ~38 equivalent (single-player); **instant via multiplayer if holding correct item** |
| Location-based | Level ~35 equivalent |
| Stat-based (Tyrogue) | ATK vs DEF comparison at evolution level |
| Personality (Tandemaus→Maushold, Dunsparce→Dudunsparce) | 50/50 random branch |

### Special Evolution Branches
- **Eevee**: 8 eeveelutions (Vaporeon, Jolteon, Flareon, Espeon, Umbreon, Leafeon, Glaceon, Sylveon) — branches toward whichever you're still missing
- **Tyrogue**: branches to Hitmonlee / Hitmonchan / Hitmontop based on ATK vs DEF
- **Wurmple**: random 50/50 → Silcoon or Cascoon
- **All 59 cross-gen evolutions** included (Magneton→Magnezone, Rhydon→Rhyperior, Scyther→Kleavor, Primeape→Annihilape, etc.)
- **Decline option**: "Keep form" — re-offered at the next level, or after 1 day at level 100

### Trade Evolution (Multiplayer)
- **26 species** evolve exclusively by trading with another player
- Trade without item: Kadabra→Alakazam, Machoke→Machamp, Graveler→Golem, Haunter→Gengar, Boldore→Gigalith, Gurdurr→Conkeldurr, Phantump→Trevenant, Pumpkaboo→Gourgeist
- Trade with item: Onix+Metal Coat→Steelix, Scyther+Metal Coat→Scizor, Seadra+Dragon Scale→Kingdra, Porygon+Up-Grade→Porygon2, Porygon2+Dubious Disc→Porygon-Z, Rhydon+Protector→Rhyperior, Electabuzz+Electirizer→Electivire, Magmar+Magmarizer→Magmortar, and more
- **Shelmet ↔ Karrablast**: both evolve simultaneously when traded with each other (Escavalier / Accelgor)
- Evolution triggers **instantly** upon arrival at the recipient's device
- **Single-player fallback**: level-threshold evolution (~38–40) so no species is permanently locked

---

## 5. Egg & Hatching System

### Hatching
- Tap the egg **3 times** to hatch (or wait — it hatches on its own after 3 minutes)
- Egg shows cracks at each tap
- Egg rarity indicator visible: Legendary / Rare / Common glow
- Species cry plays at the moment of hatching
- **First hatch gives starter seeds**: 2× Red, Blue, Green berry seeds for the garden

### Rarity Tiers (from ~480+ base forms that come from eggs)
| Tier | Base Chance | After Farewell | Species Count |
|------|------------|----------------|--------------|
| ✨ Legendary | ~3% | ~10% | 114 |
| 🔵 Rare | ~27% | ~45% | 149 |
| ⚪ Common | remainder | remainder | 310 |

- **Legendaries** only appear after you've registered **≥ 25** Pokémon
- Clean **farewell blesses** the next egg (better rarity odds)
- **Run-away curses** the next egg (forces Common tier)
- Within a tier, the system **favors species whose evolution line you haven't finished**
- Every species in the game is completable
- **Enigma Berry blessing**: using a garden Enigma Berry gives the next egg +1 rarity tier
- **Starf Berry blessing**: using a garden Starf Berry sets the next egg's shiny odds to 1/8

### Shiny Odds
- Base: **1/48**
- After farewell: **1/24**
- Care bonus (streak + bond): reduces denominator by up to 16 (floor 8)
- Starf Berry blessing: overrides to **1/8**
- Step milestone bonus: additional reduction (see §12)

### Regional Form Hatching
- When a species with regional variants hatches, there's a **20% base chance** it hatches as a regional form
- Chance increases with bond: +1% per bond level (max ~50% at bond 30)
- Regional form is chosen randomly if multiple variants exist (e.g., Alolan vs Galarian Meowth)

---

## 6. Battle System

### Wild Encounters
- ~5% chance per minute check (only when awake, not in egg, not in ceremony)
- 20-minute cooldown between battles
- Wild Pokémon level: ±5 of pet's level (random)
- Species picked from current day-phase biome pool

### Battle Mechanics
- **Turn-based** combat with 5 action choices:
  - **Attack (Quick)**: 80% of ATK, +1 priority (goes first)
  - **Attack (Heavy)**: 130% of ATK, −1 priority (goes last), risky
  - **Dodge**: 60% chance to avoid all damage, charges Counter
  - **Counter**: available after successful dodge, 200% ATK damage
  - **Rest**: heals 25% max HP (2 uses per battle)
- **Type effectiveness**: full 18×18 type chart (super effective = 120%, not very = 85%, immune = 70%)
- **Dual-type calculation**: multiplied (e.g. Ice vs Grass/Flying = 120% × 120% = 144%)
- Speed determines who goes first (with priority modifiers)
- **Move System** (§39): replaces generic Attack with 4 learnable typed moves with PP, STAB, and status effects

### Catch System
- After winning: **catch chance** based on level difference, HP remaining, and Poké Ball type used (§40)
- **10 Poké Ball types** with unique modifiers (1.0×–4.0× catch rate)
- **Critical catch**: 4% chance for instant capture
- "Respect catch" if you barely won (close HP differential)
- Caught Pokémon registered in Pokédex with "caught" mark

### Rewards
- Winner gets: +JOY, +bond, +stat training (ATK/DEF/SPE weighted by how you fought)
- Win streak tracking with escalating rewards
- Battle record: total wins/losses/streak/best streak shown on card page

---

## 7. Battle Gimmicks (Mega Evolution / Dynamax / Terastallize)

### Mega Evolution (48 species)
- Available in battle when bond ≥ 20 and species has a Mega form
- **Once per battle** — lasts until battle ends
- +30% to ATK and DEF, +20% to SPE
- Visual: unique Mega sprite loaded from SD (`/mega/xxx.tpk`)
- Type may change (e.g. Charizard X → Fire/Dragon)
- 48 Mega Evolutions: Venusaur, Charizard X/Y, Blastoise, Alakazam, Gengar, Kangaskhan, Pinsir, Gyarados, Aerodactyl, Mewtwo X/Y, Ampharos, Scizor, Heracross, Houndoom, Tyranitar, Blaziken, Gardevoir, Mawile, Aggron, Medicham, Manectric, Banette, Absol, Garchomp, Lucario, Abomasnow, Gallade, Audino, Beedrill, Pidgeot, Slowbro, Steelix, Sableye, Sharpedo, Camerupt, Altaria, Glalie, Salamence, Metagross, Latias, Latios, Rayquaza, Lopunny, Diancie

### Dynamax / Gigantamax
- Available when bond ≥ 25
- **Dynamax**: any non-Mega species can Dynamax; HP doubles for 3 turns
- **Gigantamax**: 32 species with special G-Max forms and moves
  - Charizard, Butterfree, Pikachu, Meowth, Machamp, Gengar, Kingler, Lapras, Eevee, Snorlax, Garbodor, Corviknight, Orbeetle, Drednaw, Coalossal, Flapple, Appletun, Sandaconda, Toxtricity, Centiskorch, Hatterene, Grimmsnarl, Alcremie, Copperajah, Duraludon, Urshifu
- Visual: G-Max sprite from SD (`/gmax/xxx.tpk`), red cloud aura for regular Dynamax
- Wears off after 3 turns automatically

### Terastallization
- Available when bond ≥ 30
- **Tera Type** assigned randomly at hatch (any of 18 types)
- Activating replaces your Pokémon's defensive typing with the Tera Type
- +50% STAB bonus if Tera Type matches a move's type
- Visual: crystalline crown overlay (color matches Tera Type)
- Persists until battle ends

### Gimmick Priority
- Only **one gimmick per battle** (Mega OR Dynamax OR Tera — not multiple)
- If species qualifies for multiple, player chooses which to activate
- Bond thresholds: Mega ≥ 20, Dynamax ≥ 25, Tera ≥ 30

---

## 8. Local Multiplayer (WiFi + BLE)

### Connection
- **BLE discovery**: 5-second scan, 10-meter range, up to 8 peers visible
- **WiFi Direct**: used for actual data transfer (battles + trades)
- Auto-negotiates: BLE for discovery → WiFi Direct for gameplay
- No internet required — fully local, peer-to-peer

### PvP Battle
- Both players submit actions simultaneously (no speed advantage)
- Same battle mechanics as wild battles (type chart, dodge, counter, moves, gimmicks)
- Move system active: each player uses their own 4-move set
- Winner gets: +15 JOY, +10 bond, training stat boost
- Loser gets: +5 JOY (participation), no stat penalty
- Win/loss recorded separately from wild battle stats

### Direct Trade
- Select a Pokémon from your party → preview swap → confirm
- Both sides must confirm before trade executes
- **Trade evolution triggers instantly** upon arrival at the recipient's device
- **Trade items** (§4): if the traded Pokémon is holding the correct item, evolution fires immediately
  - 13 trade items: Metal Coat, King's Rock, Dragon Scale, Up-Grade, Dubious Disc, Protector, Electirizer, Magmarizer, Reaper Cloth, Sachet, Whipped Dream, Prism Scale, Deep Sea Tooth/Scale
  - Items found on expeditions (rare loot) and as gym rewards

### Wonder Trade
- Blind random swap: deposit one Pokémon, receive a random one from a paired device
- Pairs with any nearby TamaPoke running Wonder Trade simultaneously
- No species restrictions — you might get a legendary
- Daily limit: 3 Wonder Trades per day
- Trade evolutions can trigger (Haunter → Gengar if you receive a Haunter)

---

## 9. Regional Forms (59 Variants)

### Alolan Forms (18 species)
Rattata, Raticate, Raichu, Sandshrew, Sandslash, Vulpix, Ninetales, Diglett, Dugtrio, Meowth, Persian, Geodude, Graveler, Golem, Grimer, Muk, Exeggutor, Marowak
- Completely different types (e.g. Vulpix: Fire → Ice, Marowak: Ground → Fire/Ghost)

### Galarian Forms (20 species)
Meowth, Ponyta, Rapidash, Slowpoke, Slowbro, Slowking, Farfetch'd, Weezing, Mr. Mime, Corsola, Zigzagoon, Linoone, Darumaka, Darmanitan, Yamask, Stunfisk, Mr. Rime (evo), Cursola (evo), Obstagoon (evo), Perrserker (evo), Sirfetch'd (evo), Runerigus (evo)

### Hisuian Forms (17 species)
Growlithe, Arcanine, Voltorb, Electrode, Typhlosion, Qwilfish, Sneasel, Samurott, Lilligant, Zorua, Zoroark, Braviary, Sliggoo, Goodra, Avalugg, Decidueye, Overqwil (evo), Sneasler (evo)

### Paldean Forms (4 species)
Wooper, Tauros (3 breeds: Combat, Blaze, Aqua), Clodsire (evo)

### Regional Form Mechanics
- **Hatch chance**: 20% base when a species with regional variants hatches
- Chance increases with bond: +1% per bond level
- Regional forms have **different types and base stats** from the originals
- **Separate Pokédex tracking** — regional form bitmap (128 bytes NVS)
- **9 regional-exclusive evolutions** that only exist in regional form:
  - Galarian Meowth → Perrserker, Galarian Farfetch'd → Sirfetch'd, Galarian Linoone → Obstagoon
  - Galarian Corsola → Cursola, Galarian Yamask → Runerigus, Galarian Mr. Mime → Mr. Rime
  - Hisuian Qwilfish → Overqwil, Hisuian Sneasel → Sneasler
  - Paldean Wooper → Clodsire

---

## 10. Gym Challenge, Elite Four & Champion

### Structure (117 Trainers Total)
- **72 Gym Leaders**: 8 gyms per region × 9 regions, each with a type specialty
- **36 Elite Four**: 4 per region × 9 regions
- **9 Champions**: 1 per region (Lance, Steven, Cynthia, Alder, Diantha, Kukui, Leon, Geeta, and a bonus Red)
- 426 trainer Pokémon across 321 unique species

### Gym Battle Mechanics
- **Multi-mon gauntlet**: fight the leader's full team (3–6 Pokémon) sequentially
- **HP carries over** between opponents within a gauntlet
- **Rest charges** (2 per gauntlet) don't reset between opponents
- Move system active: you use your 4 learned moves, leader uses type-appropriate AI moves
- **Level scaling**: opponents scale to `(base + playerLevel) / 2` when overleveled
- Minimum player level required to challenge each gym (10/15/20/25/30/35/40/45)
- **Sequential progression**: must beat gym N before challenging gym N+1 in each region

### Elite Four + Champion
- Unlocked after beating all 8 gyms in a region
- 4 E4 battles in a row → Champion battle (5th)
- 20% HP recovery between E4 members
- Champion has a full 6-Pokémon team
- Losing mid-run resets to the start (but beaten E4 members stay beaten on re-entry)

### Gym Leaders by Region
| Region | Gym 1 | Gym 2 | Gym 3 | Gym 4 | Gym 5 | Gym 6 | Gym 7 | Gym 8 |
|--------|-------|-------|-------|-------|-------|-------|-------|-------|
| Kanto | Brock (Rock) | Misty (Water) | Lt. Surge (Electric) | Erika (Grass) | Koga (Poison) | Sabrina (Psychic) | Blaine (Fire) | Giovanni (Ground) |
| Johto | Falkner (Flying) | Bugsy (Bug) | Whitney (Normal) | Morty (Ghost) | Chuck (Fighting) | Jasmine (Steel) | Pryce (Ice) | Clair (Dragon) |
| Hoenn | Roxanne (Rock) | Brawly (Fighting) | Wattson (Electric) | Flannery (Fire) | Norman (Normal) | Winona (Flying) | Tate&Liza (Psychic) | Wallace (Water) |
| Sinnoh | Roark (Rock) | Gardenia (Grass) | Maylene (Fighting) | Wake (Water) | Fantina (Ghost) | Byron (Steel) | Candice (Ice) | Volkner (Electric) |
| Unova | Cilan (Grass) | Lenora (Normal) | Burgh (Bug) | Elesa (Electric) | Clay (Ground) | Skyla (Flying) | Brycen (Ice) | Drayden (Dragon) |
| Kalos | Viola (Bug) | Grant (Rock) | Korrina (Fighting) | Ramos (Grass) | Clemont (Electric) | Valerie (Fairy) | Olympia (Psychic) | Wulfric (Ice) |
| Alola | Ilima (Normal) | Lana (Water) | Kiawe (Fire) | Mallow (Grass) | Sophocles (Electric) | Acerola (Ghost) | Nanu (Dark) | Hapu (Ground) |
| Galar | Milo (Grass) | Nessa (Water) | Kabu (Fire) | Bea (Fighting) | Allister (Ghost) | Gordie (Rock) | Melony (Ice) | Raihan (Dragon) |
| Paldea | Katy (Bug) | Brassius (Grass) | Iono (Electric) | Kofu (Water) | Larry (Normal) | Ryme (Ghost) | Tulip (Psychic) | Grusha (Ice) |

### Rematch System
- After completing all 8 gyms + E4 + Champion in a region → **rematch mode** unlocked
- 3 difficulty tiers: Normal / Hard (+10 levels) / Elite (+20 levels)
- Rematches use the same gauntlet format with scaled-up teams

### Rewards
| Achievement | Reward |
|-------------|--------|
| Each gym badge | +15 JOY, +10 bond, training stat boost, 2 Ultra Balls, accessory drop (25%) |
| Every 4th badge | +1 Luxury Ball |
| Full 8 badges (region) | Legendary egg blessing |
| E4 member beaten | +10 JOY, +5 bond |
| Champion beaten | +25 JOY, +15 bond, guaranteed rare accessory, legendary egg |
| All 9 Champions | Shiny legendary egg, "Grand Champion" achievement |
| First Champion victory | 1 Master Ball |

### Badge Display
- Card page showing all 72 badges in a 9-row × 8-column grid
- Gold dots for earned badges, grey for unearned
- Region progress bar, total badge count ("X / 72")
- E4 progress shown as 4 small squares + crown icon per region

---

## 11. Expeditions

- **3 durations**: 15 min / 30 min / 60 min (costs 10 / 20 / 40 ENE)
- Timer runs in real time (even while powered off via RTC)
- **Loot**: random item on completion:
  - Snack (+15 FOOD)
  - Energy drink (+15 ENE)
  - Care kit (+10 to all stats)
  - Training weight (+5 to a chosen training stat)
  - Poké Balls (20% chance): Great / Net / Dusk / Timer / Quick
  - Trade evolution items (rare): Metal Coat, King's Rock, etc.
  - Berry seeds (uncommon): any of 12 garden berry types
  - Accessory drop (15% chance)
- **Inventory**: up to 3 items held at once
- **Expedition HUD**: small icon on main screen shows active/ready/bag status
- Cannot start if energy too low

---

## 12. Pedometer & Steps

### Step Detection
- **QMI8658 IMU** accelerometer-based (no GPS)
- Software step counter with configurable sensitivity
- Steps counted even during light sleep (70 ms poll interval)

### Daily Step Milestones
| Milestone | Steps | Reward |
|-----------|-------|--------|
| 🥉 Bronze | 1,000 | Snack item |
| 🥈 Silver | 3,000 | Energy drink |
| 🥇 Gold | 5,000 | Training weight, +1 Premier Ball |

### Trail Rank (Lifetime Steps)
| Rank | Total Steps | Perk |
|------|------------|------|
| 🐣 Hatchling | 0 | — |
| 🚶 Walker | 5,000 | +JOY from walks |
| 🏃 Jogger | 25,000 | Better shiny odds (−4 denominator) |
| ⛰️ Hiker | 100,000 | Better catch bonus (+5%) |
| 🌍 Explorer | 250,000 | Best shiny odds (−8 denominator), Explorer Bag accessory |

### Shiny & Catch Bonuses
- Shiny denominator reduced by Trail Rank (base 48 → as low as 36)
- Catch chance bonus +2% to +5% based on Trail Rank

---

## 13. Bond & Streak

### Care Streak
- **+1 streak** for each day you perform at least one care action
- Breaks if you miss a full calendar day
- Best streak tracked persistently (survives across Pokémon)
- Milestones celebrated: 7, 30, 100, 365 days

### Bond (0–100)
- **Per-Pokémon** — resets to 0 with each new hatch
- +1 per feeding (favorite berry: +2)
- +1 per caress / petting
- +1 per minigame completed
- +1 per battle won
- +1 per 120 minutes of passive care (inactive party)
- Daily cap: 10 bond gain per day (prevents grinding)
- Bond decays on care slip-ups (−2 per slip-up)
- **Bond unlocks**: gimmick thresholds (Mega ≥ 20, Dynamax ≥ 25, Tera ≥ 30)
- **Bond affects**: egg rarity, shiny odds, evolution triggers, regional form hatch chance

---

## 14. Daily Goals

- **3 random goals** generated each calendar day:
  - Care: perform N care actions
  - Play: score N in a minigame
  - Battle: win N battles
  - Catch: catch N wild Pokémon
  - Memo: reach round N in Simon Says
- Progress tracked per goal
- Completing all 3: bonus reward (+15 JOY, +5 bond, 1 Great Ball, berry seed)

---

## 15. Lifecycle Endings

### Farewell (Happy Ending)
- Available after **3 days** of gameplay in final evolution form
- Red CTA button appears (can be declined — re-offered after 1 day)
- Farewell ceremony: confetti, sparkles, heart animation (10 seconds)
- Pokémon leaves a **blessed egg** (better rarity odds)
- Streak continues, bond milestone recorded

### Run-Away (Neglect Ending)
- Triggers after **60 consecutive minutes** with ALL 4 stats at 0
- Sad animation: Pokémon walks off screen
- Next egg is **cursed** (forced Common tier)
- Streak breaks

### Release (Manual)
- Long-press on pet → "Release?" confirmation
- Neutral ending — no blessing or curse
- Immediate new egg

---

## 16. Minigames (5 + Training + Quiz)

### Ball Bounce (JOY + weight loss)
- Tap to keep a Pokéball in the air
- Score = number of hits
- +JOY based on score, −weight (burns off candy)

### Catch (SPE training)
- Tap targets that appear on screen within a time limit
- Score = catches in 30 seconds
- +SPE training

### Simon Says / Memo (JOY)
- Repeat an increasingly long sequence of 4 colored pads
- Score = rounds survived
- +JOY based on rounds

### Clean (HYG bonus)
- Tap falling dirt blobs before they hit the ground
- Score = dirt caught
- +HYG bonus

### Type Match
- "What beats X?" — pick the super-effective type from 3 choices
- 30 seconds, score = correct answers
- +JOY, +bond, trains type knowledge

### Training Bag (ATK training)
- Tap rapidly to punch a training bag
- Score = total hits in time limit
- +ATK training based on hits

### Quiz Minigame (§33)
- 3 modes × 4 difficulties, 10 rounds per session
- "Who's That Pokémon?", "What Beats X?", "Who Made That Sound?"
- Persistent high scores, accessory drops on new records

---

## 17. Training Stats (ATK / DEF / SPE, 0–100)

- **Genes**: random 90–110% multiplier per stat, set at hatch
- **Training**: 0–100 per stat, earned through:
  - ATK: training bag minigame, battle wins
  - DEF: sustained good care (consecutive healthy ticks)
  - SPE: catch minigame, walking
- **Effective stat** = `(base_stat × gene% + training) × level_scale`
- Nature modifier: ±10% to one stat pair (§30)
- Shown on card page with gene quality indicator

---

## 18. Personality System

- 5 personalities: Balanced, Playful, Brave, Calm, Lazy
- Determined by `(species + shiny_flag + age_at_hatch) % 5`
- Affects: animation speed, idle behavior, ambient sound frequency
- Visible on card page

---

## 19. Medals (8 per Pokémon)

| Medal | Condition |
|-------|-----------|
| Lv.10 | Reach level 10 |
| Lv.25 | Reach level 25 |
| Lv.50 | Reach level 50 |
| Berry | Discover favorite berry flavor |
| Streak 7 | 7-day care streak (player) |
| Bond | Bond reaches 50 |
| Final Form | Evolve to final evolution |
| Fitness | Weight stays under 20 for 24 hours |

- Medal animations play on screen when earned
- Total medals tracked across all Pokémon (lifetime counter)
- Displayed as a badge row on the card page

---

## 20. Morning Greeting

- Once per calendar day, first interaction triggers a greeting
- Pet does a happy animation + species cry
- +3 JOY, +1 bond
- Helps maintain care streak

---

## 21. Weight System

- **0–100 scale**
- +12 per candy fed
- −1 per minigame point scored (exercise burns weight)
- Overweight (> 50): −1 extra ENE drain per minute
- Underweight has no penalty
- Displayed on card page

---

## 22. Collection Frames

- Pokédex milestones unlock decorative frames for the card page:
  - 10 registered → Frame 1 (simple border)
  - 25 registered → Frame 2
  - 50 registered → Frame 3
  - 100 registered → Frame 4
  - 200 registered → Frame 5
  - Escalating up to 1025 for the final frame
- Player can switch between unlocked frames

---

## 23. SD Card Structure

```
/pmd/               Animated PMD sprites (one folder per dex number)
  /001/idle.tpk     TPK2 format (multi-frame indexed color)
  /001/walk.tpk
  /001/action.tpk
  /001s/idle.tpk    Shiny variant (suffix 's')
/cries/             Pokémon cry audio files (IMA-ADPCM)
  /001.cry          One file per species, ~3-8 KB each
  /025.cry          Pikachu, etc.
/mega/              Mega Evolution sprites
  /006x.tpk         Charizard X, etc.
/gmax/              Gigantamax sprites
  /006.tpk          G-Max Charizard, etc.
/regional/          Regional form sprites
  /019a.tpk         Alolan Rattata, etc.
  /077g.tpk         Galarian Ponyta, etc.
/photos/            Saved photo mode BMP files
  /photo_001.bmp    466×466 24-bit BMP
/thumbs.bin         Thumbnail atlas for gallery grid
/names/             Localized species names (6 languages)
```

---

## 24. Sound System (ES8311 + I2S)

### Hardware
- **ES8311 audio codec** on the board (I2S DAC)
- Amplified speaker path with MX1.25 connector
- PWR amplifier on GPIO 46

### Synthesized SFX (39 effects)
- Square-wave synthesized tones via I2S
- 4 volume levels: Off / Low / Med / Full
- Non-blocking: SFX queued and played on a background task (Core 0)
- Effects include: tap, eat, play, heart, hatch, evolve, medal, battle win/loss, catch, etc.

### Species Chirp
- Each of the 1,025 species has a unique synthesized chirp
- Generated from a hash of the dex number (frequency + pattern)
- Plays on: hatch, pet/caress, Pokédex browsing

---

## 25. Pokémon Cry System (Real Audio)

### Format & Quality
- **22,050 Hz, 16-bit mono** PCM (decoded from IMA-ADPCM on the fly)
- Stored as `.cry` files on SD card under `/cries/`
- Average cry size: **3–8 KB** (IMA-ADPCM 4:1 compression)
- Total for all 1,025 species: **~3–8 MB**

### Playback Engine
- FreeRTOS streaming task on **Core 0** (doesn't block game loop)
- 16 KB PSRAM ring buffer, DMA-fed I2S output
- Latency: **~15 ms** from call to first audio
- Volume respects the global 4-level setting

### Cry Variants
| Flag | Effect | Usage |
|------|--------|-------|
| Normal | Original pitch | Hatch, pet, Pokédex |
| `CRY_FLAG_PITCHED` | +10% pitch | Taking a hit in battle |
| `CRY_FLAG_LOW` | −15% pitch | Fainting, farewell |
| `CRY_FLAG_LOOP` | Loops until stopped | Pokédex detail view ambient |

### Fallback
- If a `.cry` file is missing, `cryPlay()` returns `false`
- Caller falls back to the synthesized chirp (§24)
- Cries can be added incrementally — no all-or-nothing requirement

### Conversion Pipeline
- `tools/pack_cries.py`: takes WAV/OGG files → trims silence → resamples to 22,050 Hz → normalizes → encodes IMA-ADPCM → outputs `.cry` files

---

## 26. Real-Time Clock (PCF85063)

- Dedicated RTC chip on I2C bus (shared with touch + PMU)
- Keeps time while powered off (coin cell backed)
- Used for:
  - Level progression while off (up to 2 weeks catch-up)
  - Expedition timers
  - Day/night cycle (affects biomes, encounters, evolution triggers)
  - Daily goal reset
  - Step counter day boundary
  - Care streak tracking
  - Berry garden growth timers
- Clock set via settings screen (hour/minute picker)

---

## 27. Hardware & Power

### Display
- **466×466 AMOLED** (CO5300 driver, QSPI at 80 MHz)
- Full PSRAM framebuffer (no tearing — double-buffered flush)
- 3 brightness stages: Full → Dim (90s idle) → Near-off (5 min idle)
- AMOLED watermark protection (shifts pixels periodically)

### Touch
- **CST9217** capacitive touch (I2C)
- Interrupt-driven (GPIO 11, falling edge) — no polling when idle
- Gesture detection: tap, long-press, swipe up/down/left/right

### Power Management
- **AXP2101 PMU** manages battery, USB charging, and rail enables
- Battery voltage shown on screen (icon + percentage)
- **Light sleep** between frames when idle (ESP32 light sleep with timer + touch wakeup)
- Screen-off mode: 5-second render interval, minimal power
- Optional **power save mode**: reduces frame rate, increases light sleep

### IMU (QMI8658)
- 6-axis (accelerometer + gyroscope)
- Used for: pedometer, shake detection, tilt (future gesture minigames)
- Polled (no dedicated interrupt pin in this board revision)

---

## 28. Achievement System (51 Achievements)

### Categories & Counts
| Category | Count | Examples |
|----------|-------|---------|
| 🏆 Collection | 12 | "Hatch 10/50/100/200 Pokémon", "Complete Kanto Dex", "Catch every type" |
| 🌱 Growth | 14 | "Raise to Lv.50/100", "30-day care streak", "Max bond" |
| ⚔️ Combat | 11 | "Win 100 battles", "10-win streak", "Beat all Champions" |
| 🎯 Activity | 14 | "Walk 50K steps", "Take 20 photos", "Quiz: 500 correct", "Grow 50 berries" |

### Mechanics
- **Progress tracked as uint16_t counters** (0–65535) per achievement
- Progress persists across eggs and Pokémon (lifetime stats)
- **Banner queue**: up to 4 achievements shown sequentially (4 seconds each)
- Gold bar + medal SFX when unlocked
- **Gallery page** (card page 10): scrollable list with category filter tabs, progress bars
- Total NVS cost: **109 bytes** (102 for progress + 7 for unlock bitfield)

### Notable Achievement Rewards
| Achievement | Reward |
|-------------|--------|
| Hatch 100 Pokémon | +1 egg rarity tier permanently |
| 100-day streak | Permanent shiny odds −4 |
| Complete National Dex | Crown accessory + title |
| Beat all 9 Champions | "Grand Champion" title, shiny legendary egg |
| Walk 250K steps | Explorer Bag accessory |
| Quiz: 500 correct | Monocle accessory |

---

## 29. Watch Face / Clock Mode

### Activation
- **Auto-activates** after 10 minutes idle on the main screen
- Any tap exits back to full TamaPoke UI
- Can also be entered manually from settings

### Display (AMOLED-Optimized)
- **Pure black background** — unlit AMOLED pixels = zero power draw
- Custom **5×7 pixel font** for time digits (scale 6 = large and readable)
- Layout:
  - Top: **HH:MM** (large) with blinking colon + seconds
  - Middle: date string (day-of-week, month, day)
  - Center: **animated pet sprite** (idle animation, 1 FPS)
  - Below pet: name + level
  - **4 stat dots**: green (> 40), orange (20–40), red (≤ 20), pulsing when critical
  - Bottom: step counter icon + daily steps / battery % / Pokédex count / care streak
- **1 FPS render rate**, brightness dropped to 15
- Estimated **~3× less battery** than normal gameplay

### Pet Status at a Glance
- If any stat is critical (≤ 10): dot pulses rapidly + sad pet expression
- If sleeping: "💤" shown, calm animation
- If expedition active: "📦" icon with remaining time

---

## 30. Nature System (25 Natures)

### Nature Table
| Nature | +10% Stat | −10% Stat | Food Drain | Joy Drain | Poop% | Sleep Recov | Bond Mod | Berry Pref | Anim Speed |
|--------|-----------|-----------|------------|-----------|-------|-------------|----------|------------|------------|
| Hardy | — | — | 0 | 0 | 0 | 0 | 0 | — | 200ms |
| Lonely | ATK | DEF | 0 | +1 | 0 | 0 | 0 | Red | 200ms |
| Brave | ATK | SPE | +1 | 0 | +2 | +1 | −1 | Red | 180ms |
| Adamant | ATK | — | +1 | 0 | +1 | 0 | −1 | Red | 170ms |
| Naughty | ATK | DEF | 0 | −1 | +4 | −1 | 0 | Red | 150ms |
| Bold | DEF | ATK | 0 | 0 | −1 | 0 | +1 | Blue | 210ms |
| Docile | — | — | 0 | 0 | 0 | 0 | 0 | — | 200ms |
| Relaxed | DEF | SPE | −1 | −1 | 0 | +2 | 0 | Blue | 230ms |
| Impish | DEF | — | 0 | −1 | +1 | 0 | 0 | Blue | 180ms |
| Lax | DEF | — | 0 | −1 | +2 | +1 | 0 | Blue | 220ms |
| Timid | SPE | ATK | −1 | +1 | −2 | −1 | 0 | Green | 160ms |
| Hasty | SPE | DEF | 0 | 0 | 0 | −1 | 0 | Green | 150ms |
| Serious | — | — | 0 | 0 | 0 | 0 | 0 | — | 200ms |
| Jolly | SPE | — | 0 | −2 | 0 | 0 | +1 | Green | 150ms |
| Naive | SPE | DEF | 0 | −2 | +1 | 0 | 0 | Green | 160ms |
| Modest | — | ATK | −1 | 0 | −1 | +1 | 0 | Blue | 250ms |
| Mild | — | DEF | 0 | 0 | 0 | 0 | 0 | Blue | 200ms |
| Quiet | — | SPE | 0 | 0 | −2 | +3 | 0 | Blue | 250ms |
| Bashful | — | — | 0 | +1 | 0 | 0 | 0 | — | 200ms |
| Rash | — | DEF | 0 | 0 | +2 | −1 | −1 | Red | 170ms |
| Calm | — | ATK | −1 | −1 | −3 | +2 | +1 | Green | 230ms |
| Gentle | — | DEF | −1 | 0 | −4 | +1 | +2 | Green | 220ms |
| Sassy | — | SPE | 0 | 0 | +1 | +1 | 0 | Red | 200ms |
| Careful | — | — | 0 | 0 | −2 | +1 | +1 | Green | 210ms |
| Quirky | — | — | 0 | 0 | 0 | 0 | 0 | — | 200ms |

### Assignment
- Nature assigned at egg creation: `rand() % 25`
- Persists for the Pokémon's entire life
- Stored as 1 byte in NVS (as part of pet save)
- Shown on card page with localized name (6 languages)

### Cascade Effects
- **Stat calc**: `effectiveStat = baseStat × natureMultiplier / 100` (110% or 90%)
- **Care drains**: food/joy drain modified ±1–2/min
- **Poop chance**: 11%–19% base (Gentle lowest, Naughty highest)
- **Sleep recovery**: 5–9 ENE/min (Hasty worst, Quiet best)
- **Bond gain**: −1 to +2 per tick (Gentle bonds fastest, Adamant slowest)
- **Berry preference**: Red (Spicy), Blue (Dry), Green (Sweet) — matching nature's taste gives +5 FOOD, +3 JOY
- **Animation**: frame interval 150ms (hyperactive) to 250ms (chill)

---

## 31. Berry Garden

### Structure
- **6 planting slots** visible on a dedicated garden card page
- Each slot can hold one berry plant at a time
- **12 berry types** across 3 tiers

### Berry Types
| Tier | Berry | Grow Time | Yield | Effect When Fed | Effect When Used |
|------|-------|-----------|-------|-----------------|------------------|
| Basic | Red Berry | 2 hours | 3–5 | +30 FOOD (species fav: +40, +10 JOY) | — |
| Basic | Blue Berry | 2 hours | 3–5 | +30 FOOD (species fav: +40, +10 JOY) | — |
| Basic | Green Berry | 2 hours | 3–5 | +30 FOOD (species fav: +40, +10 JOY) | — |
| Stat | Protein Berry | 4 hours | 2–3 | +5 ATK training | — |
| Stat | Iron Berry | 4 hours | 2–3 | +5 DEF training | — |
| Stat | Carbos Berry | 4 hours | 2–3 | +5 SPE training | — |
| Battle | Sitrus Berry | 3 hours | 2–4 | Heal 30% HP in battle | Auto-use below 25% HP |
| Battle | Lum Berry | 3 hours | 2–4 | Cure status effect | Auto-cure on status |
| Rare | Liechi Berry | 6 hours | 1–2 | +15% ATK for one battle | — |
| Rare | Petaya Berry | 6 hours | 1–2 | +15% DEF for one battle | — |
| Rare | Salac Berry | 6 hours | 1–2 | +15% SPE for one battle | — |
| Special | Enigma Berry | 8 hours | 1 | — | Next egg: +1 rarity tier |
| Special | Starf Berry | 8 hours | 1 | — | Next egg: shiny odds 1/8 |

### Garden Mechanics
- **Planting**: spend 1 seed → slot begins growing (real-time, RTC-based)
- **Watering**: up to 4 times per growth cycle, 1-hour cooldown
  - Each watering reduces remaining grow time by 10% (cumulative 40%)
- **Mulching**: one-time per cycle, halves remaining time (stacks with watering)
- **Harvesting**: tap when ready → berry yield + 1 seed returned
- **Wilting**: if not harvested within 24 hours of ready → half yield, still get 1 seed
- **Perfect care**: 4 waterings + timely harvest = bonus seed drop
- **Offline growth**: uses RTC timestamps — berries grow while device is off
- Seeds found on: expeditions (uncommon), gym badge rewards, daily goal completion, battle drops

### Berry Bag
- Inventory holds harvested berries (per-type count, max 99 each)
- Feed directly from bag for garden berry bonus (+5 FOOD / +2 JOY on top of normal)
- Battle berries auto-trigger when conditions met (Sitrus below 25% HP, Lum on status)
- Special berries (Enigma/Starf) consumed as blessings for the next egg

### NVS Persistence
- 109 bytes total: 6 slots × 16 bytes + seed bag (12 bytes) + berry bag (13 bytes)

---

## 32. Accessory System (Cosmetics)

### Overview
- **32 accessories** across 4 equip slots: Head (8), Face (8), Neck (8), Back (8)
- Purely visual — no stat effects
- Rendered as **overlay sprites** on top of the pet on the main screen

### Rarity Tiers
| Tier | Count | Examples |
|------|-------|---------|
| Common | 14 | Red Cap, Trainer Hat, Round Glasses, Cool Shades, Red Scarf, Backpack |
| Uncommon | 10 | Flower Wreath, Star Glasses, Gold Chain, Hero Cape, Leaf Cloak |
| Rare | 6 | Crown, Monocle, Fairy Wings, Rocket Pack, Devil Wings |
| Special | 2 | Angel Halo, Explorer Bag (achievement-locked) |

### Acquisition
- Expeditions: 15% drop chance (Common)
- Gym badges: 25% drop chance (up to Uncommon)
- Champion victory: guaranteed drop (Rare)
- Minigame high scores: Uncommon drop
- Achievements: specific items (Crown for National Dex, Monocle for Quiz Professor, Explorer Bag for 250K steps)

### Equip System
- One accessory per slot (Head + Face + Neck + Back = 4 equipped max)
- Toggle on/off by tapping in inventory
- Accessories persist across evolution (your Charizard keeps its hat)
- Rendered in photo mode too

### Storage
- 14 bytes NVS: owned bitmask (4 bytes), new-flag bitmask (4 bytes), equipped array (4 bytes), counter (2 bytes)

---

## 33. Quiz Minigame

### Modes
| Mode | Question | Visual |
|------|----------|--------|
| Silhouette | "Who's That Pokémon?" | Black silhouette on screen |
| Type | "What Beats X?" | Type badge shown, pick super-effective type |
| Cry | "Who Made That Sound?" | Audio-only, species cry plays |

### Difficulties
| Level | Choices | Timer | Pool |
|-------|---------|-------|------|
| Easy | 4 | 8 seconds | Seen species only |
| Medium | 4 | 6 seconds | Any species |
| Hard | 6 | 4.5 seconds | Any species |
| Expert | 6 | 3 seconds | Any species + shiny silhouettes |

### Session Flow
- 10 questions per session
- Points: base 10 + speed bonus (up to 10) + streak bonus (+5 at 5+ streak)
- End-of-session: 1–3 star rating, JOY/bond rewards
- Perfect 10/10: bonus berry seed drop (50%)
- New high score: accessory drop chance

### Persistence
- 9 bytes NVS: high score per mode (6 bytes), lifetime correct (2 bytes), perfect rounds (1 byte)

---

## 34. BLE Notification Bridge (OPTIONAL)

### Compile Gate
- **Only active when `TAMAPOKE_BLE_NOTIFY` is defined** at compile time
- Without the define: all functions are zero-cost inline no-ops (0 bytes flash, 0 bytes RAM)

### Functionality
- BLE GATT server with custom service + characteristic
- Phone app writes a notification byte → pet reacts on screen
- 6 categories: Phone Call, Message, Email, Social, Calendar, Other
- Per-category SFX: urgent ring, happy bounce, nod, wave, bell chirp, generic chirp

### Settings
- Master enable/disable
- Per-category toggles (6 toggles)
- Sound on/off for notifications
- Quiet hours (start/end hour, default 22:00–07:00)
- Reaction duration (default 3 seconds)
- Show app name preview
- Forget bonded device

### Technical
- 4-event ring buffer (newest overwrites oldest if full)
- Status icon on main screen: grey (off), blue pulsing (advertising), solid blue (connected)
- Pending notification count badge
- 16 bytes NVS for settings

---

## 35. Localization (6 Languages)

- **English** (EN), **Spanish** (ES), **French** (FR), **German** (DE), **Italian** (IT), **Japanese** (JA)
- All UI strings, species names, type names, region names, nature names
- Language selector in settings
- Persisted to NVS

---

## 36. Seasonal Events

- Based on RTC calendar date
- **Halloween** (October): Ghost-type encounter boost, Pumpkaboo/Phantump/Litwick featured
- **Winter** (December): Ice-type boost, Delibird daily gift, Stantler spawns
- **Pokémon Day** (Feb 27): all shiny odds doubled, guaranteed rare encounter
- **Birthday** (user-set): confetti animation, free rare egg, +25 JOY

---

## 37. Numerical Summary

| Metric | Value |
|--------|-------|
| Total Pokémon | 1,025 |
| Generations | 9 |
| Types | 18 |
| Starter options | 27 (9 regions × 3) |
| Evolution rules | 481 |
| Cross-gen evolutions | 59 |
| Regional forms | 59 |
| Mega Evolutions | 48 |
| Gigantamax forms | 32 |
| Gym Leaders | 72 |
| Elite Four members | 36 |
| Champions | 9 |
| Total trainer battles | 117 |
| Trainer Pokémon | 426 |
| Unique trainer species | 321 |
| Minigames | 6 (Ball, Catch, Memo, Clean, Type, Quiz) + Training Bag |
| Quiz modes | 3 × 4 difficulties |
| Moves | 180 (10 per type × 18 types) |
| Poké Ball types | 10 |
| Accessories | 32 |
| Achievements | 51 |
| Berry types | 12 |
| Garden slots | 6 |
| Natures | 25 |
| Party slots | 6 |
| Box capacity | 960 |
| Photo backgrounds | 8 |
| Sticker types | 12 |
| Photo frames | 8 |
| SFX | 39 synthesized + 1,025 real cries |
| NVS usage | ~600 bytes total |
| SD card usage | ~300 MB (sprites + cries + photos) |
| Languages | 6 |
| Display | 466×466 AMOLED |

---

## 38. Party & Box System

### Party (6 Slots)
- **Slot 0**: always the active Pokémon (shown on main screen, receives full care)
- **Slots 1–5**: reserve party members
- Swap active by tapping a party member on the party card page
- **Seamless migration**: first boot auto-imports current pet into slot 0

### Active vs Inactive
| Aspect | Active (Slot 0) | Inactive (Slots 1–5) |
|--------|-----------------|---------------------|
| XP gain | 100% (1 level/60 min) | 25% (1 level/240 min) |
| Stat decay | Full (§3 rates) | Slow (joy −1/4min, food −1/8min, floor 30) |
| Bond gain | Normal | +1 per 120 minutes |
| Poops | Yes | No |
| Care slip-ups | Yes | No |
| Run-away risk | Yes | No |
| Evolution | Yes (CTA button) | No (queued until swapped in) |
| Shown on screen | Yes | No |

### Box (SD Card Storage)
- Up to **960 Pokémon** stored on SD card (`/box/` directory)
- **Index file** (`index.bin`): 4-byte header + 8 bytes per entry for fast grid browsing
- Individual `.mon` files (48 bytes each) for full Pokémon data
- **4 sort modes**: by dex number, by level, by type, newest first
- Swap-with-last deletion (O(1), no gaps)
- Deposit from party → box, withdraw from box → party

### StoredMon Format (48 bytes)
Preserves: species, level, age, all 4 care stats, bond, weight, poops, shiny flag, sleeping, berryKnown, genes (ATK/DEF/SPE), training (ATK/DEF/SPE), nature, tera type, medals, battle record (wins/losses/streak), and 11-character nickname.

### NVS Persistence
- Party: 6 × 48 bytes = **290 bytes** in NVS namespace `tamaparty`
- Box: stored entirely on SD card (0 NVS bytes)

---

## 39. Move System (180 Moves)

### Overview
- **180 moves** total: 10 per type × 18 types
- Each move has: name, type, category (Physical/Special/Status), power, accuracy, PP, status effect, priority, and special flags
- **Replaces** the generic Attack button in battle with a **4-move picker**
- Dodge and Rest remain as separate battle actions

### Move Categories
| Category | Description | Stat Used |
|----------|-------------|-----------|
| Physical | Contact/melee attacks | ATK vs DEF |
| Special | Ranged/energy attacks | ATK vs DEF (simplified) |
| Status | No damage, applies effects | — |

### STAB (Same Type Attack Bonus)
- When move type matches attacker's type: **+50% damage**
- Works with both primary and secondary types
- Works with Tera Type when Terastallized (§7)

### Status Effects (6 Types)
| Status | Effect | Duration |
|--------|--------|----------|
| Burn | −25% ATK, 6% max HP DOT/turn | Until battle ends |
| Paralyze | −50% SPE, 25% chance skip turn | Until battle ends |
| Poison | 8% max HP DOT/turn | Until battle ends |
| Sleep | Skip turns, wakes on hit | 1–3 turns |
| Confuse | 33% chance to hit self | 2–5 turns |
| Flinch | Skip next turn | 1 turn only |

### Move Flags
| Flag | Effect |
|------|--------|
| Contact | Makes physical contact (trigger contact abilities) |
| Sound | Sound-based (bypasses some defenses) |
| Recoil | User takes 25% of damage dealt |
| Heal | User heals 50% of damage dealt |
| Multi-Hit | Hits 2–5 times |
| High Crit | 2× critical hit rate (12.5% vs 6.25%) |
| Self Buff | Raises user's ATK or DEF by 1 stage |
| Lower DEF | Lowers target's DEF by 1 stage |

### Stat Stages
- Each combatant has ATK/DEF/SPE stages (−6 to +6)
- Stage multipliers: −6=25%, −3=40%, 0=100%, +3=250%, +6=400%
- Reset at battle end

### Learnset System
- **At hatch**: 2–3 moves (type1 basic + type2 basic + Normal utility if level ≥ 5)
- **Every 5 levels**: new move offered from type pool (level-appropriate power)
- **Level 30+**: cross-type coverage moves available (Ice, Ground, Fighting, etc.)
- **4-move cap**: when a 5th move is learned, player chooses which to forget
- PP restored partially after battle win (50%), fully by Rest or item

### AI Move Selection
- Opponents prefer super-effective moves against your type
- Small random factor so AI isn't perfectly predictable
- Higher difficulty (gym leaders, E4) = smarter selection bias

### Example Moves by Type
| Type | Basic (Power ≤ 60) | Mid (70–90) | Strong (100+) |
|------|-------------------|-------------|---------------|
| Fire | Ember (40), Flame Charge (50) | Fire Punch (75), Flamethrower (90) | Fire Blast (110), Overheat (130) |
| Water | Water Gun (40), Aqua Jet (40) | Scald (80), Surf (90) | Hydro Pump (110) |
| Electric | Thunder Shock (40), Nuzzle (20) | Thunderbolt (90), Discharge (80) | Thunder (110) |
| Grass | Vine Whip (45), Bullet Seed (25) | Seed Bomb (80), Energy Ball (90) | Solar Beam (120) |
| Dragon | Dragon Rage (60), Breaking Swipe (60) | Dragon Claw (80), Dragon Pulse (85) | Outrage (120), Draco Meteor (130) |
| Fairy | Fairy Wind (40), Baby-Doll Eyes (0) | Dazzling Gleam (80), Play Rough (90) | Moonblast (95) |

### Persistence
- Active pet's moveset: **8 bytes NVS** (4 slots × 2 bytes: moveId + ppCurrent)

---

## 40. Poké Ball Types (10 Balls)

### Ball Table
| Ball | Catch Modifier | Special Effect | Source |
|------|---------------|----------------|--------|
| Poké Ball | 1.0× | Free, unlimited | Always available |
| Great Ball | 1.5× | — | Expeditions, daily goals |
| Ultra Ball | 2.0× | — | Gym badge reward (2 per badge) |
| Net Ball | 3.0× vs Water/Bug | — | Expeditions (ocean biome) |
| Dusk Ball | 3.0× at night | Active 18:00–05:59 | Expeditions (cave biome) |
| Timer Ball | +0.3× per round (max 4.0×) | Best for long battles | Expedition drop |
| Quick Ball | 4.0× round 1, then 1.0× | Use first throw | Rare expedition |
| Luxury Ball | 1.0× | Caught Pokémon starts +10 bond | Gym reward (every 4th badge) |
| Premier Ball | 1.0× | 5% chance caught Pokémon becomes shiny | Step milestone (5K), special events |
| Master Ball | Guaranteed catch | 100% — no RNG | Champion reward (1 per first victory) |

### Catch Formula
```
rate = 50 + (petLevel − wildLevel) × 2 + (100 − wildHpPct) / 3
rate = rate × ballModifier / 100
rate = clamp(rate, 5, 99)   // Master Ball bypasses (always 100)
```

### Critical Catch
- **4% chance** on any throw (except Master Ball)
- Single shake animation → instant capture
- Visually dramatic: gold flash + "CRITICAL CATCH!" text

### Shake Animation
- Each catch attempt shows 0–3 shakes before success/failure
- Each shake is an independent check against catch rate
- 3 successful shakes + final check = caught

### Inventory
- Max **99** per ball type (Poké Ball always unlimited)
- 10 bytes NVS (1 byte per ball type)
- Starter gift: 5 Great Balls
- Ball selector wheel shown during catch phase in battle

### Ball Drop Sources
| Source | Balls Dropped |
|--------|--------------|
| Expedition (20% chance) | Great, Net, Dusk, Timer, Quick (weighted) |
| Gym badge | 2 Ultra Balls |
| Every 4th badge | +1 Luxury Ball |
| First Champion victory | 1 Master Ball |
| Daily goal completion | 1 Great Ball |
| Step milestone (5K) | 1 Premier Ball |

---

## 41. Photo Mode

### Scene Composition
- **8 procedural pixel-art backgrounds**: Meadow, Beach, Forest, Volcano, Cave, Snow Mountain, City Night, Space
- **12 sticker types**: Heart, Star, Musical Note, Pokéball, Badge, Sparkle, Lightning, Flame, Droplet, Leaf, Moon, Sun
- **8 decorative frames**: None, Simple Border, Pokédex Frame, Gold Ornate, Pixel Checkerboard, Hearts, Stars, Rainbow
- Up to **8 stickers** placed per photo
- **Drag-to-position** Pokémon anywhere on the 466×466 canvas
- Equipped accessories render on the Pokémon in photo mode

### Toolbar
5 tools across the bottom bar:
1. **BG**: cycle through 8 backgrounds (tap to advance)
2. **Frame**: cycle through 8 frames
3. **Sticker**: tap to place stickers, cycles through types
4. **Move**: drag Pokémon to desired position
5. **SNAP!**: capture and save to SD card

### Export
- Saved as **466×466 24-bit BMP** to `/photos/` on SD card
- ~652 KB per photo (uncompressed)
- Up to **50 photos** saved
- Filename: `photo_001.bmp` through `photo_050.bmp`
- Reads directly from the PSRAM framebuffer, converts RGB565 → BGR24

### Gallery
- Card page showing saved photos in a **4-column scrollable grid**
- Tap to view full screen
- Long-press to delete
- Photo count shown at top

### Backgrounds Detail
| Background | Key Elements |
|------------|-------------|
| Meadow | Sky gradient, green ground, flowers, clouds |
| Beach | Sky, sun, ocean with wave lines, sand with shells |
| Forest | Dark canopy, tree trunks, mushrooms, light rays |
| Volcano | Red sky, lava flows, smoke, rocky ground |
| Cave | Dark void, lit center, stalactites/stalagmites, crystals |
| Snow | Light sky, mountain peaks, pine trees, snowflakes |
| City | Night sky, stars, crescent moon, lit buildings, street |
| Space | Star field, nebula, planet with rings, Earth in corner |
