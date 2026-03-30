# Contributing

## Development

```bash
git clone https://github.com/dakaneye/word-clock.git
cd word-clock

# Install Arduino CLI
brew install arduino-cli
arduino-cli core install arduino:avr
arduino-cli lib install "RTClib"

# Compile the main sketch
arduino-cli compile --fqbn arduino:avr:mega word-clock

# Run native tests (macOS/Linux — requires clang++)
cd test && make test
```

## Before Submitting

1. All sketches compile: `arduino-cli compile --fqbn arduino:avr:mega word-clock`
2. Native tests pass: `cd test && make test`
3. Test sketches compile (pin-sweep, word-test, rtc-test, button-test)
4. New functionality has tests where possible

## Pull Requests

- Keep changes focused
- Update tests for new functionality
- Follow existing code style
- If changing pin assignments in `config.cpp`, update the duplicated arrays in test sketches too (they can't `#include` from the main sketch due to Arduino IDE temp dir compilation)
