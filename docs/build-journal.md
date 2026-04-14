# Word Clock Build Journal

Notes from the planning, assembly, and debugging sessions — raw material for a future blog post. Written in real time as the build progresses, warts and all. Kept as notes rather than polished prose so the narrative survives with its original confusion, wrong turns, and moments of "oh."

The project started in 2015 as an Arduino + shift-register build for a birthday gift and got shelved. Revived in early 2026 with a simpler hardware design (Arduino Mega + MOSFET modules) and a gift deadline around a specific May 4th.

---

## 2026-04-08 — Planning session: "how to cleanly mount and wire"

### The ask

The user had just reviewed the original assembly guide and felt it was missing critical guidance on physical mounting and wiring. Phase 3 basically said "stick the modules somewhere and use screws or tape" with no real technique. No discussion of wire routing, strain relief, color coding, bundling, or how to lay out the Arduino relative to the MOSFET modules. Quote: "i feel that the assembly guide neglects instructions on how to cleanly wire and place mounts/mosfets."

The ask: plan what can be planned *now*, while waiting for a barrel jack adapter and a multimeter to arrive in the mail.

### What the photos taught us

Five photos of the wooden frame and LED board transformed the plan. Things that would've taken multiple wrong turns to discover otherwise:

- **Frame depth ~2.25"** — plenty of room for components to stack inside
- **Existing wires exit the bottom-left corner** (buttons, AM/PM, birthday words) — the build is already asymmetric; the Arduino has to live near that corner
- **The bus wire is serpentine, not a straight rail** — it weaves between LED clusters. Any attempt to mount modules flush against the back of the LED board would collide with it
- **L-brackets occupy the interior frame corners** — components can't sit in the corners
- **Button wires are only ~1.5"** — forces the Arduino to live in the bottom-left so the buttons reach without splicing
- **A clear plastic insert, pre-drilled from a previous mounting attempt, existed in the parts pile**

### The plastic insert was the architectural unlock

Early in the planning I was picturing "mount modules directly to the wooden frame somehow." Once the plastic insert showed up in a photo, the whole strategy pivoted:

- Mount every component (MOSFETs, Arduino, RTC, GND bus) to the plastic insert, not to the frame or the LED board
- The insert sits parallel to the LED board with a gap between them — the serpentine bus wire lives in that gap, untouched
- The whole insert pulls out as a unit for debugging, the LED board stays put
- No drilling into the wooden frame
- Reuses existing screw holes from a previous mounting attempt

This wasn't in the original assembly guide at all. It's the kind of thing I only found by actually looking at the photos the user had already put in the repo. Lesson for future planning sessions: **the photos are part of the spec. Read them.**

### Decisions locked in during planning

- **Mounting method:** M3 machine screws + nuts through the plastic insert. Holes can be melted with a soldering iron tip (no drill required) or drilled with a small bit.
- **Arduino location:** bottom-left corner (the original guide had it bottom-right). Moved to accommodate the 1.5" button wires without splicing.
- **Wire color scheme:**
  - **Red** = +5V always
  - **Black** = GND always
  - **Yellow / White / Green / Blue** = CH1 / CH2 / CH3 / CH4 on every MOSFET module
  - Channel identity is encoded in color; module identity comes from physical position. No labels needed — debugging by color from across the room.
- **Common ground:** 4" of solid 18 AWG bare copper wire mounted to the insert along the bottom edge. Every black wire in the system solders directly to it. No terminal blocks, no breadboards — just a solder bus.

### The birthday word discovery

I asked the user: "are the four birthday words (HAPPY, BIRTH, DAY, CHELSEA) on separate cathode wires, or tied together somehow?" The answer: they're **daisy-chained on the cathode side**. All four birthday word groups physically share one cathode return.

My first reaction: "oh no, the code controls them as four separate MOSFET channels, this breaks the build."

My second reaction after reading `birthday.cpp` and `display.cpp`: "wait, it actually works unchanged."

