# Word Clock Assembly Guide

## What You Have Right Now

- Wooden frame with LED board inside
- ~100 white LEDs with resistors, soldered into the board
- Common anode (+5V) bus wire running across all LEDs (the thick wire connecting everything — DO NOT CUT)
- Each word's LEDs share a cathode (-) connection, but those wires are currently not connected to anything
- Button wires: attached to the physical buttons in the frame, dangling free on the other end
- AM/PM: have wires, dangling free
- Birthday word LEDs: wired together in their groups
- Arduino Mega (new, in box)
- DS3231 RTC module (existing, tested working)
- 7x NOYITO 4-Channel MOSFET modules (new, in box)
- 5V 4A power supply (existing)
- Jumper wires, hookup wire, wire strippers (new)

## Tools You Need

- Soldering iron + solder
- Wire strippers (ordered)
- Small phillips screwdriver (for MOSFET module screw terminals)
- Masking tape + sharpie (for labeling)

## Before You Start

Take a photo of the back of the board as-is. You'll want to compare later if something isn't working.

---

## Phase 1: Identify Word Groups (no soldering)

Each word on the front panel has a cluster of LEDs behind it. Each cluster shares a cathode wire. You need to figure out which wire goes to which word.

1. Look at the front panel. Count the words you can see
2. Look at the back. Each cluster of LED legs should be grouped
3. The common anode bus connects all the positive (+) leads — that's the thick wire running across the board
4. The cathode (-) side of each word group either has an existing wire or bare leads

**Label each word group.** Use masking tape and sharpie. Write the word name on a piece of tape and stick it near the cathode wire/leads for that group. Go through the grid row by row:

```
Row 1:  IT IS, TEN, HALF
Row 2:  QUARTER, TWENTY
Row 3:  FIVE (minutes), MINUTES
Row 4:  HAPPY, PAST, TO
Row 5:  ONE, BIRTH, THREE
Row 6:  ELEVEN, FOUR, DAY
Row 7:  TWO, EIGHT, SEVEN
Row 8:  NINE, SIX, TWELVE
Row 9:  CHELSEA, FIVE (hours)
Row 10: TEN (hours), O'CLOCK, AM, PM
```

Some groups already have wires (you mentioned AM/PM and birthday). Some may just have bare LED leads that need a wire soldered on.

**If you can't tell which LEDs belong to which word:** Hold the board up to the front panel and look through from behind. The LED positions map directly to the letter positions on the stencil.

---

## Phase 2: Wire Cathode Leads (soldering)

For each word group that doesn't already have a cathode wire:

1. Strip about 1/2 inch of insulation off one end of a piece of hookup wire
2. Solder it to the cathode leads of that word group (all the cathode leads for one word connect together)
3. Leave about 6-8 inches of wire — long enough to reach a MOSFET module mounted nearby
4. Use different color wires for different zones if possible (helps with debugging)

**For groups that already have wires** (AM/PM, birthday words, buttons): leave them as-is. They just need to be connected to the right place later.

When done, you should have 27 labeled wires hanging off the back of the board, plus 2 button wires. Plus the common anode bus.

---

## Phase 3: Mount MOSFET Modules

The 7 MOSFET modules mount on the back panel near the words they control. Each module is about 2.6" x 2.6".

Use small screws, standoffs, or double-sided tape to mount them. They don't carry mechanical load — they just need to stay in place.

**Module placement (viewed from back of board):**

```
┌─────────────────────────────────┐
│  [Module 1]         [Module 2]  │  ← Top
│  IT IS,TEN,         TWENTY,FIVE │
│  HALF,QUARTER       MIN,MINUTES │
│                     PAST        │
│       [Module 3]                │  ← Mid-upper
│       TO,HAPPY,                 │
│       BIRTH,THREE               │
│                                 │
│  [Module 4]         [Module 5]  │  ← Middle
│  ONE,ELEVEN,        TWO,EIGHT,  │
│  FOUR,DAY           SEVEN,NINE  │
│                                 │
│  [Module 6]                     │  ← Lower
│  SIX,TWELVE,                    │
│  CHELSEA,FIVE(hr)               │
│                                 │
│  [Module 7]    [Arduino Mega]   │  ← Bottom
│  TEN(hr),OCLOCK,  + RTC         │
│  AM,PM                          │
└─────────────────────────────────┘
```

Don't mount the Arduino Mega yet — do that in Phase 5 after testing.

---

## Phase 4: Connect Word Wires to MOSFET Screw Terminals

Each MOSFET module has 4 channels. Each channel has a screw terminal on the output side (where the load connects).

**For each channel:** loosen the screw terminal, insert the cathode wire for that word, tighten the screw.

### Module 1 — Top (rows 1-2)
| Channel | Word | Notes |
|---------|------|-------|
| CH1 | IT IS | |
| CH2 | TEN (minutes) | Row 1 — the "TEN" between "IS" and "HALF" |
| CH3 | HALF | |
| CH4 | QUARTER | |

### Module 2 — Upper (rows 2-4)
| Channel | Word | Notes |
|---------|------|-------|
| CH1 | TWENTY | |
| CH2 | FIVE (minutes) | Row 3 — left side, not row 9 |
| CH3 | MINUTES | |
| CH4 | PAST | |

### Module 3 — Mid-upper (rows 4-5)
| Channel | Word | Notes |
|---------|------|-------|
| CH1 | TO | |
| CH2 | HAPPY | Birthday word |
| CH3 | BIRTH | Birthday word |
| CH4 | THREE | |

