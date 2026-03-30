# Word Clock — Design Spec

## Overview

Revive a 2015 word clock project for Chelsea (wife, birthday May 4th). The clock displays time in English words on a laser-cut birch panel using white LEDs. A birthday mode activates on May 4th displaying "HAPPY BIRTH DAY CHELSEA".

## Design Principles

1. **Simple** — No clever abstractions. Each file does one thing. Code reads like what it does: "if it's quarter past, turn on QUARTER and PAST." Prefer obvious and verbose over compact and clever.
2. **Well-tested** — Core logic (time → words) tested natively on macOS without hardware. Hardware integration tested incrementally via a progression of test sketches, each validating one layer before moving to the next.
3. **Easy to maintain** — Pin assignments in one config file. If an LED burns out or a MOSFET module dies, swap the part and change one line. No hidden dependencies between modules.

## Hardware

### Existing (keeping)
- Laser-cut birch front panel with word grid (chelsea variant)
- ~100 white 5mm LEDs with 470ohm resistors, soldered into wooden backing board
- Common anode (+5V) bus wiring across all LEDs
- Walnut frame (11" x 11.75" interior, ~2" depth)
- Arduino UNO (used for development/testing only)
- DS3231 RTC module with I2C connection
- 2 push buttons (hour and minute setting)
- 5V 4A wall power supply
- CR2032 battery for RTC backup (may need replacement)

### New (ordered)
- ELEGOO Arduino Mega 2560 R3 (replaces UNO as production brain)
- 7x NOYITO 4-Channel MOSFET Switch Modules (IRF540, screw terminals)
- Dupont jumper wires (M/F, M/M, F/F kit)
- 22 AWG solid core hookup wire (6 colors)
- WGGE wire strippers

### Why Mega + MOSFETs over shift registers
- Zero IC soldering — MOSFET modules are pre-assembled with screw terminals
- One Arduino pin per word — simple `digitalWrite()`, no bit-shifting logic
- Easier to debug — toggle one pin, one word lights up
- Mega has 54 digital pins, we need 27 + 2 buttons + 2 I2C = 31

## Word Grid Layout

Front panel (10 rows x 13 columns):
```
Row 1:  I T R I S C T E N H A L F
Row 2:  Q U A R T E R T W E N T Y
Row 3:  F I V E C M I N U T E S H
Row 4:  N H A P P Y P A S T T O D
Row 5:  O N E B I R T H T H R E E
Row 6:  E L E V E N F O U R D A Y
Row 7:  T W O E I G H T S E V E N
Row 8:  N I N E S I X T W E L V E
Row 9:  C H E L S E A R W F I V E
Row 10: T E N O C L O C K A M P M
```

## 27 Word Groups

### Time display words (23)
| # | Word | Row | Purpose |
|---|------|-----|---------|
| 1 | IT IS | 1 | Always on |
| 2 | TEN (min) | 1 | :10, :50 |
| 3 | HALF | 1 | :30 |
| 4 | QUARTER | 2 | :15, :45 |
| 5 | TWENTY | 2 | :20, :25, :35, :40 |
| 6 | FIVE (min) | 3 | :05, :25, :35, :55 |
| 7 | MINUTES | 3 | :05, :10, :20, :25, :35, :40, :50, :55 |
| 8 | PAST | 4 | :05–:30 |
| 9 | TO | 4 | :35–:55 |
| 10 | ONE | 5 | Hour 1 |
| 11 | TWO | 7 | Hour 2 |
| 12 | THREE | 5 (letters 9-13) | Hour 3 |
| 13 | FOUR | 6 | Hour 4 |
| 14 | FIVE (hr) | 9 | Hour 5 |
| 15 | SIX | 8 | Hour 6 |
| 16 | SEVEN | 7 | Hour 7 |
| 17 | EIGHT | 7 | Hour 8 |
| 18 | NINE | 8 | Hour 9 |
| 19 | TEN (hr) | 10 | Hour 10 |
| 20 | ELEVEN | 6 | Hour 11 |
| 21 | TWELVE | 8 | Hour 12 |
| 22 | O'CLOCK | 10 | :00 |
| 23 | AM | 10 | Morning indicator |

### Shared time + birthday words (1)
| # | Word | Row | Purpose |
|---|------|-----|---------|
| 24 | PM | 10 | Afternoon indicator |

### Birthday words (3)
| # | Word | Row | Purpose |
|---|------|-----|---------|
| 25 | HAPPY | 4 | Birthday mode |
| 26 | BIRTH | 5 (letters 5-9) | Birthday mode |
| 27 | CHELSEA | 9 | Birthday mode |

Note: "BIRTH" and "THREE" share row 5. "DAY" at end of row 6 shares LEDs with "FOUR" + "DAY" — need to verify during pin sweep whether DAY has its own cathode group or is part of another word. Birthday message displays as "HAPPY BIRTH DAY CHELSEA" where DAY uses the row 6 DAY group.

## Pin Assignments

### MOSFET Modules — grouped by physical position on back of board

| Module | Zone | CH1 (pin) | CH2 (pin) | CH3 (pin) | CH4 (pin) |
|--------|------|-----------|-----------|-----------|-----------|
| 1 | Top (rows 1-2) | IT IS (2) | TEN min (3) | HALF (4) | QUARTER (5) |
| 2 | Upper (rows 2-4) | TWENTY (6) | FIVE min (7) | MINUTES (8) | PAST (9) |
| 3 | Mid-upper (rows 4-5) | TO (10) | HAPPY (11) | BIRTH (12) | THREE (13) |
| 4 | Middle (rows 5-6) | ONE (22) | ELEVEN (23) | FOUR (24) | DAY (25) |
| 5 | Mid-lower (rows 7-8) | TWO (26) | EIGHT (27) | SEVEN (28) | NINE (29) |
| 6 | Lower (rows 8-9) | SIX (30) | TWELVE (31) | CHELSEA (32) | FIVE hr (33) |
| 7 | Bottom (row 10) | TEN hr (34) | O'CLOCK (35) | AM (36) | PM (37) |

### Other pins
| Function | Pin |
|----------|-----|
| DS3231 SDA | 20 |
| DS3231 SCL | 21 |
| Hour button | 38 |
| Minute button | 39 |

## Time Display Logic

Time shown in 5-minute intervals. "IT IS" always on. AM/PM shows current period.

| Minute range | Words displayed |
|-------------|-----------------|
| :00 | IT IS [hour] O'CLOCK |
| :01-:04 | IT IS [hour] O'CLOCK |
| :05-:09 | IT IS FIVE MINUTES PAST [hour] |
| :10-:14 | IT IS TEN MINUTES PAST [hour] |
| :15-:19 | IT IS QUARTER PAST [hour] |
| :20-:24 | IT IS TWENTY MINUTES PAST [hour] |
| :25-:29 | IT IS TWENTY FIVE MINUTES PAST [hour] |
| :30-:34 | IT IS HALF PAST [hour] |
| :35-:39 | IT IS TWENTY FIVE MINUTES TO [next hour] |
| :40-:44 | IT IS TWENTY MINUTES TO [next hour] |
| :45-:49 | IT IS QUARTER TO [next hour] |
| :50-:54 | IT IS TEN MINUTES TO [next hour] |
| :55-:59 | IT IS FIVE MINUTES TO [next hour] |

Hour displayed in 12-hour format. For :35-:59, the next hour is shown (e.g., at 2:45 → "QUARTER TO THREE").

## Birthday Mode

- Activates on May 4th (Chelsea's birthday)
- Alternates between time display and "HAPPY BIRTH DAY CHELSEA"
- Cycle: show time for 5 seconds, show birthday message for 5 seconds
- Active all day (midnight to midnight May 4th)

## Button Behavior

- **Hour button (pin 38):** Each press advances the hour by 1 (wraps 12→1). Debounced.
- **Minute button (pin 39):** Each press advances the minute by 5 (wraps 55→00 and increments hour). Debounced.
- Both buttons set the time on the DS3231 RTC so it persists across power cycles.
- Hold detection: holding a button auto-repeats after 500ms at 4 presses/second.

## Software Architecture

```
word-clock/
├── word-clock.ino          # Main sketch — setup + loop
├── time_to_words.h/.cpp    # Pure logic: hour+minute → word set
├── display.h/.cpp          # MOSFET pin control (turn words on/off)
├── clock.h/.cpp            # DS3231 RTC read/write
├── buttons.h/.cpp          # Button handling with debounce + hold-repeat
├── birthday.h/.cpp         # Birthday detection + alternating display
└── config.h                # Pin assignments, word-to-pin mapping
```

### Testability
`time_to_words` is pure logic with zero hardware dependencies. A native test harness compiles and runs on macOS to verify all 144 time→word mappings (12 hours x 12 five-minute intervals) without an Arduino. All other modules depend on hardware and are tested via upload.

### Test sketch progression
1. **Blink test** — verify upload works (done)
2. **RTC test** — verify DS3231 reads time correctly
3. **Pin sweep test** — cycle each MOSFET output one at a time to map words to pins
4. **Word group test** — light up specific words by name via serial commands
5. **Time display test** — show current time in words
6. **Button test** — verify button presses adjust time
7. **Birthday test** — force May 4th date, verify alternating display
8. **Integration** — full system running

## Power

- 5V 4A wall supply powers LEDs via common anode bus
- Arduino Mega powered via USB or barrel jack (separate from LED power)
- Each MOSFET module gets 5V from the supply
- Common ground between Arduino, MOSFET modules, and power supply

## Timeline

- Parts ordered: March 29, 2026
- Parts arrive: ~March 31 - April 2
- Software ready: before parts arrive
- Assembly + wiring: April 2-6
- Testing + debugging: April 6-13
- Buffer: April 13-May 3
- Deadline: May 4, 2026 (Chelsea's birthday)
