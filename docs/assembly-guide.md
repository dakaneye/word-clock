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
- 5V 4A power supply with barrel connector (existing)
- DC barrel jack breakout adapter (to expose +5V and GND wires from the power supply's barrel plug)
- Jumper wires, hookup wire, wire strippers (new)
- Inline fuse holder + 3A fast-blow fuse (for the 5V power line)
- 1000uF electrolytic capacitor (for the power supply — absorbs voltage spikes on startup)

## Tools You Need

- Soldering iron + solder
- Wire strippers
- Small phillips screwdriver (for MOSFET module screw terminals)
- Masking tape + sharpie (for labeling)
- CR2032 coin cell battery (for testing LED groups — you probably have one in the RTC module's packaging or a spare)
- Multimeter (optional but helpful for checking continuity and debugging)

## How This Thing Works (quick primer)

**LEDs have two legs: anode (+) and cathode (-).**
Current flows from anode to cathode. An LED only lights up when + voltage is on the anode side and the cathode side is connected to ground (GND). On a single LED, the longer leg is the anode (+) and the shorter leg is the cathode (-). If the legs have been trimmed (yours have — they're soldered in), look for the flat spot on the LED's plastic housing — that's the cathode (-) side.

**Your LED board is wired "common anode."**
That means all the anode (+) legs are tied together by the thick bus wire running across the board. That bus wire connects to +5V from the power supply. This is always on — it's just providing power to every LED.

**Each word group shares a cathode (-) connection.**
The cathode legs of all LEDs in one word (e.g., the 5 LEDs behind "QUARTER") are soldered together. To light up a word, you need to connect its cathode wire to ground (GND). To turn it off, you disconnect it from ground.

**The MOSFET modules are the switches.**
Each channel on a MOSFET module is a switch that connects a word's cathode wire to GND when the Arduino tells it to. Arduino pin goes HIGH → MOSFET connects cathode to GND → current flows through LEDs → word lights up. Arduino pin goes LOW → MOSFET disconnects → word goes dark.

**So the full circuit for one word is:**
```
+5V (power supply) → bus wire → LED anode (+) → LED → LED cathode (-) → word wire → MOSFET OUT terminal → MOSFET → MOSFET GND terminal → power supply GND
                                                                                                          ↑
                                                                                               Arduino pin controls this switch
```

## Safety Rules

Follow these throughout the entire build:

1. **Always disconnect the power supply AND USB cable before making or changing any wire connections.** Reconnect only for testing. The 5V 4A supply can push enough current through a short circuit to overheat wires or damage components.
2. **Touch a grounded metal surface** before handling the Arduino or RTC module to discharge static, especially in dry weather.
3. **Never feed LED power through the Arduino.** The Arduino's 5V pin is for logic power to the MOSFET modules only (~100mA total). The LEDs get their power directly from the external 5V supply through the bus wire.

## Before You Start

**WARNING: The thick wire running across the back of the LED board is the common anode bus. It powers every LED on the clock. DO NOT cut, unsolder, or disconnect it. If you accidentally break it, every LED downstream of the break will stop working.**

Take a photo of the back of the board as-is. You'll want to compare later if something isn't working.

---

## Phase 1: Identify and Label Word Groups (no soldering)

Each word on the front panel has a cluster of LEDs behind it. You need to figure out which cluster maps to which word, and label every one. There are 28 word groups total.

### Step 1: Orient yourself

Hold the clock face-up so you can read the words. Then flip it over. The back is a mirror image — what was on the left is now on the right. Keep the front panel nearby for reference.

On the back, you'll see:
- **The thick bus wire** running across the board — that's the common anode (+5V). Don't touch it.
- **Clusters of LED legs** poking through the board, grouped by word. Each cluster's cathode (-) legs are either already soldered together, have a wire attached, or are bare leads.
- **Some existing wires** (AM/PM, birthday words, buttons) — these are already labeled or identifiable.

### Step 2: Map each cluster to a word using a coin cell

This is the reliable way to identify groups when you can't tell by visual inspection alone.

1. Hold a **CR2032 coin cell battery** with the **positive side** (the wider face with the "+" marking and model number printed on it) touching the bus wire or any anode lead
2. Touch the **negative side** (the plain, smaller face on the bottom) to a bare cathode lead or wire from a cluster
3. The LEDs in that cluster should glow dimly — look at the front panel to see which word lit up
4. Stick a piece of masking tape near that cluster and write the word name on it

**If a group doesn't light with the coin cell,** try in a darkened room. White LEDs need ~3V to turn on and the coin cell is marginal after accounting for the resistor drop. If you still see nothing, mark the group as "unknown" and verify it later in Phase 9 with the full 5V supply. A non-glowing group at this stage does not necessarily mean dead LEDs.

Work row by row across the front panel. The 28 groups are:

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

**Groups that already have wires** (AM/PM, birthday words, buttons): test them the same way to confirm which is which, then label them.

### Step 3: Verify your count

You should have exactly 28 labeled groups when done. If you're short, you probably missed a group where the cathode leads are hidden or soldered close to the bus wire — look carefully around the edges of existing clusters.

---

## Phase 2: Wire Cathode Leads (soldering)

**Disconnect all power before soldering.**

For each word group that doesn't already have a cathode wire:

1. Strip about 1/2 inch of insulation off one end of a piece of hookup wire (22 AWG or thicker)
2. Solder it to the cathode leads of that word group (all the cathode leads for one word connect together)
3. Leave about 6-8 inches of wire — long enough to reach a MOSFET module mounted nearby
4. Use different color wires for different zones if possible (helps with debugging)

**For groups that already have wires** (AM/PM, birthday words, buttons): leave them as-is. They just need to be connected to the right place later.

When done, you should have 28 labeled cathode wires hanging off the back of the board, plus 2 button wires. The common anode bus stays untouched.

---

## Phase 3: Mount MOSFET Modules

The 7 MOSFET modules mount on the back panel near the words they control. Each module is about 2.6" x 2.6".

Use small screws, standoffs, or double-sided tape to mount them. They don't carry mechanical load — they just need to stay in place.

**Module placement (viewed from back of board):**

```
┌─────────────────────────────────┐
│  [Module 1]         [Module 2]  │  ← Top
│  IT IS,TEN,         TWENTY,     │
│  HALF,QUARTER       FIVE(min),  │
│                     MINUTES,PAST│
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

Don't mount the Arduino Mega yet — do that in Phase 10 after testing.

---

## Phase 4: Connect Word Wires to MOSFET Screw Terminals

**Disconnect all power before wiring.**

Each MOSFET module has two sides. Look at the board markings on your specific module — labels vary between NOYITO batches (you may see "OUT"/"GND", "DC+"/"DC-", "IN1"/"S1", etc.). The layout is always the same:

- **Output side (screw terminals):** 4 channels, each with **two** screw terminals — one for the load (your cathode wire) and one for GND. The screw terminals are the larger connectors you tighten with a screwdriver.
- **Input side (pin headers):** VCC, GND, and 4 signal pins. These are the smaller pins/headers on the opposite edge. This is where the Arduino connects (Phase 5).

If the labels on your modules don't match the names used in this guide, look at the product listing photos for your specific NOYITO module to orient yourself.

### Wiring each channel

For each channel on each module:

1. Connect the **cathode wire** for that word to the channel's **load/OUT terminal** (not the GND terminal)
2. The **GND terminals** on the output side are internally connected to the module's input-side GND on standard NOYITO modules. You do not need to wire them separately — they will get their ground connection when you wire the input side in Phase 5, and ultimately connect to the power supply GND in Phase 8. If in doubt, use a multimeter in continuity mode to confirm the output-side GND and input-side GND are connected on your specific module.

**For each channel:** loosen the load/OUT screw terminal, insert the cathode wire for that word, tighten the screw.

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

**Disconnect all power before wiring.**

Each MOSFET module has input pins on the logic side (the side without screw terminals). These connect to the Arduino Mega with jumper wires.

**Each module needs:**
- **VCC** → Arduino 5V
- **GND** → Arduino GND
- **CH1-CH4 input** → specific Arduino digital pin

Use male-to-female jumper wires for this.

### Power connections first

All 7 modules need 5V and GND on their input-side pins. This is just logic power (milliamps) to run the MOSFET chips — the actual LED current comes from the external power supply through the bus wire.

You can daisy-chain: run 5V from the Arduino's 5V pin to Module 1's VCC, then jump from Module 1's VCC to Module 2's VCC, etc. Same for GND. Or use a small breadboard/terminal block as a GND distribution point — this is cleaner than chaining through one Arduino GND pin, especially since the power supply GND also needs to join this common ground (Phase 8).

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

### Checkpoint: early smoke test (recommended)

Before adding more wiring, do a quick test to catch mistakes early. Temporarily connect the power supply: +5V directly to bus wire, and GND to the MOSFET modules' GND chain (the daisy-chained GND wire from earlier in this phase), **not** only to the Arduino GND pin. LED return current should flow directly back to the power supply through the module GND chain, not through the Arduino's GND trace. If you have the fuse holder ready, wire it inline — otherwise keep this test brief.

Upload and run the pin-sweep test:

```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega test-sketches/pin-sweep
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=9600
```

Each word should light up for 2 seconds while the serial monitor names the expected word. If a word doesn't light or the wrong one lights, fix it now — it's much easier to debug 28 signal wires than 78 total connections.

**Note:** The word THREE (pin 13) will flash briefly on startup. This is normal — the Arduino bootloader blinks pin 13 during boot. It's cosmetic and will not affect normal operation.

**Disconnect the power supply and USB before continuing.**

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

**Disconnect all power before wiring.**

The LEDs need external 5V from the power supply, not from the Arduino.

**Your power supply has a barrel connector.** It looks like it should plug into the Arduino's barrel jack, but **do not do this** — the Arduino barrel jack expects 7-12V input for its onboard voltage regulator. Feeding 5V into it will not power the Arduino reliably. Instead, use a **DC barrel jack breakout adapter** — a small board that accepts the barrel plug and exposes screw terminals or bare +5V and GND wires.

1. Plug the power supply's barrel connector into the breakout adapter.
2. Connect an **inline fuse holder with a 3A fast-blow fuse** to the breakout adapter's +5V terminal. Connect the other end of the fuse holder to the common anode bus on the LED board. This protects against short circuits — without it, the 4A supply can overheat wires if something shorts.
3. Connect the breakout adapter's GND terminal to the common GND point (the terminal block or breadboard where the Arduino GND and all module GNDs meet). All grounds in the system must be connected together.
4. The Arduino is powered separately via USB during testing. For permanent use, plug a USB power adapter into the Arduino's USB port.

**Put the 1000uF electrolytic capacitor across +5V and GND** where the power supply enters the board (on the bus-wire side of the fuse). Electrolytic capacitors are polarized — the longer leg is positive and goes to +5V, the shorter leg (also marked with a minus-sign stripe on the housing) goes to GND. **Getting this backwards WILL damage the capacitor — it may pop loudly, leak, or vent hot caustic electrolyte. Double-check polarity before connecting power.**

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

If you already ran this at the Phase 5 checkpoint and everything was correct, you can skip this test.

### Test 3: Word Test
```bash
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega test-sketches/word-test
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=9600
```
The sketch prints valid word names on startup. Use lowercase with no spaces or underscores (e.g., `itis`, `tenmin`, `quarter`, `fivemin`, `fivehr`, `tenhr`). Each word command adds to the display — it does not replace the previous word. Type `off` to clear all words before testing the next one. Type `all` to verify every LED works.

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
The clock should display the current time in words. Use buttons to set the correct time (hour button = +1 hour, minute button = +5 minutes). **Tap briefly** for a single increment — the buttons auto-repeat when held down (4 times per second after half a second).

---

## Phase 10: Close It Up

Once everything works:

1. Mount the Arduino Mega securely inside the frame (screws, standoffs, or double-sided tape)
2. For permanent power, plug a USB power adapter into the Arduino's USB port. The 5V LED power supply (via barrel breakout) and the USB adapter can share a wall outlet or power strip.
3. Route the power cable(s) through the hole in the frame
4. Slide the back panel into the frame grooves
5. Hang it up or set it on a shelf

---

## Troubleshooting

**No LEDs light up at all:**
- Is the 5V power supply plugged in and turned on?
- Did the fuse blow? Check or replace the 3A fuse.
- Is there a common GND between the Arduino, MOSFET modules, and the power supply? All grounds must be connected.
- Check the common anode bus wire — is it connected to +5V from the supply (through the fuse)?
- Check the MOSFET output-side GND path — the modules' input-side GND must be connected to the common GND.

**One word doesn't light up:**
- Check the screw terminal — is the cathode wire in the load/OUT terminal (not the GND terminal)?
- Is the screw tight?
- Check the jumper wire from module to Arduino — correct pin?
- Try the word-test sketch and type that word name

**Wrong word lights up:**
- Pin mapping needs adjustment. Note which word lit for which pin during the pin sweep test, then update config.cpp
- If you change config.cpp, also update the WORD_PINS arrays in test-sketches/pin-sweep/pin-sweep.ino and test-sketches/word-test/word-test.ino to match

**THREE flashes briefly on startup:**
- Normal. The Arduino bootloader blinks pin 13 during boot, and THREE is wired to pin 13.

**RTC shows wrong time:**
- Use the buttons to set it. Hour button advances by 1 hour, minute button advances by 5 minutes
- If the time resets when you unplug: replace the CR2032 battery on the RTC module

**LEDs light up but are dim:**
- Check that the power supply GND is connected to the common GND point shared by all MOSFET modules. If the ground path runs only through the Arduino, LED current is bottlenecked through the Arduino's GND trace, causing dim output and potentially damaging the Arduino.
- Verify the power supply is delivering 5V (not sagging under load). Use a multimeter on the bus wire.

**All LEDs blink on and off in a loop:**
- The RTC module wasn't found. Check SDA (pin 20) and SCL (pin 21) wiring

**Capacitor popped, leaked, or smells bad:**
- Disconnect power immediately. The capacitor was installed backwards.
- Remove the damaged capacitor, clean any leaked electrolyte with isopropyl alcohol, and inspect nearby components for damage.
- Install a new 1000uF capacitor with correct polarity (long leg to +5V, striped side to GND) and double-check before reconnecting power.
