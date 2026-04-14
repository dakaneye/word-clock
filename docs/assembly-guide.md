# Word Clock Assembly Guide

## What You Have

- Wooden frame with LED board inside
- ~100 white LEDs with 470 ohm resistors, soldered into the board
- Common cathode bus wire running across all LEDs (connects all cathode legs to GND)
- Each word's LEDs share an anode (+) connection with a wire exiting the board
- Rainbow LEDs for the birthday words (HAPPY BIRTH DAY CHELSEA), daisy-chained as one group
- Button wires: attached to the physical buttons in the frame
- AM/PM: have wires, dangling free
- Arduino Mega 2560
- DS3231 RTC module
- 6V wall adapter with barrel connector

## Tools You Need

- Soldering iron + solder
- Wire strippers
- Multimeter (essential for identifying words and debugging)
- Masking tape + sharpie (for labeling)

## How It Works

**The LED board is wired common cathode.** All LED cathode (-) legs are tied together by the bus wire. This bus connects to Arduino GND and stays connected permanently.

Each word group's LED anode (+) legs are tied together with a separate wire. To light a word, the Arduino sets its digital pin HIGH, sending +5V through the wire to the word's anodes. Current flows through the LEDs to the bus (GND), and the word lights up.

**No MOSFET modules, shift registers, or external drivers are needed.** The Arduino's digital pins source enough current (~4.3mA per LED through the 470 ohm resistors) to drive the LEDs directly.

```
Arduino pin (HIGH = +5V) → word anode wire → LED anode → LED → LED cathode → bus wire → Arduino GND
```

## Phase 1: Identify and Label Word Groups

Each word on the front panel has a cluster of LEDs behind it. You need to figure out which wire maps to which word.

### Setup

1. Connect the Arduino to your computer via USB
2. Run a jumper wire from the Arduino's **GND** pin to the bus wire on the LED board
3. Run a jumper wire from the Arduino's **5V** pin — this is your probe

### Identify each word

1. Touch the 5V probe to one of the word anode wires
2. Look at the front panel to see which word lit up
3. Label the wire with masking tape and a sharpie
4. Repeat for all word groups

The 25 groups are:

```
Row 1:  IT IS, TEN (minutes), HALF
Row 2:  QUARTER, TWENTY
Row 3:  FIVE (minutes), MINUTES
Row 4:  HAPPY/BIRTH/DAY/CHELSEA (birthday group), PAST, TO
Row 5:  ONE, THREE
Row 6:  ELEVEN, FOUR
Row 7:  TWO, EIGHT, SEVEN
Row 8:  NINE, SIX, TWELVE
Row 9:  FIVE (hours)
Row 10: TEN (hours), O'CLOCK, AM, PM
```

Note: the 4 birthday words (HAPPY, BIRTH, DAY, CHELSEA) are wired together as one group — they all light from a single wire.

### Verify your count

You should have 25 labeled wires: 24 time words + 1 birthday group. AM and PM may already have their own pre-attached wires.

---

## Phase 2: Wire Words to Arduino Pins

Connect each labeled word wire to its assigned Arduino pin:

| Pin | Word | Pin | Word |
|-----|------|-----|------|
| 2 | IT IS | 22 | ONE |
| 3 | TEN (minutes) | 23 | ELEVEN |
| 4 | HALF | 24 | FOUR |
| 5 | QUARTER | 26 | TWO |
| 6 | TWENTY | 27 | EIGHT |
| 7 | FIVE (minutes) | 28 | SEVEN |
| 8 | MINUTES | 29 | NINE |
| 9 | PAST | 30 | SIX |
| 10 | TO | 31 | TWELVE |
| 11 | BIRTHDAY | 33 | FIVE (hours) |
| 13 | THREE | 34 | TEN (hours) |
| | | 35 | O'CLOCK |
| | | 36 | AM |
| | | 37 | PM |

Connect the bus wire to any Arduino **GND** pin.

**Note:** Pin 13 (THREE) shares the Arduino's onboard LED. THREE flashes briefly on startup — this is normal and cosmetic.

---

## Phase 3: Wire the DS3231 RTC

| RTC Pin | Arduino Mega Pin |
|---------|-----------------|
| VCC | 5V |
| GND | GND |
| SDA | Pin 20 (SDA) |
| SCL | Pin 21 (SCL) |

---

## Phase 4: Wire the Buttons

Each button needs 2 wires: one signal wire to an Arduino pin, one wire to GND. The Arduino uses internal pullup resistors.

If your buttons have more than 2 wires (illuminated buttons have extra wires for the LED), use a multimeter in continuity mode to find the switch pair: the two wires that beep only when the button is pressed.

| Button | Arduino Pin | Other wire |
|--------|-------------|------------|
| Hour | Pin 38 | GND |
| Minute | Pin 39 | GND |

---

## Phase 5: Upload and Test

```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega word-clock
```

The clock should display the current time in words. Use buttons to set the correct time:
- **Hour button** — advances 1 hour per press, auto-repeats when held
- **Minute button** — advances 5 minutes per press, auto-repeats when held

### If something is wrong

**No LEDs light up:**
- Is the bus wire connected to Arduino GND?
- Is the Arduino powered (USB or barrel jack)?

**Wrong word lights up for a given time:**
- The pin assignment is wrong. Note which word lit for which pin, then either swap the wires or update `config.cpp`

**All LEDs blink on and off in a loop:**
- The DS3231 RTC was not found. Check SDA (pin 20) and SCL (pin 21) wiring

**THREE flashes briefly on startup:**
- Normal. Pin 13 is the Arduino's onboard LED pin

**Time doesn't advance:**
- Check the RTC module. The DS3231's coin cell battery may be dead — replace the CR2032

---

## Phase 6: Permanent Wiring

Once everything works with temporary connections:

1. Solder each word wire to its anode lead on the LED board
2. Cut wires to clean lengths with ~2" of slack
3. Route wires through the frame
4. Mount the Arduino inside the frame (screws, standoffs, or double-sided tape)
5. Connect the power cable (USB wall charger or 6V adapter through the barrel jack)
6. Slide the back panel into the frame grooves
7. Hang it up

## Power Options

| Source | Notes |
|--------|-------|
| **USB wall charger** | Cleanest. Any phone charger works. 500mA is sufficient. |
| **6V wall adapter + barrel jack** | Works through the Arduino's onboard voltage regulator. Marginal but functional. |
| **7-12V adapter + barrel jack** | Ideal voltage range for the barrel jack regulator. |

Do not feed more than 5.5V directly to the Arduino's 5V pin — it bypasses the regulator and can damage the microcontroller.
