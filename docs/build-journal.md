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

## Running follow-up list

Things to do once we're past the current debug:

- Resolve the 6V power supply question (replace supply, add regulator, or separate Arduino power)
- Add a note to the assembly guide about verifying supply voltage BEFORE wiring anything
- Possibly: add a note recommending soldered wires over alligator clips for permanent testing
- Possibly: add the "buy a multimeter" recommendation to the materials list
- Optional: collapse `W_HAPPY`/`W_BIRTH`/`W_DAY`/`W_CHELSEA` into one `W_BIRTHDAY` enum + update `birthday.cpp`, the test sketches, and `customization.md` (cosmetic cleanup, not required for function)