The firmware always sets all four birthday-word pins together during the birthday display phase, and `displayShow()` clears all pins before each update. So the four pins **always toggle as a group**. If only one of the four MOSFET channels is physically wired to the daisy chain, it still toggles all four LEDs on/off as a unit — because any one of them grounds the chain, which grounds all four word groups.

Result: **one cathode wire instead of four, three unused MOSFET channels, zero code changes.** The wire-saving interpretation of "wired together" also turned out to be the one that required the least engineering. I've rarely seen a constraint collapse so cleanly.

The commit notes the "collapse to W_BIRTHDAY in code" as purely cosmetic cleanup — not required for function. Skippable unless someone wants the three freed pins later.

### What shipped

One commit, pushed to `main`:

- **`docs(assembly): mount and wire on plastic insert`** — ~215 added lines, ~37 removed
  - Phase 2.5 (new): dry-fit layout on the plastic insert + multimeter check for birthday cathode wiring
  - Phase 3 (rewritten): M3 screw mounting on the plastic insert with module orientation rules
  - Phase 4 preamble (new): wiring technique — color scheme, pre-cut/strip/tin, strain relief, bundle-by-module, wire highway
  - Phase 4 birthday consolidation note
  - Phase 4.5 (new): build the common ground bus before any module wiring
  - Phases 5 / 7 / 8 / 10 updated for bottom-left Arduino, GND bus references, and insert reseating
  - Troubleshooting entry for the daisy-chained birthday words case

### Meta-observations from the planning session

- **Photos > spec.** The user had already added photos to `docs/photos/` earlier. I should have been looking at them much earlier in the planning process instead of asking questions whose answers were already on disk.
- **The right level of planning is bounded by real constraints, not by ceremony.** I was tempted to reach for GSD's planning commands but the scope was a single documentation rewrite. One planning conversation and one commit was the right shape.
- **Ask "what do you have?" before "what should we buy?"** The plastic insert, the wire colors, the M3 hardware question, the GND bus approach — every one of these got simpler once I stopped assuming missing parts and asked what was in the pile.

---

## 2026-04-09 — Multimeter day: first hands-on, many surprises

### Setup attempt

Barrel jack adapter and multimeter both arrived. The user wired it up:

- White hookup wire: barrel jack + terminal → white alligator clip → common anode bus wire
- Red hookup wire: barrel jack − terminal → green alligator clip → "IT" cathode lead

