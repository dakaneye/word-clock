# Customizing Your Word Clock

This project was built as a birthday gift for Chelsea. If you're building your own, here's what to change.

## Quick: Change the Name and Birthday

**1. Edit `word-clock/config.h`:**

Change these at the top of the file:

```cpp
#define W_NAME W_CHELSEA   // Change to your recipient's enum value
const int BIRTHDAY_MONTH = 5;  // 1=Jan, 12=Dec
const int BIRTHDAY_DAY = 4;
```

**2. Rename the enum value:**

In the same file, find `W_CHELSEA` in the enum and rename it:

```cpp
// Before
W_CHELSEA,  // Replace with your recipient's name

// After
W_ALEX,     // Your recipient's name
```

Then update `#define W_NAME` to match:
```cpp
#define W_NAME W_ALEX
```

**3. Update `word-clock/config.cpp`:**

Find the line with `W_CHELSEA` in the pin comment and rename it:

```cpp
// Before
32,  // W_CHELSEA    — Module 6, CH3

// After
32,  // W_ALEX       — Module 6, CH3
```

**4. Update test sketches** (`test-sketches/pin-sweep/pin-sweep.ino` and `test-sketches/word-test/word-test.ino`):

Find `"CHELSEA"` / `"chelsea"` in the `WORD_NAMES` arrays and replace with your name.

**5. Update `test/test_time_to_words.cpp`:**

Find `"CHELSEA"` in `WORD_NAMES` and the birthday test, rename to match.

That's it. Recompile and all references update: `cd test && make test` then `arduino-cli compile --fqbn arduino:avr:mega word-clock`.

## Design Your Own Front Panel

The front panel is a grid of letters where words are formed by lighting specific LEDs behind each letter. This build uses a 10x13 grid:

```
I T R I S C T E N H A L F
Q U A R T E R T W E N T Y
F I V E C M I N U T E S H
N H A P P Y P A S T T O D
O N E B I R T H T H R E E
E L E V E N F O U R D A Y
T W O E I G H T S E V E N
N I N E S I X T W E L V E
C H E L S E A R W F I V E
T E N O C L O C K A M P M
```

To design your own:

1. **Start with the time words** — these are fixed for any English word clock: IT IS, FIVE, TEN, QUARTER, TWENTY, HALF, MINUTES, PAST, TO, ONE through TWELVE, O'CLOCK, AM, PM
2. **Add your custom words** — a name, birthday message, or anything else
3. **Fill gaps with random letters** — unused positions get random letters so the face looks like a grid of characters, not words with gaps
4. **Use a monospaced stencil font** for laser cutting (each letter must be >1mm thick to survive cutting)

The original was laser-cut on [ponoko.com](https://www.ponoko.com) using 12"x12"x1/8" birch.

### If you change the word layout:

1. Update the `Word` enum in `config.h` to match your words
2. Update `WORD_PINS` in `config.cpp` with the correct pin for each word
3. Update `time_to_words.cpp` if you change any time-related words
4. Update `birthday.cpp` if you change the birthday message words
5. Update `WORD_NAMES` arrays in the test sketches
6. Run the pin sweep test to verify your wiring matches

## Hardware Alternatives

This build uses an Arduino Mega + 7 MOSFET modules because it minimizes soldering. Other approaches that work:

| Approach | Pros | Cons |
|----------|------|------|
| **Arduino Mega + MOSFET modules** (this build) | No IC soldering, easy debugging | More expensive (~$94), more wires |
| **Arduino UNO + TPIC6B595 chips** | Cheaper (~$30), fewer wires (3 to Arduino) | Must solder ICs onto perfboard |
| **Arduino UNO + shift registers + darlington arrays** | Cheapest (~$20), classic design | Most soldering, most failure points |
| **ESP32 + MOSFET modules** | WiFi for NTP time sync (no RTC needed) | More complex software, different pin voltages |

The software architecture works with any approach — only `config.h`, `config.cpp`, and `display.cpp` need changes. The time-to-words logic is hardware-independent.

## Power Sizing

Each white LED draws ~15-20mA through its 470 ohm resistor. The maximum simultaneous draw is about 7 words lit at once (e.g., "IT IS TWENTY FIVE MINUTES PAST ELEVEN AM" = ~35 LEDs = ~600mA). A 5V 2A supply is sufficient. This build uses a 5V 4A supply for headroom.
