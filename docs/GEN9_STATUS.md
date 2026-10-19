# Gen 9 feature set: status and test guide

All new behaviour is behind build flags that are **off by default**, so a normal build behaves exactly like before.
Enable them by adding defines to the compiler flags, for example with arduino-cli:

    --build-property "compiler.cpp.extra_flags=-DTAMAPOKE_GEN3_RELEASE -DTAMAPOKE_NATURES -DTAMAPOKE_BALLS -DTAMAPOKE_MOVES -DTAMAPOKE_EXTRAS"

| Flag | What it turns on |
|------|------------------|
| `TAMAPOKE_NATURES` | 25 natures: assigned at hatch, change stats and care rates, shown on the personality card |
| `TAMAPOKE_BALLS` | Poke Ball stock (saved), best ball used automatically when catching, Master Ball needed for legendaries |
| `TAMAPOKE_MOVES` | Starting moves at hatch (saved), shown on the Bag page. The move battle engine exists but the battle screen does not use it yet |
| `TAMAPOKE_EXTRAS` | Card pages 10-15 (Trainer, Garden, Bag, Achievements, Accessories, Quiz), gym / Elite Four / Champion fights, achievements, daily seed, legendary reward egg, seasonal shiny boost |

## Where things are

Logic modules (host-tested, no hardware): `pokeballs`, `natures`, `garden`, `moves`, `movebattle`, `gyms`,
`expedition`, `tradeevo`, `regional`, `gimmicks`, `seasonal`, `dexbits`, `egg`, `steps`, `achievements`,
`accessories`, `quiz`, `party`, `photo`, `blenotify`, `watchface`, `extras`, `dex_gen9.h`/`dexall.h` (#387-1025 data).
UI for the extras pages and trainer fights: `extras_ui.h`. Run the unit tests with `cd tests && make test`.

## Not wired into the firmware yet

* Move-based battle screen (engine is done: `movebattle.*`)
* Gen 4-9 species in the game: `DEX_COUNT` is still 386; sprites for 1-1025 can be built (see below)
* Mega / Dynamax / Tera (`gimmicks`), regional forms, trade evolutions, Wonder Trade, PvP, party/box, photo mode,
  BLE notifications, watch face, `dexbits` 1025 bitmaps, merged expedition rewards (accessory drops)
* Accessories are collected and equipped but not drawn on the pet
* Rematch tiers only change levels; Elite Four healing is a flat 20% between members
* The 5th card page indicator: with all flags there are 15 card pages

## Sprites

PMD SpriteCollab (CC BY-NC 4.0, credited in CREDITS.md) covers most species; build with
`python3 tools/pack_pmd.py $(seq 387 1025)`. Species it lacks can be filled with static frames using
`python3 tools/pack_static.py` (PokeAPI/sprites: **Nintendo artwork, no open licence, do not commit**).
Then `python3 tools/make_thumbs.py --max 1025` and `python3 tools/pack_bundle.py --gen9`.

## Things to verify by hand (no one has run this on a device yet)

Please open an issue for anything that fails. Include firmware flags, what you tapped and what you saw.

1. Default build (no flags) boots and plays exactly as before.
2. Flags `NATURES`: new egg hatches with a nature; personality card shows its name; old saves load as Hardy.
3. `BALLS`: after a won wild battle, Catch uses a ball; Bag page counts drop; Great Balls run out after 5, then Poke Ball is used.
4. `EXTRAS` Trainer page: swipe to page 10; region arrows work; FIGHT is greyed below the level requirement; a won gym increases Badges.
5. Trainer fights: members fight in sequence, HP carries over, OK advances, fleeing cancels, a loss gives no badge.
6. Garden: plant a seed, water (1 h cooldown), mulch (right side of the row), harvest when READY, feed R/B/G berries.
7. Achievements list scrolls; unlocking plays a sound; Accessories toggle ON/OFF.
8. Quiz: 10 rounds, reward after the last one.
9. Save and reboot after each step: everything above must persist.
10. Language switch: new pages show translated labels. Non-English natures and labels were written from memory and may need correction.