### Module 4 — Middle (rows 5-6)
| Channel | Word | Notes |
|---------|------|-------|
| CH1 | ONE | |
| CH2 | ELEVEN | |
| CH3 | FOUR | |
| CH4 | DAY | Birthday word — end of row 6 |

### Module 5 — Mid-lower (rows 7-8)
| Channel | Word | Notes |
|---------|------|-------|
| CH1 | TWO | |
| CH2 | EIGHT | |
| CH3 | SEVEN | |
| CH4 | NINE | |

### Module 6 — Lower (rows 8-9)
| Channel | Word | Notes |
|---------|------|-------|
| CH1 | SIX | |
| CH2 | TWELVE | |
| CH3 | CHELSEA | Birthday word |
| CH4 | FIVE (hours) | Row 9 — right side |

### Module 7 — Bottom (row 10)
| Channel | Word | Notes |
|---------|------|-------|
| CH1 | TEN (hours) | Row 10 — leftmost |
| CH2 | O'CLOCK | |
| CH3 | AM | |
| CH4 | PM | |

---

## Phase 5: Wire MOSFET Modules to Arduino Mega

Each MOSFET module has input pins on the logic side (the side without screw terminals). These connect to the Arduino Mega with jumper wires.

**Each module needs:**
- **VCC** → Arduino 5V
- **GND** → Arduino GND
- **CH1-CH4 input** → specific Arduino digital pin

Use male-to-female jumper wires for this.

### Power connections first

All 7 modules need 5V and GND from the Arduino. You can daisy-chain: run 5V from the Arduino to Module 1, then Module 1 to Module 2, etc. Same for GND.

### Signal connections

| Module | CH1 Pin | CH2 Pin | CH3 Pin | CH4 Pin |
|--------|---------|---------|---------|---------|
| 1 | 2 | 3 | 4 | 5 |
| 2 | 6 | 7 | 8 | 9 |
| 3 | 10 | 11 | 12 | 13 |
| 4 | 22 | 23 | 24 | 25 |
| 5 | 26 | 27 | 28 | 29 |
| 6 | 30 | 31 | 32 | 33 |
| 7 | 34 | 35 | 36 | 37 |

---

## Phase 6: Wire the DS3231 RTC

4 jumper wires from the RTC module to the Arduino Mega:

| RTC Pin | Arduino Mega Pin |
|---------|-----------------|
| VCC | 5V |
| GND | GND |
| SDA | Pin 20 (SDA) |
| SCL | Pin 21 (SCL) |

---

## Phase 7: Wire the Buttons

Each button needs 2 connections: one wire to an Arduino pin, one wire to GND. The Arduino uses internal pullup resistors, so no external resistor is needed.

| Button | Arduino Pin | Other wire |
|--------|-------------|------------|
| Hour | Pin 38 | GND |
| Minute | Pin 39 | GND |

---

## Phase 8: Wire the Power Supply

The LEDs need external 5V from the power supply, not from the Arduino.

1. Connect the power supply's +5V wire to the common anode bus on the LED board
2. Connect the power supply's GND wire to the Arduino's GND (common ground is essential)
3. The Arduino itself is powered separately via USB during testing, or via its barrel jack for permanent use

**Put the 1000uF capacitor across the power supply's +5V and GND** (either polarity for electrolytic — the longer leg is + and goes to +5V). This absorbs voltage spikes when the supply turns on.

---

## Phase 9: Test (before closing the frame)

Follow this order. Upload each test sketch from your computer via USB.

### Test 1: RTC
```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega test-sketches/rtc-test
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=9600
```
Expected: Time prints every second. If "ERROR: DS3231 not found" — check SDA/SCL wiring.

### Test 2: Pin Sweep
Plug in the 5V power supply (LEDs need external power).
```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega test-sketches/pin-sweep
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=9600
```
Watch the clock face. Each word should light up for 2 seconds. The serial monitor shows which word is expected. **Write down any mismatches** — we'll fix pin assignments in config.cpp.

### Test 3: Word Test
```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega test-sketches/word-test
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=9600
```
Type word names to light them individually. Type "all" to verify every LED works. Type "off" to clear.

### Test 4: Button Test
```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega test-sketches/button-test
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=9600
```
Press each button. Should see "PRESSED" / "RELEASED" in the monitor.

### Test 5: Full System
```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega word-clock
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=9600
```
The clock should display the current time in words. Use buttons to set the correct time (hour button = +1 hour, minute button = +5 minutes).

---

## Phase 10: Close It Up

Once everything works:

1. Mount the Arduino Mega securely inside the frame (screws, standoffs, or double-sided tape)
2. Switch Arduino power from USB to barrel jack (connect 5V supply to both the LED bus and the Arduino's barrel jack — or use a separate USB power adapter)
3. Route the power cable through the hole in the frame
4. Slide the back panel into the frame grooves
5. Hang it up or set it on a shelf

---

## Troubleshooting

**No LEDs light up at all:**
- Is the 5V power supply plugged in and turned on?
- Is there a common GND between the Arduino and the power supply?
- Check the common anode bus wire — is it connected to +5V from the supply?

**One word doesn't light up:**
- Check the screw terminal — is the cathode wire tight?
- Check the jumper wire from module to Arduino — correct pin?
- Try the word-test sketch and type that word name

**Wrong word lights up:**
- Pin mapping needs adjustment. Note which word lit for which pin during the pin sweep test, then update config.cpp

**RTC shows wrong time:**
- Use the buttons to set it. Hour button advances by 1 hour, minute button advances by 5 minutes
- If the time resets when you unplug: replace the CR2032 battery on the RTC module

**All LEDs blink on and off in a loop:**
- The RTC module wasn't found. Check SDA (pin 20) and SCL (pin 21) wiring
