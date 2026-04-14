# Customizing Your Word Clock

This project was built as a birthday gift for Chelsea. If you're building your own, here's what to change.

## Quick: Change the Birthday Date

Edit `word-clock/config.h`:

```cpp
const int BIRTHDAY_MONTH = 5;  // 1=Jan, 12=Dec
const int BIRTHDAY_DAY = 4;
```

## Change the Birthday Words

The birthday words (HAPPY BIRTH DAY CHELSEA) are physically wired as a single daisy-chained LED group controlled by one Arduino pin (`W_BIRTHDAY`). Changing the name on the clock face requires changing the laser-cut front panel — the software just turns the group on or off as a unit.

If your build has birthday words wired as **separate groups** (one wire per word), add separate enum values in `config.h`, update `WORD_PINS` in `config.cpp`, and update `birthdayWords()` in `birthday.cpp` to include all of them.

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

## Wiring

This build uses **common cathode** wiring:

- A shared bus wire connects all LED cathodes to Arduino GND
- Each word group has its own anode wire connected to an Arduino Mega digital pin
- Arduino pin HIGH = word on, LOW = word off
- No MOSFET modules, shift registers, or external driver ICs needed

Each LED has a 470 ohm series resistor. At 5V, each LED draws ~4.3mA. The Arduino Mega can handle up to ~30mA per pin (words with up to 7 LEDs).

**Important:** verify your board's LED polarity before wiring. This build was originally documented as "common anode" (which was wrong) — the error wasn't caught until LEDs were tested directly with a multimeter. Always probe a single LED with the multimeter's diode test to confirm which leg is anode and which is cathode before connecting anything.

## Power

The Arduino is powered via its barrel jack from a 6V wall adapter. The onboard voltage regulator drops this to ~5V for the digital pins. USB power also works (500mA is sufficient for ~200mA peak LED draw).

No external power supply is needed for the LEDs — the Arduino's digital pins source current directly.

## Hardware Alternatives

| Approach | Pros | Cons |
|----------|------|------|
| **Arduino Mega + direct pin drive** (this build) | Simplest wiring, fewest components | Limited to ~30mA per pin, requires Mega for pin count |
| **Arduino Mega + P-channel MOSFET modules** | Higher current capacity per word | More components, more wiring |
| **Arduino UNO + TPIC6B595 chips** | Fewer wires (3 to Arduino) | Must solder ICs onto perfboard |
| **ESP32 + direct drive** | WiFi for NTP time sync (no RTC needed) | Different pin voltages, more complex software |

The software architecture works with any approach — only `config.h`, `config.cpp`, and `display.cpp` need changes. The time-to-words logic is hardware-independent.