(Side note for the blog: the alligator clip's metal tooth was green but the wire attached to it was red, and I kept calling the whole assembly a "red clip" for half the conversation until the user corrected me. Lesson: when debugging, agree on vocabulary up front.)

Expected: "IT" lights up.
Actual: nothing.

### Surprise #1: the power supply is 6V, not 5V

The first useful thing the new multimeter did was reveal that the "5V 4A" power supply the user had been planning around was actually a **6V** wall wart. Check the label. It says 6V.

This matters a lot, because:

- The Arduino Mega's 5V pin **bypasses** the onboard voltage regulator. Whatever you feed in is what the ATmega2560 sees as Vcc.
- The ATmega2560's absolute maximum operating voltage is 5.5V.
- 6V into the 5V pin is outside spec and risks damaging the chip — either immediately, or cumulatively over time, or showing up as flaky behavior that looks like bugs.

The LEDs themselves are fine at 6V: with the 470Ω series resistors, each LED draws ~6.4 mA, comfortably within its safe range and actually a little brighter than at 5V. So the LED side is unaffected.

Options for fixing the Arduino side:
1. **Return the 6V supply, buy a proper 5V 4A regulated supply.** Cleanest. Cheap. Recommended.
2. Add a 5V buck converter or linear regulator in the Arduino's power path.
3. Power the Arduino via USB and use the 6V supply only for the LED bus, keeping the grounds tied.

The assembly guide still says 5V throughout. It'll need a note added when this is resolved. Added to the follow-up list.

**Lesson:** assume nothing about parts labels until you've verified them. The user had been describing the supply as "5V 4A" in conversation and I'd been writing docs based on that description. Neither of us had actually read the label.

### Surprise #2: nothing lit up, but the multimeter saw 6V everywhere

Probes in the right jacks (black to COM, red to V/Ω/µAmA), dial on 20V DC. The readings came in cleanly:

- Between bus wire and "IT" cathode: **+6V**. Positive sign, so polarity is correct.
- Between bus wire and a different word's cathode (floating, no clip): **~1V**. Weirdly low, but floating nodes do unpredictable things when a high-impedance meter is peeking at them.

Moved the green clip to three different word clusters in sequence. **None lit up.** This meant the problem was systemic, not cluster-specific.

Suspects (in rough priority order):
1. The white clip isn't actually on the bus wire (got clipped to some other stray wire)
2. One of the clips is biting insulation or flux instead of bare copper
3. Polarity is reversed and the user is misreading the multimeter
4. The bus wire has a break between the clip point and the LEDs
5. Both clips are on the same rail somehow

### Surprise #3: the finger trick

Somewhere mid-debug the user tried touching the TEN cathode lead with a finger — the green clip was dangling in air, white clip still on the bus. **All 3 LEDs in TEN lit up.** "whoa."

Electrically, what was happening:

- The +6V bus was connected to TEN's anodes via the white clip
- Touching the TEN cathode with a finger gave the circuit a weak return path through body resistance (tens of kΩ)
- Somewhere else in the user's posture/contact, there was an incidental ground path back to the supply's negative — possibly through the multimeter probe they were holding, the bench, or capacitive coupling through the wall wart
- That's enough current to visibly light LEDs at ~tens of microamps

**This confirmed three things simultaneously:**

1. The white clip IS on the real bus wire. (If it weren't, the finger trick couldn't work.)
2. The power supply IS delivering voltage. (Same.)
3. TEN's LEDs are electrically intact.

So the white-clip side of the chain was fine. The problem had to be on the green-clip side. Most likely: **the green clip isn't actually making electrical contact with the cathode lead when it's clipped on.** Plausible reasons:

- The cathode leads are coated in dried rosin flux from old soldering (insulating)
- The clip's tooth is landing on insulation instead of copper
- The clip lead has an internal break at the crimp (wire not bonded to the metal teeth inside the clip boot)

### Still debugging

Active hypotheses to test next:

- Verify the green clip + red wire assembly's end-to-end continuity with the multimeter
- Try "body in the loop" deliberately: grip the green clip's metal tooth with one hand, touch a cathode with the other. If this lights LEDs on multiple words, all the clusters are intact and the clip-to-cathode contact is the only blocker.
- Scrape cathode leads with a knife blade to expose bare copper before clipping

### Meta-observations from this session

- **Finger-as-diagnostic-probe is a real technique.** Body resistance is high enough to be safe at 6V but low enough to visibly light an LED. It rules in or out a huge space of failures in seconds.
- **When you can't distinguish "wire broken" from "wire contacts open," continuity mode on the multimeter is the tiebreaker.** Voltage mode just tells you the potential at a point; it doesn't tell you if that point is part of a low-impedance loop.
- **Cheap alligator clip leads are a constant source of hardware flake.** The crimp inside the plastic boot is often the first thing to fail. Worth learning to solder wires directly instead for anything that needs to be reliable.
- **Chasing the voltage reading can mislead you.** I spent a while trying to interpret the "1V" reading on the floating cathode as if it were meaningful. It wasn't — it was just a floating node with capacitive coupling to whatever else was nearby. Floating nodes read whatever they want. The signal was "this node isn't grounded," not "this node is at exactly 1V."
- **Conversation vocabulary matters.** I spent four messages saying "the red clip" when the user actually had a green clip with a red wire attached. Neither of us noticed until the user got frustrated and corrected it. For future debug sessions: agree on names for things explicitly before diagnosing.

---

## 2026-04-13 — The breakthrough: common cathode, not common anode

### Hours of wrong-tree debugging

We spent the entire April 9 session and part of today chasing phantom problems: alligator clip contact, barrel jack adapter continuity, wire breaks, bus wire identification. Every individual component tested fine. Voltage was present. But no LED would light.

The finger trick on TEN was the most misleading clue. It worked once, couldn't be reproduced reliably, and sent us down a path of debugging clip-to-cathode contact quality. In hindsight, it was probably a fluke — static discharge, or incidental body contact completing a circuit through an accidental path. We built an entire diagnostic theory on a single unreproducible observation.

### The actual root cause

The assembly guide said "common anode." The board is actually **common cathode.**

- The thick serpentine bus wire connects all LED **cathodes** (GND side), not anodes
- The individual word wires connect to each word's LED **anodes** (+V side)
- To light a word: +5V on the word wire, GND on the bus
- We had been doing the opposite: +5V on the bus, GND on the word wire = every LED reverse-biased = nothing lights

The discovery came from going back to absolute basics. Instead of probing through the board's wiring, we tested a single LED directly on its two legs with the Arduino's 5V and GND pins. It lit up — proving the LEDs were alive. And the leg that received +5V connected to what we'd been calling the "cathode wire," not the "anode bus."

Every LED on the board is fine. Every wire is fine. The clips were fine. The barrel jack adapter was probably fine. We were just pushing current backwards through every LED for two sessions.

### The cascade of simplification

Once we knew it was common cathode, the user asked the right question: "Can you take a step back and think about what the simplest possible path to completion is?"

The answer turned out to eliminate most of the hardware:

**Original plan (common anode + MOSFET modules):**
- 7 NOYITO 4-channel MOSFET modules mounted on a plastic insert
- 28 cathode wires from LED board to MOSFET screw terminals
- 28 signal jumpers from MOSFETs to Arduino
- VCC + GND daisy chain across all 7 modules
- Common ground bus (18 AWG copper wire)
- External 5V power supply + barrel jack adapter + inline fuse + capacitor
- M3 screws, zip ties, wire highway, color-coded bundles

**Actual solution (common cathode + direct pin drive):**
- Wire each word's anode directly to an Arduino Mega digital pin
- Wire the bus to Arduino GND
- Power via USB
- That's it

The MOSFET modules are unnecessary — the Arduino's pins can source enough current to drive the LEDs directly. The external power supply is unnecessary — USB provides plenty for ~200mA peak draw. The barrel jack adapter, fuse, capacitor, ground bus, plastic insert mounting hardware — all unnecessary.

The firmware didn't even need changes. `digitalWrite(pin, HIGH)` sources +5V from the pin to the word's anode, current flows through the LEDs to the bus (GND), LED lights up. Same logic as the MOSFET design, just without the MOSFET in the middle.

### The N-channel MOSFET incompatibility

Even if we'd wanted to use the NOYITO modules, they wouldn't have worked for common cathode. N-channel MOSFET modules are low-side switches — they connect a load to GND when activated. Common cathode needs high-side switching — connecting a load to +5V. The modules the user bought were fundamentally incompatible with the board's wiring. They need to be returned.

### It works

From "common cathode discovery" to "working clock on the wall" took about an hour:
1. Label each word by touching +5V to its anode wire (with GND on the bus) — confirmed all 28 word groups work
2. Wire each word's anode to its assigned Arduino pin per `config.cpp`
3. Wire the bus to Arduino GND
4. Wire the DS3231 RTC (4 wires)
5. Skip the buttons for now (5 wires per button, unknown pinout, not needed for basic operation)
6. Upload the firmware via `arduino-cli`
7. Upload a quick time-setting sketch to set the RTC to the correct time
8. Re-upload the clock firmware
9. "IT IS HALF PAST TWO PM" — first correct time display

The clock updates every 5 minutes. Birthday mode (May 4th) is untested but the birthday LEDs are multicolored (rainbow), which is a nice surprise nobody mentioned in the original design.

### Parts that turned out to be unnecessary

- 7x NOYITO 4-channel MOSFET modules → return
- 5V power supply → unnecessary
- DC barrel jack breakout adapter → unnecessary
- Inline fuse holder + 3A fuse → unnecessary
- 1000µF capacitor → unnecessary
- M3 screws + nuts → unnecessary (no plastic insert mounting needed)
- 18 AWG copper wire for GND bus → unnecessary
- Heat shrink tubing → unnecessary

### Parts that DID get used

- Arduino Mega 2560
- DS3231 RTC module
- 6V power supply (through the Arduino's barrel jack — the onboard regulator handles the 6V→5V drop despite being technically below the 7V spec. Works fine in practice, same as it did with the original Uno)
- Hookup wire (yellow for all 28 word signals, black for GND, red for RTC VCC)
- The existing LED board, buttons, and wooden frame from 2015
- A multimeter (Klein MM325) — bought for this project, paid for itself ten times over

### What got done

- Labeled all 28 word groups using Arduino 5V + GND as a probe
- Soldered permanent wires from each word's anode lead to the correct Arduino pin
- Drilled two routing holes in the plastic insert (one for pins 2–13, one for pins 20+)
- Wired the RTC (4 wires: VCC, GND, SDA→20, SCL→21)
- Identified button switch contacts using multimeter continuity mode (white + green wires on 5-wire illuminated buttons — the other 3 wires are for the button LEDs, unused)
- Wired both buttons (white→pin 38/39, green→GND)
- Wired the common cathode bus to Arduino GND
- Tested birthday mode by setting the RTC to May 4th — rainbow birthday LEDs alternate with the time display every 5 seconds
- Powered via the 6V wall wart through the Arduino's barrel jack

### The RTC is broken too

After the clock was assembled and running, the time wasn't advancing. Added diagnostic output (printing RTC seconds + millis() every second) and got hard evidence:

- 117 real seconds elapsed (millis)
- RTC only counted 50 seconds
- **The DS3231 oscillator is running at 43% speed**

A healthy DS3231 drifts ±2 parts per million. This one is off by 57%. The crystal or oscillator circuit on the 11-year-old module has degraded. Not fixable in software.

Replacement DS3231 module ordered ($8 for a 2-pack, arrives April 15).

### Interim fix: millis()-based timekeeping

Rather than wait for the replacement, we swapped `clock.cpp` to use the Arduino's own `millis()` for timekeeping. The Arduino Mega's 16MHz crystal drifts ~±4 seconds per day — more than adequate for a wall clock.

Trade-offs vs RTC:
- Time resets to midnight on any power loss or Arduino reset (including opening the serial port — learned this the hard way when a diagnostic check erased the time)
- No date tracking, so birthday mode is disabled
- Drift of ~4 seconds/day vs ~1 minute/year with a working DS3231

Both issues go away when the replacement module arrives.

Gotcha discovered during debugging: opening the serial port on macOS toggles the DTR line, which resets the Arduino Mega. With RTC-based timekeeping this is harmless (RTC remembers). With millis-based timekeeping, it wipes the clock. **Can't use serial monitoring without losing the time.** This made verifying the fix require trusting the display rather than reading serial output.

### The final product (for now)

A word clock that displays the time in English ("IT IS TWENTY FIVE TO THREE PM"), updates every 5 minutes, and has hour and minute buttons for time setting. Powered via the 6V wall wart through the Arduino Mega's barrel jack.

Birthday mode (rainbow LEDs showing "HAPPY BIRTH DAY CHELSEA" on May 4th) was tested and works, but is disabled until the replacement RTC arrives (needs date tracking).

Total active components: 1 Arduino Mega, ~100 white LEDs, a handful of rainbow birthday LEDs, 2 push buttons, 1 6V wall wart. No MOSFETs, no shift registers, no external driver ICs, no working RTC (temporarily). The Arduino's digital pins drive the LEDs directly and its crystal keeps time.

### Meta-observations

- **"Common anode" was in the original project description and nobody questioned it.** The assembly guide, the CLAUDE.md, the README, the code comments — every document assumed common anode because that's what was stated at the start. The actual board was never verified. Lesson: **verify your assumptions about existing hardware before designing a system around them.**
- **The simplest solution was invisible while we were deep in the MOSFET plan.** We spent days planning mounting techniques, wire color schemes, ground bus construction, and module orientation — all for hardware that turned out to be unnecessary. The direct-drive approach was always available but never considered because "you need MOSFETs to drive LEDs" was an unquestioned assumption.
- **Sunk cost bias nearly cost more time.** After buying 7 MOSFET modules, the instinct was to find a way to USE them (P-channel replacements, board rewiring, pull-up resistor hacks). The user cut through this by asking "what's the simplest path to completion?" The answer was "don't use MOSFETs at all."
- **A 10-year-old project carries 10-year-old assumptions.** The original 2015 build used shift registers, which might have had a different driver topology. When the project was revived in 2026, the wiring description was reconstructed from memory, and "common anode" was either misremembered or confused with the shift register design. Nobody went back to the board to check.
- **The multimeter paid for itself.** Without it, we'd still be guessing about wire connectivity and LED polarity. The diode test on individual LED legs was the single action that broke the impasse — not the voltage readings, not the continuity tests on wires, but a direct probe of the component itself.
- **"Stop guessing and add a diagnostic" is always the right call.** I spent multiple rounds speculating about why the RTC was slow — dead battery, voltage sag, I2C hangs, button interference. Each theory was plausible but evidence-free. What actually worked: adding a one-line diagnostic print (millis + RTC seconds every second) that showed the oscillator running at 43% speed. Two minutes of data beat two hours of theorizing.
- **Old hardware fails in unexpected ways.** LEDs last forever. Crystals don't. An 11-year-old DS3231 module with a dead battery and a degraded oscillator was the last failure mode anyone would predict — it's a "precision" component that was running at less than half speed. Lesson: test old components before building around them.
- **Serial port resets are invisible until they bite you.** The DTR-triggered reset on serial open is well-known Arduino behavior, but its consequences change depending on your timekeeping strategy. With an RTC, it's harmless. With millis(), it destroys your clock state. This only became apparent after we'd already switched to the millis() approach — a consequence that wasn't obvious at design time.
- **Don't upload throwaway sketches when the firmware can handle it.** I kept uploading separate time-setting sketches instead of using the buttons the user had already wired. The user rightly called this out. If the firmware has a feature (buttons for time-setting), use it.

---

## Running follow-up list

- ~~Solder permanent wires, cut to length, mount in frame~~ ✓
- ~~Figure out button wiring~~ ✓ (white + green, multimeter continuity)
- ~~Test birthday mode~~ ✓ (rainbow LEDs, alternating display)
- Update `docs/assembly-guide.md` — rewrite for common cathode + direct pin drive
- Update `CLAUDE.md` — project description says "MOSFET drivers"
- Update `README.md` — hardware section lists MOSFET modules
- Update `config.h` / `config.cpp` / `display.cpp` — comments reference common anode + MOSFET switching
- Update `docs/customization.md` — hardware alternatives table references MOSFET approach
- Update test sketches — labels reference MOSFET modules
- Collapse `W_HAPPY`/`W_BIRTH`/`W_DAY`/`W_CHELSEA` into `W_BIRTHDAY` (cosmetic, confirmed daisy-chained)
- Add "verify your board's polarity before designing the driver circuit" to the assembly guide
- ~~Replace DS3231 module~~ ordered, arrives April 15 — swap module, revert clock.cpp to RTC version, set time, re-enable birthday mode
- Remove diagnostic serial output from word-clock.ino (the seconds print) after RTC is verified
- Return NOYITO MOSFET modules
- Write the blog post
