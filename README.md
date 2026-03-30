# Word Clock

[![CI](https://github.com/dakaneye/word-clock/actions/workflows/ci.yml/badge.svg)](https://github.com/dakaneye/word-clock/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

An Arduino-powered word clock that displays time in English words on a laser-cut birch panel. Built with an Arduino Mega 2560, DS3231 RTC, and MOSFET driver modules.

Includes a birthday mode that alternates "HAPPY BIRTH DAY CHELSEA" with the time display on May 4th.

## Word Grid

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

## Hardware

- Arduino Mega 2560
- DS3231 RTC module (I2C)
- 7x 4-channel MOSFET driver modules (28 word groups)
- ~100 white 5mm LEDs with 470 ohm resistors
- 2 push buttons (hour / minute setting)
- 5V 4A power supply
- Laser-cut birch front panel, walnut frame

## Software Architecture

```
word-clock/         Arduino sketch
  config.h/cpp      Pin assignments, word enum
  time_to_words     Pure logic: hour+minute -> which words light up
  display           MOSFET pin control
  clock             DS3231 RTC read/write
  buttons           Debounce + hold-repeat
  birthday          May 4th alternating display

test/               Native macOS/Linux tests for time_to_words
test-sketches/      Hardware validation sketches (RTC, pin sweep, etc.)
```

## Building

```bash
# Install toolchain
brew install arduino-cli
arduino-cli core install arduino:avr
arduino-cli lib install "RTClib"

# Compile
arduino-cli compile --fqbn arduino:avr:mega word-clock

# Upload (with Arduino connected via USB)
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega word-clock

# Run native tests
cd test && make test
```

## Docs

- [Design Spec](docs/superpowers/specs/2026-03-29-word-clock-design.md)
- [Assembly Guide](docs/assembly-guide.md)
