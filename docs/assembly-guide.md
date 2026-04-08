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
- Clear plastic insert (re-used from a previous build) — your mounting platform for all electronics
- Hookup wire in 6 colors: red, black, yellow, white, green, blue (used per the color scheme in Phase 4)
- M3 machine screws + nuts (~20 of each) for mounting modules to the plastic insert
- ~4" of solid 18 AWG bare copper wire for the common ground bus
- Jumper wires, wire strippers (new)
- Inline fuse holder + 3A fast-blow fuse (for the 5V power line)
- 1000uF electrolytic capacitor (for the power supply — absorbs voltage spikes on startup)

## Tools You Need

- Soldering iron + solder
- Wire strippers
- Small phillips screwdriver (for MOSFET module screw terminals)
- Masking tape + sharpie (for labeling)
- Multimeter (continuity mode required for Phase 2.5; also useful for debugging)
- Either a small drill bit (~3.2mm for M3) **or** the soldering iron tip can melt holes through the plastic insert
- Heat shrink tubing or electrical tape (for splices and strain relief)

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
3. **Never route LED current through the Arduino.** The LEDs draw amps — that current flows from the power supply through the bus wire, through the LEDs, and back to the supply through the MOSFET modules. The Arduino's 5V pin powers the Arduino and MOSFET logic only (~100mA total).

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

### Step 2: Map each cluster to a word using the 5V supply

The CR2032 coin cell method doesn't reliably light white LEDs — 3V nominal drops under load, and with the series resistors on the board there's not enough headroom. Use the actual 5V power supply with a jumper wire as a probe instead.

**One-time setup (safe because nothing else is wired yet):**
1. Plug the power supply's barrel connector into the breakout adapter
2. Wire the breakout's +5V terminal through the inline fuse (3A fast-blow) to the common anode bus on the LED board
3. Leave the breakout's GND terminal free for now — you'll use a jumper wire clipped to it as your probe

**To identify each cluster:**
1. Clip one end of a jumper wire to the breakout's GND terminal
2. Turn on the power supply
3. Briefly touch the other end of the jumper to a bare cathode lead or cathode wire from a cluster — the LEDs in that cluster should light up brightly
4. Look at the front panel to see which word lit up
5. Stick a piece of masking tape near that cluster and write the word name on it
6. Turn off the power supply when you're done probing a cluster

**Safety notes:**
- The inline fuse protects against accidental shorts, but still work deliberately — don't touch the probe to anything except cathode leads
- Only touch one cluster at a time
- If a group doesn't light: check that the jumper is making solid contact with the cathode lead. If it still won't light, mark it as "unknown" — it may be a broken solder joint or dead LED, and you can diagnose it later once the MOSFETs are wired

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

## Phase 2.5: Plan the Layout on the Plastic Insert

You have a clear plastic insert that previously screwed into the wooden frame. This insert is the **mounting platform** for everything: the 7 MOSFET modules, the Arduino Mega, the DS3231 RTC, and a small ground bus wire. Mounting components on the insert (instead of directly on the LED board or the wooden frame) means:

- The insert pulls out as a complete unit for debugging
- The serpentine bus wire on the LED board lives in the gap between the insert and the board, untouched
- You don't drill into the wooden frame
- You can dry-fit the entire layout before committing to drilling

**Do this entire phase before any drilling, soldering, or mounting.**

### Step 0: Verify the birthday word wiring (multimeter)

Set your multimeter to continuity mode. Probe between two birthday word cathode points — for example, a CHELSEA cathode lead and a HAPPY cathode lead.

- **Beep (continuity):** All four birthday words (HAPPY, BIRTH, DAY, CHELSEA) share a single cathode daisy chain. You'll wire them as a single channel — see the "Birthday word consolidation" note in Phase 4.
- **No beep:** Each birthday word has an independent cathode. Wire all four channels per the original Phase 4 module tables.

