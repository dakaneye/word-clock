# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Arduino word clock — displays time as English words on a laser-cut panel. Arduino Mega 2560, DS3231 RTC, MOSFET drivers. Has a birthday mode (May 4th) that alternates "HAPPY BIRTH DAY CHELSEA" with the time.

## Build & Test

```bash
# Compile (requires: brew install arduino-cli && arduino-cli core install arduino:avr && arduino-cli lib install "RTClib")
arduino-cli compile --fqbn arduino:avr:mega word-clock

# Upload to connected Arduino
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega word-clock

# Run native tests (compiles with clang++, no Arduino dependencies)
cd test && make test

# Clean test artifacts
cd test && make clean
```

CI runs both native tests and Arduino compilation (including test-sketches) on every push/PR to main.

## Architecture

The core logic separation: `time_to_words` is a pure function (hour, minute, isPM → WordSet of which words to light) with no Arduino dependencies. This is the only module with native tests.

All other modules (`display`, `clock`, `buttons`, `birthday`) depend on Arduino libraries and are only verified via `arduino-cli compile` and hardware test-sketches.

Key types:
- `Word` enum in `config.h` — one value per word group on the physical panel (28 total)
- `WordSet` in `time_to_words.h` — fixed-size array of active words (max 8)
- `WORD_PINS[]` in `config.cpp` — maps each Word enum to an Arduino Mega pin

## Customization Points

All user-customizable values are in the top section of `config.h`: name word (`W_NAME`), birthday date, and cycle timing. Changing the name also requires updating the Word enum and `birthday.cpp`. See `docs/customization.md`.

## Testing Approach

Tests in `test/test_time_to_words.cpp` use a minimal hand-rolled framework (no test library). They `#include` the `.cpp` files directly with Arduino stubs to compile natively. Tests verify every 5-minute block, hour transitions, AM/PM, and edge cases.

`test-sketches/` are standalone Arduino sketches for hardware validation — not automated tests.

## Build Journal

This project maintains a build journal at `docs/build-journal.md` as raw material for a future blog post. It captures planning sessions, hardware debugging moments, surprises, wrong turns, decisions, and meta-observations — the narrative detail that gets lost in code and commit messages but makes for interesting writing.

**Append to it periodically when something journal-worthy happens:**
- A planning decision with meaningful tradeoffs or reasoning worth recording
- Hardware debugging that reveals a surprise ("whoa" moments, wrong assumptions, new techniques)
- A wrong turn or mistake that taught something
- Hitting a milestone worth marking
- A meta-observation about the process, tooling, or collaboration pattern

**Don't journal every tool use or routine commit.** Use judgment. The goal is the kind of signal that would make a reader lean in.

**Format:**
- Dated sessions as `## YYYY-MM-DD — <session theme>`
- Free-form headings beneath (`### What the photos taught us`, `### Surprise #N`, etc.)
- Honest tone — include the confusion and the wrong turns, not just the successful path
- First-person "the user" / "I" is fine; the blog author will rewrite
- Keep a "Running follow-up list" at the bottom for things noted but not yet resolved

**Commit strategy:** commit journal updates as their own commits so the progression is preserved. Don't bundle journal edits with unrelated code changes.