Both cases work with the existing firmware unchanged. Knowing which case you're in tells you how many cathode wires to cut and solder.

### Step 1: Lay out components on the insert (no adhesive, no drilling)

Place the plastic insert on a flat work surface, oriented the way it will go into the frame. Place each component in its target position without securing anything yet:

- **7 MOSFET modules** in their target positions, oriented with **load side (screw terminals) toward where the LED board will be** and **logic side (pin headers) toward open space**
- **Arduino Mega** in the bottom-left corner — it has to be close to the buttons (button wires are ~1.5" long, can't reach across the board without splicing)
- **DS3231 RTC** next to the Arduino
- **A 4" length of solid 18 AWG copper wire** along the bottom edge of the insert — this becomes the common ground bus

**Module placement** (viewed from behind the clock, after moving the Arduino out of the bottom-right):

```
┌─────────────────────────────────┐
│  [Module 1]         [Module 2]  │  ← Top
│  IT IS,TEN(min),    TWENTY,     │
│  HALF,QUARTER       FIVE(min),  │
│                     MINUTES,PAST│
│         [Module 3]              │  ← Mid-upper
│         TO,HAPPY,               │
│         BIRTH,THREE             │
│                                 │
│  [Module 4]         [Module 5]  │  ← Middle
│  ONE,ELEVEN,        TWO,EIGHT,  │
│  FOUR,DAY           SEVEN,NINE  │
│                                 │
│         [Module 6]              │  ← Lower
│         SIX,TWELVE,             │
│         CHELSEA,FIVE(hr)        │
│                                 │
│  [Arduino+RTC]      [Module 7]  │  ← Bottom
│  [GND bus wire]     TEN(hr),    │
│   ↑                 O'CLOCK,    │
│   buttons + power   AM, PM      │
│   enter here                    │
└─────────────────────────────────┘
```

### Step 2: Verify reach with a string

For each MOSFET module, run a piece of string from its corresponding word cluster on the LED board (above) to the module's load terminal on the insert. Confirm the cathode wire you'll cut later will reach with at least 1" of slack. If any reach is marginal, reposition the module.

For the buttons: confirm the existing button wires will reach Arduino pins 38 and 39 without splicing. **If they don't reach, move the Arduino, not the buttons.**

### Step 3: Avoid the L-brackets

The wooden frame has L-brackets in the interior corners that hold the insert in place. Components on the insert can't sit where the brackets contact. Leave 0.5–1" clearance from each interior corner.

### Step 4: Mark mount points

Once you're happy with the layout, trace each component's corner positions onto the insert with a sharpie or masking tape. These marks guide the drilling/melting in Phase 3.

### Step 5: Photograph

Take a photo of the marked-up insert. If anything shifts during Phase 3 you have a reference to recover from.

---

## Phase 3: Mount Components to the Plastic Insert

**Disconnect all power before starting this phase.**

### Mounting method

**M3 machine screws + nuts** through the plastic insert. The insert is thin enough that you can either:

- **Melt clean holes** with the tip of a hot soldering iron (no drill required, smells a little, very fast), or
- **Drill** with a 3.2mm bit if you have one (cleaner, no fumes)

Hold the insert flat against a scrap board so the iron tip or drill bit doesn't punch through to your work surface.

**Alternative for non-permanent install:** double-sided foam tape (3M VHB or similar). Easier but adhesive can fail in heat over years. Screws are the recommended path for longevity.

### Module orientation (critical)

Each NOYITO MOSFET module has two sides:

- **Load side** (large screw terminals) — must face the LED board so cathode wires don't cross over the module to reach their terminals
- **Logic side** (pin headers) — must face away from the LED board so jumper wires to the Arduino run through clear space

Getting this wrong means every wire crosses every module. Double-check orientation against the layout diagram before screwing anything down.

### Clearance rules

- **~5mm around screw terminals** — you need to fit a screwdriver
- **~10mm around pin headers** — you need to seat and remove jumpers
- **0.5–1" from each L-bracket** — they're the only thing holding the insert in place

### Step-by-step

1. Using your Phase 2.5 marks, identify the screw hole positions for each module (2 holes diagonally — opposite corners of the PCB are sufficient)
2. Melt or drill the holes
3. Place the module over the holes, insert M3 screws from the LED-board side, secure with nuts on the back
4. Repeat for all 7 modules
5. Mount the Arduino Mega with M3 screws through 2 of its corner mounting holes
6. Mount the DS3231 RTC with its mounting hole, or with a small dab of hot glue if it doesn't have one
7. **Mount the GND bus wire:** strip the insulation off a 4" length of solid 18 AWG copper wire so the entire length is bare conductor. Secure it to the insert along the bottom edge with two zip ties through small holes you melt at each end. This is your common ground point — every black wire in the system will solder here

Don't run any wires yet — Phase 4 covers wiring technique before you start cutting and soldering.

---

## Phase 4: Connect Word Wires to MOSFET Screw Terminals

**Disconnect all power before wiring.**

### Wiring technique (read this before cutting any wire)

#### Color scheme

Lock this in before you cut anything. The rule lets you identify any channel from across the room — module identity comes from physical position, channel comes from color.

| Color | Use |
|---|---|
| **Red** | +5V (bus wire feed, Arduino 5V pin, MOSFET VCC daisy chain) |
| **Black** | GND (everything that returns to the common ground bus) |
| **Yellow** | CH1 of every MOSFET module (cathode wire AND logic jumper) |
| **White** | CH2 of every MOSFET module (cathode wire AND logic jumper) |
| **Green** | CH3 of every MOSFET module (cathode wire AND logic jumper) |
| **Blue** | CH4 of every MOSFET module (cathode wire AND logic jumper) |

#### Pre-cut, pre-strip, pre-tin

Batch this work before any soldering. For every wire:

1. Measure the run from source to destination, add 1" slack, mark
2. Cut to length
3. Strip ~1/4" from each end
4. Tin both ends (apply iron to wire, feed solder until strands are coated)

Tinned ends sit cleanly in screw terminals without fraying and pre-flow into solder joints. Doing 30 wires in a batch is much faster than one wire at a time.

#### Strain relief

After tightening a wire into a screw terminal, route it in a small loop and zip-tie the loop to a nearby anchor point on the insert. A tug on the wire pulls on the loop, not the terminal. Without this, a single accidental yank can rip a cathode joint loose.

#### Bundling (by module, not by destination)

Bundle wires **by module**: all 4 cathode wires for Module 3 form one bundle that exits Module 3. All 6 jumpers for Module 3 (4 signal + VCC + GND) form another bundle that runs to the Arduino.

**Why by module:** if you ever reposition or replace a module, the bundle goes with it. Bundling by destination locks the layout permanently.

Use spiral wrap, fabric tape, or a zip tie every ~2".

#### Wire highway

All logic-side jumpers run along **one edge** of the insert, not fanning out across the back. Pick the bottom edge as the highway since the Arduino is in the bottom-left corner. Bundle the highway every 2".

### MOSFET module sides

Each MOSFET module has two sides. Look at the board markings on your specific module — labels vary between NOYITO batches (you may see "OUT"/"GND", "DC+"/"DC-", "IN1"/"S1", etc.). The layout is always the same:

- **Output side (screw terminals):** 4 channels, each with **two** screw terminals — one for the load (your cathode wire) and one for GND. The screw terminals are the larger connectors you tighten with a screwdriver.
- **Input side (pin headers):** VCC, GND, and 4 signal pins. These are the smaller pins/headers on the opposite edge. This is where the Arduino connects (Phase 5).

If the labels on your modules don't match the names used in this guide, look at the product listing photos for your specific NOYITO module to orient yourself.

### Wiring each channel

For each channel on each module:

1. Connect the **cathode wire** for that word to the channel's **load/OUT terminal** (not the GND terminal)
2. The **GND terminals** on the output side are internally connected to the module's input-side GND on standard NOYITO modules. You do not need to wire them separately — they will get their ground connection when you wire the input side in Phase 5, and ultimately connect to the common GND bus in Phase 4.5. If in doubt, use a multimeter in continuity mode to confirm the output-side GND and input-side GND are connected on your specific module.

**For each channel:** loosen the load/OUT screw terminal, insert the (color-matched, tinned) cathode wire for that word, tighten the screw.

### Birthday word consolidation

If your Phase 2.5 Step 0 multimeter check confirmed the four birthday word cathodes (HAPPY, BIRTH, DAY, CHELSEA) are tied together in a single daisy chain, you only need **one** cathode wire for all four words.

Pick whichever of the four birthday MOSFET channels is most physically convenient — Module 3 CH2 (HAPPY), Module 3 CH3 (BIRTH), Module 4 CH4 (DAY), or Module 6 CH3 (CHELSEA). Run a single cathode wire from the daisy chain to that channel's load terminal. **Leave the other 3 birthday cathode terminals empty.**

The firmware drives all 4 birthday word pins together during the birthday phase, so any single wired channel will light all four words. **No code changes are required.** The 3 unused channels still get logic-side jumpers in Phase 5 (the firmware drives them), but their load terminals stay empty.

If the multimeter check showed no continuity — wire all 4 birthday channels normally per the tables below.

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

## Phase 4.5: Tie Module GNDs to the Common Ground Bus

**Disconnect all power before wiring.**

The bare 18 AWG copper bus wire you mounted along the bottom edge of the insert in Phase 3 is the merge point for every ground in the system. Wire the MOSFET modules to it before you start the Arduino-side wiring in Phase 5 — that way the GND distribution is already in place.

For each of the 7 MOSFET modules:

1. Cut a length of **black** hookup wire long enough to reach from the module's input-side GND pin to the bus wire (with 1" slack)
2. Strip and tin both ends
3. Crimp or solder one end into a female jumper that fits the module's GND header pin (or use a male-female jumper wire and skip the crimping)
4. Solder the other end directly to the bus wire — touch the iron to the joint, feed a small amount of solder, hold for ~2 seconds, remove iron
5. Space solder points on the bus wire ~5mm apart so they don't merge

After wiring all 7 modules, you should have 7 black wires fanning off the bus wire, each going to one MOSFET module's GND pin. The bus wire still has room for 2 more solder points: one for the Arduino GND (Phase 5) and one for the power supply GND (Phase 8).

---

## Phase 5: Wire MOSFET Modules to Arduino Mega

**Disconnect all power before wiring.**

Each MOSFET module has input pins on the logic side (the side without screw terminals). These connect to the Arduino Mega with jumper wires.

**Each module needs:**
- **VCC** → Arduino 5V (red wires)
- **GND** → already wired to the common ground bus in Phase 4.5 — nothing more to do here
- **CH1-CH4 input** → specific Arduino digital pin (yellow/white/green/blue per the color scheme)

Use male-to-female jumper wires (or pre-cut hookup wire with crimped ends) for this.

### Power connections first

All 7 modules need 5V on their input-side VCC pin. This is just logic power (milliamps) to run the MOSFET chips — the actual LED current comes from the external power supply through the bus wire.

Daisy-chain the +5V using **red** wires: run from the Arduino's 5V pin to Module 1's VCC, then jump from Module 1's VCC to Module 2's VCC, etc. Or run individual red wires from each module to a +5V rail near the Arduino if you prefer star topology over daisy-chaining.

The Arduino's own GND also joins the common ground bus: run a single black wire from any Arduino GND pin to a free spot on the bus wire and solder it.

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

Before adding more wiring, do a quick test to catch mistakes early. Temporarily connect the power supply: +5V directly to bus wire, and GND to the common ground bus on the insert (where all the MOSFET module GNDs already terminate from Phase 4.5). LED return current should flow directly back to the power supply through the bus wire, not through the Arduino's GND trace. If you have the fuse holder ready, wire it inline — otherwise keep this test brief.

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

The buttons are physically mounted in the bottom-left corner of the wooden frame, with ~1.5" of wire on each lead. **Verify the Arduino reaches the button wires without splicing before you do anything else.** If it doesn't, move the Arduino on the insert before wiring (this is why Phase 2.5 puts the Arduino in the bottom-left).

Each button needs 2 connections: one wire to an Arduino pin, one wire to the common ground bus. The Arduino uses internal pullup resistors, so no external resistor is needed.

| Button | Arduino Pin | Other wire |
|--------|-------------|------------|
| Hour | Pin 38 | GND bus |
| Minute | Pin 39 | GND bus |

If a button wire is too short to reach: solder a short black extension wire (1-2") to the GND lead and heat-shrink the splice. Don't extend the signal lead — keep that as direct as possible to the Arduino pin.

---

## Phase 8: Wire the Power Supply

**Disconnect all power before wiring.**

The LEDs need external 5V from the power supply, not from the Arduino.

**Your power supply has a barrel connector.** It looks like it should plug into the Arduino's barrel jack, but **do not do this** — the Arduino barrel jack expects 7-12V input for its onboard voltage regulator. Feeding 5V into it will not power the Arduino reliably. Instead, use a **DC barrel jack breakout adapter** — a small board that accepts the barrel plug and exposes screw terminals or bare +5V and GND wires. Mount the breakout adapter on the plastic insert near the Arduino so the +5V wire to the bus and the GND wire to the GND bus are short.

The power cable enters the wooden frame from the **bottom-left corner**, alongside the buttons.

1. Plug the power supply's barrel connector into the breakout adapter.
2. Connect an **inline fuse holder with a 3A fast-blow fuse** to the breakout adapter's +5V terminal. Connect the other end of the fuse holder to the common anode bus on the LED board with a red wire. This protects against short circuits — without it, the 4A supply can overheat wires if something shorts.
3. Connect the breakout adapter's GND terminal to the common ground bus on the insert with a black wire. This is the final solder point on the bus wire — Arduino GND, all 7 module GNDs, and now the power supply GND all share this single bus.
4. Run a red wire from the breakout adapter's +5V terminal (before the fuse) to the Arduino Mega's **5V pin**. This powers the Arduino from the same supply — the 5V pin connects directly to the board's 5V rail, bypassing the barrel jack's voltage regulator. Do **not** use the barrel jack — it expects 7-12V input and will not deliver reliable voltage from a 5V supply.
5. During testing, you can power the Arduino via USB instead (just unplug the 5V pin wire). USB and the 5V pin should not supply power simultaneously — use one or the other.

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

1. The Arduino, MOSFET modules, RTC, and ground bus are already mounted on the plastic insert from Phase 3 — no extra mounting needed
2. Disconnect USB. The 5V power supply powers both the LEDs and the Arduino (via the 5V pin) — one cord runs everything.
3. Route the power cable through the bottom-left corner of the frame, alongside the button wires
4. Slide the populated insert back into the frame, secured by the same L-brackets and screws that held it before
5. Slide the back panel into the frame grooves
6. Hang it up or set it on a shelf

If you ever need to debug, you can pull the insert out as a complete unit — the LED board stays in the frame, the bus wire stays untouched, and you have full access to every joint.

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

**Only one birthday word lights up (or all four light at the same time, can't be controlled separately):**
- Expected if you confirmed Option A (cathode daisy chain) in Phase 2.5 Step 0. The four birthday words share a single cathode wire and always light/extinguish together. The firmware drives all four pins together during the birthday phase, so this is correct behavior.
- If you wanted independent control: cut the daisy chain between the four word groups, then run a separate cathode wire from each word to its dedicated MOSFET channel.

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
